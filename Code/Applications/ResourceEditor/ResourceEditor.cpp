#include "ResourceEditor.h"
#include "Tools/Core/Resource/ResourceEditorWorkspace.h"
#include "Engine/Core/Imgui/OrientationGuide.h"
#include "System/Render/RenderViewportManager.h"
#include "System/Input/InputSystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    ResourceEditor::~ResourceEditor()
    {
        KRG_ASSERT( m_pDataBrowser == nullptr );
    }

    void ResourceEditor::Initialize( UpdateContext const& context )
    {
        m_model.Initialize( context );
        m_pDataBrowser = KRG::New<DataBrowser>( m_model );
        m_db.Initialize( m_model.GetTypeRegistry(), m_model.GetRawResourceDirectory() );
    }

    void ResourceEditor::Shutdown( UpdateContext const& context )
    {
        m_db.Shutdown();
        KRG::Delete( m_pDataBrowser );
        m_model.Shutdown( context );
    }

    void ResourceEditor::Update( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
        // Request that game is rendered to texture
        viewportManager.SetUseCustomRenderTargetForViewport( 0, true );

        //-------------------------------------------------------------------------

        UpdateStage const updateStage = context.GetUpdateStage();

        switch ( updateStage )
        {
            case UpdateStage::FrameStart:
            {
                m_db.Update();

                if ( ImGui::BeginMainMenuBar() )
                {
                    DrawMainMenu( context, viewportManager );

                    // Draw Performance Stats
                    //-------------------------------------------------------------------------

                    ImGuiViewport const* viewport = ImGui::GetMainViewport();
                    ImGui::SameLine( viewport->WorkSize.x - 165 );

                    float const currentFPS = 1.0f / context.GetDeltaTime();
                    float const allocatedMemory = Memory::GetTotalAllocatedMemory() / 1024.0f / 1024.0f;
                    ImGui::Text( "FPS: %3.0f | Mem: %.2fMB", currentFPS, allocatedMemory );

                    ImGui::EndMainMenuBar();
                }

                FrameStartUpdate( context, viewportManager );
            }
            break;

            case UpdateStage::FrameEnd:
            {
                FrameEndUpdate( context, viewportManager );
                DrawPopups( context );
            }
            break;
        }

        //-------------------------------------------------------------------------

        // Disable input when we the mouse is not within the editor viewport
        auto pInputSystem = context.GetSystem<Input::InputSystem>();
        KRG_ASSERT( pInputSystem != nullptr );
        pInputSystem->SetEnabled( m_mouseWithinEditorViewport );
    }

    void ResourceEditor::DrawMainMenu( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
        // Needed to ensure that the perf stats get drawn correctly
        ImGui::NewLine();
    }

    void ResourceEditor::DrawPopups( UpdateContext const& context )
    {
        // Get any new warnings/errors and create pop-ups for them
        //-------------------------------------------------------------------------

        auto const unhandledWarningsAndErrors = Log::GetUnhandledWarningsAndErrors();
        for ( auto const& entry : unhandledWarningsAndErrors )
        {
            auto& popupMessage = m_modalPopups.emplace_back( ModalPopupMessage() );

            UUID const ID = UUID::GenerateID();
            popupMessage.m_ID = ( entry.m_severity == Log::Severity::Warning ) ? "Warning##" : "Error##";
            popupMessage.m_ID += ID.ToString().c_str();
            popupMessage.m_channel = entry.m_channel;
            popupMessage.m_severity = entry.m_severity;
            popupMessage.m_message = entry.m_message;
        }

        // Remove closed warnings/errors
        //-------------------------------------------------------------------------

        for ( auto i = 0; i < m_modalPopups.size(); i++ )
        {
            if ( !m_modalPopups[i].m_isOpen )
            {
                m_modalPopups.erase( m_modalPopups.begin() + i );
                i--;
            }
        }

        // Draw pop-ups
        //-------------------------------------------------------------------------

        if ( !m_modalPopups.empty() )
        {
            // Always draw latest first
            auto& message = m_modalPopups.back();

            ImVec4 const titleBarColor = ( message.m_severity == Log::Severity::Warning ) ? ImGuiX::ConvertColor( Colors::Yellow ) : ImGuiX::ConvertColor( Colors::Red );
            ImVec4 const titleBarTextColor = ( message.m_severity == Log::Severity::Warning ) ? ImGuiX::ConvertColor( Colors::Black ) : ImGuiX::ConvertColor( Colors::Black );

            ImGui::OpenPopup( message.m_ID.c_str() );
            ImGui::PushStyleColor( ImGuiCol_Text, titleBarTextColor );
            ImGui::PushStyleColor( ImGuiCol_TitleBgActive, titleBarColor );
            if ( ImGui::BeginPopupModal( message.m_ID.c_str(), &message.m_isOpen, ImGuiWindowFlags_NoSavedSettings ) )
            {
                ImGui::PopStyleColor( 2 );

                ImGui::BeginGroup();
                {
                    ImGui::PushStyleColor( ImGuiCol_Text, titleBarColor );
                    ImGuiX::ScopedFont font( ImGuiX::Font::ExtraLarge );
                    ImGui::Text( KRG_ICON_EXCLAMATION_TRIANGLE );
                    ImGui::PopStyleColor( 1 );
                }
                ImGui::EndGroup();

                ImGui::SameLine();

                ImGui::BeginGroup();
                {
                    ImGui::Text( "Channel: %s", message.m_channel.c_str() );
                    ImGui::Text( "Message: %s", message.m_message.c_str() );
                }
                ImGui::EndGroup();

                //-------------------------------------------------------------------------

                ImGui::NewLine();
                ImGui::NewLine();
                ImGui::SameLine( ( ImGui::GetWindowWidth() - 100 ) / 2 );
                if ( ImGui::Button( "Ok", ImVec2( 100, 0 ) ) )
                {
                    message.m_isOpen = false;
                }

                //-------------------------------------------------------------------------

                ImGui::EndPopup();
            }
        }
    }

    //-------------------------------------------------------------------------

    void ResourceEditor::FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
        //-------------------------------------------------------------------------
        // Create main dock window
        //-------------------------------------------------------------------------

        ImGuiWindowClass mainEditorWindowClass;
        mainEditorWindowClass.ClassId = ImGui::GetID( "EditorWindowClass" );
        mainEditorWindowClass.DockingAllowUnclassed = false;

        ImGuiID const dockspaceID = ImGui::GetID( "EditorDockSpace" );
        ImGuiID dockRightID = 0, dockLeftID = 0;

        ImGuiWindowFlags const windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGuiViewport const* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos( viewport->WorkPos );
        ImGui::SetNextWindowSize( viewport->WorkSize );
        ImGui::SetNextWindowViewport( viewport->ID );

        ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0.0f, 0.0f ) );
        bool const shouldDrawEditorDockspace = ImGui::Begin( "EditorDockSpaceWindow", nullptr, windowFlags );
        ImGui::PopStyleVar( 3 );

        if ( shouldDrawEditorDockspace )
        {
            // Initial Layout
            if ( !ImGui::DockBuilderGetNode( dockspaceID ) )
            {
                ImGui::DockBuilderAddNode( dockspaceID, ImGuiDockNodeFlags_DockSpace | ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton );
                ImGui::DockBuilderSetNodeSize( dockspaceID, viewport->Size );

                dockLeftID = ImGui::DockBuilderSplitNode( dockspaceID, ImGuiDir_Left, 0.25f, nullptr, &dockRightID );

                // Disable tab bars for left node
                ImGuiDockNode* pLeftNode = ImGui::DockBuilderGetNode( dockLeftID );
                pLeftNode->LocalFlags |= ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoCloseButton | ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton | ImGuiDockNodeFlags_NoDockingOverMe | ImGuiDockNodeFlags_NoDockingSplitMe;

                // Flag right node as the central one
                ImGuiDockNode* pRightNode = ImGui::DockBuilderGetNode( dockRightID );
                pRightNode->LocalFlags |= ImGuiDockNodeFlags_CentralNode | ImGuiDockNodeFlags_NoDockingSplitMe | ImGuiDockNodeFlags_NoDockingOverMe;

                ImGui::DockBuilderFinish( dockspaceID );
            }
            else // look up the correct IDs (this is okay to do since we have fixed dock node and dont allow further splitting)
            {
                if ( dockLeftID == 0 && dockRightID == 0 )
                {
                    auto pDockRootNode = ImGui::DockBuilderGetNode( dockspaceID );
                    dockLeftID = pDockRootNode->ChildNodes[0]->ID;
                    dockRightID = pDockRootNode->ChildNodes[1]->ID;
                }
            }

            // Create the actual dock space
            ImGui::DockSpace( dockspaceID, viewport->WorkSize, ImGuiDockNodeFlags_None, &mainEditorWindowClass );
        }
        ImGui::End();

        //-------------------------------------------------------------------------
        // Draw Data Browser
        //-------------------------------------------------------------------------

        ImGui::SetNextWindowClass( &mainEditorWindowClass );
        ImGui::SetNextWindowDockID( dockLeftID );
        m_pDataBrowser->Draw( context );

        //-------------------------------------------------------------------------
        // Draw open workspaces
        //-------------------------------------------------------------------------

        if ( m_model.HasOpenWorkspaces() )
        {
            ResourceEditorWorkspace* pWorkspaceToClose = nullptr;

            // Draw all workspaces
            for ( auto pWorkspace : m_model.GetOpenWorkspaces() )
            {
                ImGui::SetNextWindowDockID( dockRightID );
                if ( !DrawWorkspaceWindow( context, viewportManager, pWorkspace ) )
                {
                    pWorkspaceToClose = pWorkspace;
                }
            }

            // Did we get a close request?
            if ( pWorkspaceToClose != nullptr )
            {
                m_model.CloseWorkspace( pWorkspaceToClose );

                // If we still have open workspaces, then switch to the new active one
                if ( m_model.HasOpenWorkspaces() )
                {
                    ImGui::SetWindowFocus( m_model.GetActiveWorkspace()->GetWindowName() );
                }
            }
        }

        //-------------------------------------------------------------------------
        // Handle saving
        //-------------------------------------------------------------------------

        if ( m_model.GetActiveWorkspace() != nullptr )
        {
            if ( ( ImGui::GetIO().KeyMods & ImGuiKeyModFlags_Ctrl ) && ImGui::IsKeyDown( 'S' ) )
            {
                m_model.GetActiveWorkspace()->Save();
            }
        }
    }

    void ResourceEditor::FrameEndUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {

    }

    bool ResourceEditor::DrawWorkspaceWindow( UpdateContext const& context, Render::ViewportManager& viewportManager, ResourceEditorWorkspace* pWorkspace )
    {
        KRG_ASSERT( pWorkspace != nullptr );

        bool const isActiveWorkspace = m_model.IsActiveWorkspace( pWorkspace );
        bool const isDirty = pWorkspace->IsDirty();
        bool isTabOpen = true;
        bool isFocused = false;

        //-------------------------------------------------------------------------

        ImGuiWindowClass workspaceWindowClass;
        workspaceWindowClass.ClassId = ImGui::GetID( pWorkspace->GetWorkspaceName() );
        workspaceWindowClass.DockingAllowUnclassed = false;

        ImGuiID const dockspaceID = ImGui::GetID( pWorkspace->GetWorkspaceName() );

        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0.0f, 0.0f ) );
        bool const shouldDrawWindowContents = ImGui::Begin( pWorkspace->GetWindowName(), &isTabOpen, isDirty ? ImGuiWindowFlags_UnsavedDocument : 0 );
        ImGui::PopStyleVar();

        if ( shouldDrawWindowContents )
        {
            isFocused = ImGui::IsWindowFocused();

            // Initial layout of workspace dock
            //-------------------------------------------------------------------------

            if ( !ImGui::DockBuilderGetNode( dockspaceID ) )
            {
                ImGui::DockBuilderAddNode( dockspaceID, ImGuiDockNodeFlags_DockSpace | ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton );
                pWorkspace->InitializeDockingLayout( dockspaceID );
                ImGui::DockBuilderFinish( dockspaceID );
            }

            // Create the actual dock space
            //-------------------------------------------------------------------------

            // Since we share the dock ID across all open workspaces of the same type we dont want to display multiple dockspaces on the frame when we switch tab
            ImGuiDockNodeFlags const dockFlags = isActiveWorkspace ? ImGuiDockNodeFlags_None : ImGuiDockNodeFlags_KeepAliveOnly;
            ImGui::DockSpace( dockspaceID, ImGui::GetContentRegionAvail(), dockFlags, &workspaceWindowClass );

            // Activate the workspace, if it isnt already active
            if ( !isActiveWorkspace )
            {
                m_model.SetActiveWorkspace( pWorkspace );
            }

            // Draw workspace windows
            //-------------------------------------------------------------------------

            pWorkspace->UpdateAndDraw( context, viewportManager, &workspaceWindowClass );

            // Does the active workspace require a viewport?
            if ( isActiveWorkspace && pWorkspace->RequiresViewportWindow() )
            {
                ImGui::SetNextWindowClass( &workspaceWindowClass );
                DrawViewportWindow( context, viewportManager, pWorkspace->GetViewportWindowName() );
            }
        }
        ImGui::End();

        //-------------------------------------------------------------------------

        return isTabOpen;
    }

    //-------------------------------------------------------------------------

    void ResourceEditor::DrawViewportWindow( UpdateContext const& context, Render::ViewportManager& viewportManager, char const* const pEditorViewportName )
    {
        // Create viewport window
        ImGui::SetNextWindowSizeConstraints( ImVec2( 128, 128 ), ImVec2( FLT_MAX, FLT_MAX ) );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0, 0 ) );
        if ( ImGui::Begin( pEditorViewportName, nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar ) )
        {
            ImGuiStyle const& style = ImGui::GetStyle();
            ImVec2 const windowPosition = ImGui::GetWindowPos();
            ImVec2 const windowSize = ImGui::GetWindowSize();
            ImVec2 const viewportSize( Math::Max( ImGui::GetContentRegionAvail().x, 64.0f ), Math::Max( ImGui::GetContentRegionAvail().y, 64.0f ) );

            // Update engine viewport dimensions
            //-------------------------------------------------------------------------

            Math::Rectangle const viewportRect( Float2::Zero, viewportSize );
            viewportManager.ResizePrimaryViewport( viewportRect );
            m_mouseWithinEditorViewport = ImGui::IsWindowHovered();

            // Draw 3D scene
            //-------------------------------------------------------------------------

            ImTextureID vpTextureID = (void*) &viewportManager.GetViewportRenderTargetTextureSRV( 0 );
            ImGui::Image( vpTextureID, viewportSize );

            // Draw orientation guides
            //-------------------------------------------------------------------------

            Float2 const orientationGuidePosition( 4, viewportSize.y - ImGuiX::OrientationGuide::GetRequiredDimensions().m_y );
            ImGui::SetCursorPos( orientationGuidePosition );
            ImGuiX::OrientationGuide::DrawAsChildWindow( *viewportManager.GetPrimaryViewport() );

            // Draw viewport toolbar
            //-------------------------------------------------------------------------

            if ( HasViewportToolbar() )
            {
                ImGui::SetCursorPos( style.WindowPadding );

                ImGui::SetNextWindowBgAlpha( 0.2f );
                ImGui::SetNextWindowSizeConstraints( ImVec2( 0, 0 ), ImVec2( -1, 24 ) );
                ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 2, 2 ) );
                if ( ImGui::BeginChild( "EditorViewportToolbar", ImVec2( -1, -1 ), false, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar ) )
                {
                    DrawViewportToolbar( context, viewportManager );
                }
                ImGui::EndChild();
                ImGui::PopStyleVar();
            }

            // Handle being docked
            if ( auto pDockNode = ImGui::GetWindowDockNode() )
            {
                pDockNode->LocalFlags = 0;

                if ( !m_viewportSettings.m_allowDockingOverViewport )
                {
                    pDockNode->LocalFlags |= ImGuiDockNodeFlags_NoDockingOverMe;
                }

                if ( m_viewportSettings.m_hideViewportTabBar )
                {
                    pDockNode->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
                }
            }
        }
        ImGui::PopStyleVar();
        ImGui::End();
    }

    bool ResourceEditor::HasViewportToolbar() const
    {
        auto pActiveWorkspace = m_model.GetActiveWorkspace();
        if ( pActiveWorkspace != nullptr )
        {
            return pActiveWorkspace->HasViewportToolbar();
        }

        return false;
    }

    void ResourceEditor::DrawViewportToolbar( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
        auto pActiveWorkspace = m_model.GetActiveWorkspace();
        KRG_ASSERT( pActiveWorkspace != nullptr );
        pActiveWorkspace->DrawViewportToolbar( context, viewportManager );
    }
}