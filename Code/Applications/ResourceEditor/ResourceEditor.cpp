#include "ResourceEditor.h"
#include "System/Render/RenderViewportManager.h"
#include "Tools/Core/Editors/ResourceEditorWorkspace.h"

//-------------------------------------------------------------------------

namespace KRG
{
    ResourceEditor::~ResourceEditor()
    {
        KRG_ASSERT( m_pDataBrowser == nullptr );
    }

    void ResourceEditor::Initialize( UpdateContext const& context, SettingsRegistry const& settingsRegistry )
    {
        TStandaloneEditor<ResourceEditorModel>::Initialize( context, settingsRegistry );
     
        m_pDataBrowser = KRG::New<DataBrowser>( GetModel() );
    }

    void ResourceEditor::Shutdown( UpdateContext const& context )
    {
        KRG::Delete( m_pDataBrowser );

        TStandaloneEditor<ResourceEditorModel>::Shutdown( context );
    }

    void ResourceEditor::FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
        auto& model = GetModel();

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

        if ( model.HasOpenWorkspaces() )
        {
            ResourceEditorWorkspace* pWorkspaceToClose = nullptr;

            // Draw all workspaces
            for ( auto pWorkspace : model.GetOpenWorkspaces() )
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
                model.CloseWorkspace( pWorkspaceToClose );

                // If we still have open workspaces, then switch to the new active one
                if ( model.HasOpenWorkspaces() )
                {
                    ImGui::SetWindowFocus( model.GetActiveWorkspace()->GetWindowName() );
                }
            }
        }

        //-------------------------------------------------------------------------
        // Handle saving
        //-------------------------------------------------------------------------

        if ( model.GetActiveWorkspace() != nullptr )
        {
            if ( ( ImGui::GetIO().KeyMods & ImGuiKeyModFlags_Ctrl ) && ImGui::IsKeyDown( 'S' ) )
            {
                model.GetActiveWorkspace()->Save();
            }
        }
    }

    bool ResourceEditor::DrawWorkspaceWindow( UpdateContext const& context, Render::ViewportManager& viewportManager, ResourceEditorWorkspace* pWorkspace )
    {
        KRG_ASSERT( pWorkspace != nullptr );

        auto& model = GetModel();

        bool const isActiveWorkspace = model.IsActiveWorkspace( pWorkspace );
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
                model.SetActiveWorkspace( pWorkspace );
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

    bool ResourceEditor::HasViewportToolbar() const
    {
        auto& model = GetModel();
        auto pActiveWorkspace = model.GetActiveWorkspace();
        if ( pActiveWorkspace != nullptr )
        {
            return pActiveWorkspace->HasViewportToolbar();
        }

        return false;
    }

    void ResourceEditor::DrawViewportToolbar( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
        auto& model = GetModel();
        auto pActiveWorkspace = model.GetActiveWorkspace();
        KRG_ASSERT( pActiveWorkspace != nullptr );
        pActiveWorkspace->DrawViewportToolbar( context, viewportManager );
    }
}