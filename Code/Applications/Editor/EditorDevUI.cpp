#include "EditorDevUI.h"
#include "MapEditor/Workspace_MapEditor.h"
#include "MapEditor/Workspace_GamePreviewer.h"
#include "Tools/Core/Workspaces/EditorWorkspace.h"
#include "Engine/Render/Systems/WorldSystem_WorldRenderer.h"
#include "Engine/Physics/Debug/DebugView_Physics.h"
#include "Engine/Core/DevUI/OrientationGuide.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "Engine/Core/DebugViews/DebugView_Resource.h"
#include "Engine/Core/Entity/EntityWorldManager.h"
#include "Engine/Render/Debug/DebugView_Render.h"

//-------------------------------------------------------------------------

namespace KRG
{
    EditorDevUI::~EditorDevUI()
    {
        KRG_ASSERT( m_pResourceBrowser == nullptr );
    }

    void EditorDevUI::Initialize( UpdateContext const& context )
    {
        m_model.Initialize( context );
        m_pResourceBrowser = KRG::New<ResourceBrowser>( m_model );
        m_db.Initialize( m_model.GetTypeRegistry(), m_model.GetSourceResourceDirectory() );
    }

    void EditorDevUI::Shutdown( UpdateContext const& context )
    {
        m_db.Shutdown();
        KRG::Delete( m_pResourceBrowser );
        m_model.Shutdown( context );
    }

    //-------------------------------------------------------------------------

    void EditorDevUI::FrameStartUpdate( UpdateContext const& context )
    {
        UpdateStage const updateStage = context.GetUpdateStage();
        KRG_ASSERT( updateStage == UpdateStage::FrameStart );

        // Update the resource database and potentially refresh the browser
        if ( m_db.Update() )
        {
            m_pResourceBrowser->RebuildBrowserTree();
        }

        // Update the model - this process all workspace lifetime requests
        m_model.Update( context );

        //-------------------------------------------------------------------------
        // Main Menu
        //-------------------------------------------------------------------------

        if ( ImGui::BeginMainMenuBar() )
        {
            DrawMainMenu( context );
            ImGui::EndMainMenuBar();
        }

        //-------------------------------------------------------------------------
        // Create main dock window
        //-------------------------------------------------------------------------

        m_editorWindowClass.ClassId = ImGui::GetID( "EditorWindowClass" );
        m_editorWindowClass.DockingAllowUnclassed = false;

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
            ImGui::DockSpace( dockspaceID, viewport->WorkSize, ImGuiDockNodeFlags_None, &m_editorWindowClass );
            ImGui::PopStyleVar( 1 );
        }
        ImGui::End();

        //-------------------------------------------------------------------------
        // Draw editor windows
        //-------------------------------------------------------------------------

        if ( m_isResourceBrowserWindowOpen )
        {
            ImGui::SetNextWindowClass( &m_editorWindowClass );
            m_isResourceBrowserWindowOpen = m_pResourceBrowser->Draw( context );
        }

        if ( m_isResourceLogWindowOpen )
        {
            ImGui::SetNextWindowClass( &m_editorWindowClass );
            Resource::ResourceDebugView::DrawResourceLogWindow( m_model.GetResourceSystem(), &m_isResourceLogWindowOpen );
        }

        if ( m_isResourceReferenceTrackerWindowOpen )
        {
            ImGui::SetNextWindowClass( &m_editorWindowClass );
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

        if ( m_isPhysicsMaterialDatabaseWindowOpen )
        {
            m_isPhysicsMaterialDatabaseWindowOpen = Physics::PhysicsMaterialDatabaseDebugView::Draw( context );
        }

        if ( m_isImguiDemoWindowOpen )
        {
            ImGui::ShowDemoWindow( &m_isImguiDemoWindowOpen );
        }

        //-------------------------------------------------------------------------
        // Draw open workspaces
        //-------------------------------------------------------------------------

        // Reset mouse state, this is updated via the workspaces
        EditorWorkspace* pWorkspaceToClose = nullptr;

        // Draw all workspaces
        for ( auto pWorkspace : m_model.GetWorkspaces() )
        {
            if ( m_model.IsGamePreviewWorkspace( pWorkspace ) )
            {
                continue;
            }

            ImGui::SetNextWindowClass( &m_editorWindowClass );
            if ( !DrawWorkspaceWindow( context, pWorkspace ) )
            {
                pWorkspaceToClose = pWorkspace;
            }
        }

        // Did we get a close request?
        if ( pWorkspaceToClose != nullptr )
        {
            // We need to defer this to the start of the update since we may have references resources that we might unload (i.e. textures)
            m_model.QueueDestroyWorkspace( pWorkspaceToClose );
        }

        //-------------------------------------------------------------------------
        // Pop-ups
        //-------------------------------------------------------------------------

        DrawPopups( context );
    }

