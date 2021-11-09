#include "Editor.h"
#include "MapEditor/Workspace_MapEditor.h"
#include "Tools/Core/Editor/EditorWorkspace.h"
#include "Engine/Core/Imgui/OrientationGuide.h"
#include "System/Input/InputSystem.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "Engine/Core/DebugViews/DebugView_Resource.h"

//-------------------------------------------------------------------------

namespace KRG
{
    Editor::~Editor()
    {
        KRG_ASSERT( m_pDataBrowser == nullptr );
    }

    void Editor::Initialize( UpdateContext const& context )
    {
        m_model.Initialize( context );
        m_pDataBrowser = KRG::New<DataBrowser>( m_model );
        m_db.Initialize( m_model.GetTypeRegistry(), m_model.GetRawResourceDirectory() );
    }

    void Editor::Shutdown( UpdateContext const& context )
    {
        m_db.Shutdown();
        KRG::Delete( m_pDataBrowser );
        m_model.Shutdown( context );
    }

    void Editor::FrameStartUpdate( UpdateContext const& context )
    {
        UpdateStage const updateStage = context.GetUpdateStage();
        KRG_ASSERT( updateStage == UpdateStage::FrameStart );

        m_db.Update();

        //-------------------------------------------------------------------------
        // Main Menu
        //-------------------------------------------------------------------------

        if ( ImGui::BeginMainMenuBar() )
        {
            DrawMainMenu( context );

            // Draw Performance Stats
            //-------------------------------------------------------------------------

            ImGuiViewport const* viewport = ImGui::GetMainViewport();
            ImGui::SameLine( viewport->WorkSize.x - 165 );

            float const currentFPS = 1.0f / context.GetDeltaTime();
            float const allocatedMemory = Memory::GetTotalAllocatedMemory() / 1024.0f / 1024.0f;
            ImGui::Text( "FPS: %3.0f | MEM: %.2fMB", currentFPS, allocatedMemory );

            ImGui::EndMainMenuBar();
        }

        //-------------------------------------------------------------------------
        // Create main dock window
        //-------------------------------------------------------------------------

        ImGuiWindowClass editorWindowClass;
        editorWindowClass.ClassId = ImGui::GetID( "EditorWindowClass" );
        editorWindowClass.DockingAllowUnclassed = false;

        ImGuiID const dockspaceID = ImGui::GetID( "EditorDockSpace" );

        ImGuiWindowFlags const windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGuiViewport const* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos( viewport->WorkPos );
        ImGui::SetNextWindowSize( viewport->WorkSize );
        ImGui::SetNextWindowViewport( viewport->ID );

        ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0.0f, 0.0f ) );
        ImGui::Begin( "EditorDockSpaceWindow", nullptr, windowFlags );
        ImGui::PopStyleVar( 3 );
        {
            // Create the actual dock space
            ImGui::PushStyleVar( ImGuiStyleVar_TabRounding, 0 );
            ImGui::DockSpace( dockspaceID, viewport->WorkSize, ImGuiDockNodeFlags_None, &editorWindowClass );
            ImGui::PopStyleVar( 1 );
        }
        ImGui::End();

        //-------------------------------------------------------------------------
        // Draw editor windows
        //-------------------------------------------------------------------------

        if ( m_isResourceBrowserWindowOpen )
        {
            ImGui::SetNextWindowClass( &editorWindowClass );
            m_isResourceBrowserWindowOpen = m_pDataBrowser->Draw( context );
        }

        if ( m_isResourceLogWindowOpen )
        {
            ImGui::SetNextWindowClass( &editorWindowClass );
            Resource::ResourceDebugView::DrawResourceLogWindow( m_model.GetResourceSystem(), &m_isResourceLogWindowOpen );
        }

        if ( m_isResourceReferenceTrackerWindowOpen )
        {
            ImGui::SetNextWindowClass( &editorWindowClass );
            Resource::ResourceDebugView::DrawReferenceTrackerWindow( m_model.GetResourceSystem(), &m_isResourceReferenceTrackerWindowOpen );
        }

        if ( m_isSystemLogWindowOpen )
        {
            m_isSystemLogWindowOpen = m_systemLogView.Draw( context );
        }

        if ( m_isDebugSettingsWindowOpen )
        {
            m_isDebugSettingsWindowOpen = m_debugSettingsView.Draw( context );
        }

        //-------------------------------------------------------------------------
        // Draw open workspaces
        //-------------------------------------------------------------------------

        m_mouseWithinEditorViewport = false;
        EditorWorkspace* pWorkspaceToClose = nullptr;

        // Draw all workspaces
        for ( auto pWorkspace : m_model.GetWorkspaces() )
        {
            ImGui::SetNextWindowClass( &editorWindowClass );
            if ( !DrawWorkspace( context, pWorkspace ) )
            {
                pWorkspaceToClose = pWorkspace;
            }
        }

        // Did we get a close request?
        if ( pWorkspaceToClose != nullptr )
        {
            m_model.DestroyWorkspace( pWorkspaceToClose );
        }

        //-------------------------------------------------------------------------
        // Handle saving
        //-------------------------------------------------------------------------

        /*if ( m_model.GetActiveWorkspace() != nullptr )
        {
            if ( ( ImGui::GetIO().KeyMods & ImGuiKeyModFlags_Ctrl ) && ImGui::IsKeyDown( 'S' ) )
            {
                m_model.GetActiveWorkspace()->Save();
            }
        }*/

        DrawPopups( context );

        //-------------------------------------------------------------------------

        // Disable input when we the mouse is not within the editor viewport
        auto pInputSystem = context.GetSystem<Input::InputSystem>();
        KRG_ASSERT( pInputSystem != nullptr );
        pInputSystem->SetEnabled( m_mouseWithinEditorViewport );
    }

    void Editor::DrawMainMenu( UpdateContext const& context )
    {
        //-------------------------------------------------------------------------
        // Map
        //-------------------------------------------------------------------------

        if ( ImGui::BeginMenu( "Map" ) )
        {
            if ( ImGui::MenuItem( "Create New Map" ) )
            {
                m_model.GetMapEditorWorkspace()->CreateNewMap();
            }

            if ( ImGui::MenuItem( "Load Map" ) )
            {
                m_model.GetMapEditorWorkspace()->SelectAndLoadMap();
            }

            if ( ImGui::MenuItem( "Save Map" ) )
            {
                m_model.GetMapEditorWorkspace()->SaveMap();
            }
            
            if ( ImGui::MenuItem( "Save Map As" ) )
            {
                m_model.GetMapEditorWorkspace()->SaveMapAs();
            }

            ImGui::EndMenu();
        }

        //-------------------------------------------------------------------------
        // Tools
        //-------------------------------------------------------------------------

        if ( ImGui::BeginMenu( "Tools" ) )
        {
            ImGui::Checkbox( "Resource Browser", &m_isResourceBrowserWindowOpen );
            ImGui::Checkbox( "Resource Log", &m_isResourceLogWindowOpen );
            ImGui::Checkbox( "Resource Reference Tracker", &m_isResourceReferenceTrackerWindowOpen );

            ImGui::Separator();

            ImGui::Checkbox( "Debug Settings", &m_isDebugSettingsWindowOpen );
            ImGui::Checkbox( "System Log", &m_isSystemLogWindowOpen );

            ImGui::EndMenu();
        }
    }

    void Editor::DrawPopups( UpdateContext const& context )
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
                    ImGuiX::ScopedFont font( ImGuiX::Font::LargeBold );
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
            else
            {
                ImGui::PopStyleColor( 2 );
            }
        }
    }

    void Editor::BeginHotReload( TVector<ResourceID> const& resourcesToBeReloaded )
    {
        for ( auto pWorkspace : m_model.GetWorkspaces() )
        {
            pWorkspace->BeginHotReload( resourcesToBeReloaded );
        }
    }

    void Editor::EndHotReload()
    {
        for ( auto pWorkspace : m_model.GetWorkspaces() )
        {
            pWorkspace->EndHotReload();
        }
    }

    //-------------------------------------------------------------------------

    bool Editor::DrawWorkspace( UpdateContext const& context, EditorWorkspace* pWorkspace )
    {
        KRG_ASSERT( pWorkspace != nullptr );

        bool const isMapEditorWorkspace = ( pWorkspace == m_model.GetMapEditorWorkspace() );
        bool const isDirty = pWorkspace->IsDirty();

        //-------------------------------------------------------------------------
        // Draw Workspace Window
        //-------------------------------------------------------------------------
        // This is an empty window that just contains the dockspace for the workspace

        bool isTabOpen = true;
        bool* pIsTabOpen = isMapEditorWorkspace ? nullptr : &isTabOpen;

        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0.0f, 0.0f ) );
        ImGuiWindowFlags windowFlags = isDirty ? windowFlags |= ImGuiWindowFlags_UnsavedDocument : 0;
        ImGui::SetNextWindowSizeConstraints( ImVec2( 128, 128 ), ImVec2( FLT_MAX, FLT_MAX ) );
        ImGui::SetNextWindowSize( ImVec2( 640, 480 ), ImGuiCond_FirstUseEver );
        bool const shouldDrawWindowContents = ImGui::Begin( pWorkspace->GetWorkspaceWindowID(), pIsTabOpen, windowFlags );
        ImGui::PopStyleVar();

        bool const isFocused = ImGui::IsWindowFocused( ImGuiFocusedFlags_RootAndChildWindows | ImGuiFocusedFlags_DockHierarchy );

        //-------------------------------------------------------------------------

        ImGuiID const dockspaceID = ImGui::GetID( pWorkspace->GetDockspaceID() );
        ImGuiWindowClass workspaceWindowClass;
        workspaceWindowClass.ClassId = dockspaceID;
        workspaceWindowClass.DockingAllowUnclassed = false;

        if ( !ImGui::DockBuilderGetNode( dockspaceID ) )
        {
            ImGui::DockBuilderAddNode( dockspaceID, ImGuiDockNodeFlags_DockSpace | ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton );
            pWorkspace->InitializeDockingLayout( dockspaceID );
            ImGui::DockBuilderFinish( dockspaceID );
        }

        ImGuiDockNodeFlags const dockFlags = shouldDrawWindowContents ? ImGuiDockNodeFlags_None : ImGuiDockNodeFlags_KeepAliveOnly;
        ImGui::DockSpace( dockspaceID, ImGui::GetContentRegionAvail(), dockFlags, &workspaceWindowClass );

        ImGui::End();

        //-------------------------------------------------------------------------
        // Draw Child Windows
        //-------------------------------------------------------------------------

        auto pWorld = pWorkspace->GetWorld();

        if ( shouldDrawWindowContents )
        {
            // Manage world update
            //-------------------------------------------------------------------------

            if ( isFocused )
            {
                pWorld->ResumeUpdates();
            }
            else
            {
                pWorld->SuspendUpdates();
            }

            // Draw Windows
            //-------------------------------------------------------------------------

            pWorkspace->UpdateAndDrawWindows( context, &workspaceWindowClass );

            // Draw viewport
            //-------------------------------------------------------------------------

            // Does the active workspace require a viewport?
            if ( pWorkspace->HasViewportWindow() )
            {
                auto pViewport = pWorld->GetViewport();

                // Viewport flags
                ImGuiWindowFlags viewportWindowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
                if ( pWorkspace->HasViewportToolbar() )
                {
                    viewportWindowFlags |= ImGuiWindowFlags_MenuBar;
                }

                // Create viewport window
                ImGui::SetNextWindowClass( &workspaceWindowClass );
                ImGui::SetNextWindowSizeConstraints( ImVec2( 128, 128 ), ImVec2( FLT_MAX, FLT_MAX ) );
                ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0, 0 ) );
                if ( ImGui::Begin( pWorkspace->GetViewportWindowID(), nullptr, viewportWindowFlags ) )
                {
                    ImGuiStyle const& style = ImGui::GetStyle();
                    ImVec2 const viewportSize( Math::Max( ImGui::GetContentRegionAvail().x, 64.0f ), Math::Max( ImGui::GetContentRegionAvail().y, 64.0f ) );

                    // Update engine viewport dimensions
                    //-------------------------------------------------------------------------

                    Math::Rectangle const viewportRect( Float2::Zero, viewportSize );
                    pViewport->Resize( viewportRect );
                    m_mouseWithinEditorViewport |= ImGui::IsWindowHovered();

                    // Draw 3D scene
                    //-------------------------------------------------------------------------

                    ImTextureID const vpTextureID = m_model.GetViewportTextureForWorkspace( pWorkspace );
                    ImGui::Image( vpTextureID, viewportSize );

                    // Draw overlay elements
                    //-------------------------------------------------------------------------

                    ImGui::SetCursorPos( style.WindowPadding );
                    pWorkspace->DrawViewportOverlayElements( context, pViewport );
                    ImGuiX::OrientationGuide::Draw( ImGui::GetWindowPos() + viewportSize - ImVec2( ImGuiX::OrientationGuide::GetWidth() + 4, ImGuiX::OrientationGuide::GetWidth() + 4 ), *pViewport );

                    // Draw viewport toolbar
                    //-------------------------------------------------------------------------

                    if( pWorkspace->HasViewportToolbar() )
                    {
                        if ( ImGui::BeginMenuBar() )
                        {
                            pWorkspace->DrawViewportToolbar( context, pViewport );
                            ImGui::EndMenuBar();
                        }
                    }

                    // Handle being docked
                    //-------------------------------------------------------------------------

                    if ( auto pDockNode = ImGui::GetWindowDockNode() )
                    {
                        pDockNode->LocalFlags = 0;
                        pDockNode->LocalFlags |= ImGuiDockNodeFlags_NoDockingOverMe;
                        pDockNode->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
                    }
                }
                ImGui::PopStyleVar();
                ImGui::End();
            }
        }
        else
        {
            pWorld->SuspendUpdates();
        }

        return isTabOpen;
    }
}