    void EditorDevUI::FrameEndUpdate( UpdateContext const& context )
    {
        // Game previewer needs to be drawn at the end of the frames since then all the game simulation data will be correct and all the debug tools will be accurate
        if ( m_model.IsGamePreviewRunning() )
        {
            GamePreviewer* pGamePreviewer = m_model.GetGamePreviewWorkspace();
            if ( !DrawWorkspaceWindow( context, pGamePreviewer ) )
            {
                m_model.QueueDestroyWorkspace( pGamePreviewer );
            }
        }
    }

    //-------------------------------------------------------------------------

    void EditorDevUI::BeginHotReload( TVector<ResourceID> const& resourcesToBeReloaded )
    {
        for ( auto pWorkspace : m_model.GetWorkspaces() )
        {
            pWorkspace->BeginHotReload( resourcesToBeReloaded );
        }
    }

    void EditorDevUI::EndHotReload()
    {
        for ( auto pWorkspace : m_model.GetWorkspaces() )
        {
            pWorkspace->EndHotReload();
        }
    }

    //-------------------------------------------------------------------------

    void EditorDevUI::DrawMainMenu( UpdateContext const& context )
    {
        ImVec2 const menuDimensions = ImGui::GetContentRegionMax();

        //-------------------------------------------------------------------------
        // Engine
        //-------------------------------------------------------------------------

        if ( ImGui::BeginMenu( "Resource" ) )
        {
            ImGui::MenuItem( "Resource Browser", nullptr, &m_isResourceBrowserWindowOpen );
            ImGui::MenuItem( "Resource Log", nullptr, &m_isResourceLogWindowOpen );
            ImGui::MenuItem( "Resource Reference Tracker", nullptr, &m_isResourceReferenceTrackerWindowOpen );
            ImGui::EndMenu();
        }

        if ( ImGui::BeginMenu( "Physics" ) )
        {
            ImGui::MenuItem( "Physics Material DB", nullptr, &m_isPhysicsMaterialDatabaseWindowOpen );
            ImGui::EndMenu();
        }

        if ( ImGui::BeginMenu( "System" ) )
        {
            ImGui::MenuItem( "Debug Settings", nullptr, &m_isDebugSettingsWindowOpen );
            ImGui::MenuItem( "System Log", nullptr, &m_isSystemLogWindowOpen );

            ImGui::Separator();

            ImGui::MenuItem( "Imgui Demo Window", nullptr, &m_isImguiDemoWindowOpen );

            ImGui::EndMenu();
        }

        //-------------------------------------------------------------------------
        // Game Preview
        //-------------------------------------------------------------------------

        ImGui::BeginDisabled( !m_model.IsGamePreviewAllowed() );
        if ( m_model.IsGamePreviewRunning() )
        {
            char const * const stopPreviewStr = KRG_ICON_STOP" Stop Game Preview";
            ImGui::SameLine( menuDimensions.x / 2 - ImGui::CalcTextSize( stopPreviewStr ).x / 2 );
            if ( ImGui::MenuItem( stopPreviewStr ) )
            {
                m_model.StopGamePreview( context );
            }
        }
        else
        {
            char const * const startPreviewStr = KRG_ICON_PLAY" Preview Game";
            ImGui::SameLine( menuDimensions.x / 2 - ImGui::CalcTextSize( startPreviewStr ).x / 2 );
            if ( ImGui::MenuItem( startPreviewStr ) )
            {
                m_model.StartGamePreview( context );
            }
        }
        ImGui::EndDisabled();

        //-------------------------------------------------------------------------
        // Draw Performance Stats
        //-------------------------------------------------------------------------

        float const currentFPS = 1.0f / context.GetDeltaTime();
        float const allocatedMemory = Memory::GetTotalAllocatedMemory() / 1024.0f / 1024.0f;

        InlineString<100> const perfStats( InlineString<100>::CtorSprintf(), "FPS: %3.0f | MEM: %.2fMB", currentFPS, allocatedMemory );
        ImGui::SameLine( menuDimensions.x - 8 - ImGui::CalcTextSize( perfStats.c_str() ).x );
        ImGui::Text( perfStats.c_str() );
    }

    void EditorDevUI::DrawPopups( UpdateContext const& context )
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

    bool EditorDevUI::DrawWorkspaceWindow( UpdateContext const& context, EditorWorkspace* pWorkspace )
    {
        KRG_ASSERT( pWorkspace != nullptr );

        //-------------------------------------------------------------------------
        // Create Workspace Window
        //-------------------------------------------------------------------------
        // This is an empty window that just contains the dockspace for the workspace

        bool isTabOpen = true;
        bool* pIsTabOpen = m_model.IsMapEditorWorkspace( pWorkspace ) ? nullptr : &isTabOpen;

        ImGuiWindowFlags windowFlags = 0;

        if ( pWorkspace->HasWorkspaceToolbar() )
        {
            windowFlags |= ImGuiWindowFlags_MenuBar;
        }

        if ( pWorkspace->IsDirty() )
        {
            windowFlags |= ImGuiWindowFlags_UnsavedDocument;
        }

        ImGui::SetNextWindowSizeConstraints( ImVec2( 128, 128 ), ImVec2( FLT_MAX, FLT_MAX ) );
        ImGui::SetNextWindowSize( ImVec2( 640, 480 ), ImGuiCond_FirstUseEver );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0, 0 ) );
        bool const shouldDrawWindowContents = ImGui::Begin( pWorkspace->GetWorkspaceWindowID(), pIsTabOpen, windowFlags );
        bool const isFocused = ImGui::IsWindowFocused( ImGuiFocusedFlags_ChildWindows | ImGuiFocusedFlags_DockHierarchy );
        ImGui::PopStyleVar();

        // Draw Workspace Menu
        //-------------------------------------------------------------------------

        if ( pWorkspace->HasWorkspaceToolbar() )
        {
            if ( ImGui::BeginMenuBar() )
            {
                if ( pWorkspace->ShouldDrawFileMenu() )
                {
                    ImGui::BeginDisabled( !pWorkspace->IsDirty() );
                    if ( ImGui::MenuItem( KRG_ICON_FLOPPY_O" Save" ) )
                    {
                        pWorkspace->Save();
                    }
                    ImGui::EndDisabled();

                    ImGui::BeginDisabled( !pWorkspace->CanUndo() );
                    if ( ImGui::MenuItem( KRG_ICON_UNDO" Undo" ) )
                    {
                        pWorkspace->Undo();
                    }
                    ImGui::EndDisabled();

                    ImGui::BeginDisabled( !pWorkspace->CanRedo() );
                    if ( ImGui::MenuItem( KRG_ICON_REPEAT" Redo" ) )
                    {
                        pWorkspace->Redo();
                    }
                    ImGui::EndDisabled();
                }

                //-------------------------------------------------------------------------

                pWorkspace->DrawWorkspaceToolbar( context );
                ImGui::EndMenuBar();
            }
        }

        // Create dockspace
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
        // Draw workspace contents
        //-------------------------------------------------------------------------

        bool enableInputForWorld = false;
        auto pWorldManager = context.GetSystem<EntityWorldManager>();
        auto pWorld = pWorkspace->GetWorld();

        if ( shouldDrawWindowContents )
        {
            if ( !m_model.IsMapEditorWorkspace( pWorkspace ) || !m_model.IsGamePreviewRunning() )
            {
                pWorld->ResumeUpdates();
            }

            pWorkspace->UpdateAndDrawWindows( context, &workspaceWindowClass );

            if ( pWorkspace->HasViewportWindow() )
            {
                ImTextureID const pTextureID = m_model.GetViewportTextureForWorkspace( pWorkspace );
                enableInputForWorld = pWorkspace->DrawViewport( context, pTextureID, &workspaceWindowClass );
            }
        }
        else // If the workspace window is hidden suspend world updates
        {
            pWorld->SuspendUpdates();
        }

        pWorldManager->SetPlayerEnabled( pWorld, enableInputForWorld );

        // Handle input
        //-------------------------------------------------------------------------

        if ( isFocused )
        {
            auto& IO = ImGui::GetIO();
            if ( IO.KeyCtrl && ImGui::IsKeyPressed( ImGui::GetKeyIndex( ImGuiKey_Z ) ) )
            {
                if ( pWorkspace->CanUndo() )
                {
                    pWorkspace->Undo();
                }
            }

            if ( IO.KeyCtrl && ImGui::IsKeyPressed( ImGui::GetKeyIndex( ImGuiKey_Y ) ) )
            {
                if ( pWorkspace->CanRedo() )
                {
                    pWorkspace->Redo();
                }
            }
        }

        return isTabOpen;
    }
}