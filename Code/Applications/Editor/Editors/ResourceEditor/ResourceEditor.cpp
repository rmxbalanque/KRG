#include "ResourceEditor.h"
#include "Workspaces\ResourceWorkspace.h"
#include "System\Render\RenderViewportManager.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    class ResourceEditorFactory : public EditorFactory
    {
    public:

        virtual const char* GetID() const override { return "ResourceEditor" ; }
        virtual Editor* CreateEditor() const override { return KRG::New<ResourceEditor>(); }
    };

    static const ResourceEditorFactory g_ResourceEditorFactory;
}

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    ResourceEditor::~ResourceEditor()
    {
        KRG_ASSERT( m_pDataBrowser == nullptr );
    }

    void ResourceEditor::Initialize( UpdateContext const& context, SettingsRegistry const& settingsRegistry )
    {
        TEditor<ResourceEditorModel>::Initialize( context, settingsRegistry );
     
        m_pDataBrowser = KRG::New<DataBrowser>( GetModel() );
    }

    void ResourceEditor::Shutdown( UpdateContext const& context )
    {
        KRG::Delete( m_pDataBrowser );

        TEditor<ResourceEditorModel>::Shutdown( context );
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

        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking;
        windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

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
            if ( !ImGui::DockBuilderGetNode( dockspaceID ) )
            {
                ImGui::DockBuilderRemoveNode( dockspaceID );
                ImGui::DockBuilderAddNode( dockspaceID, ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton );

                dockRightID = dockspaceID;
                dockLeftID = ImGui::DockBuilderSplitNode( dockRightID, ImGuiDir_Left, 0.2f, nullptr, &dockRightID );

                // Disable tab bars for main windows
                ImGuiDockNode* pLeftNode = ImGui::DockBuilderGetNode( dockLeftID );
                pLeftNode->LocalFlags |= ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoCloseButton | ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton | ImGuiDockNodeFlags_NoDockingOverMe;

                ImGuiDockNode* pRightNode = ImGui::DockBuilderGetNode( dockRightID );
                pRightNode->LocalFlags |= ImGuiDockNodeFlags_CentralNode | ImGuiDockNodeFlags_NoDockingSplitMe | ImGuiDockNodeFlags_NoDockingOverMe;

                // Dock data browser
                ImGui::DockBuilderDockWindow( "Data Browser", dockLeftID );

                ImGui::DockBuilderFinish( dockRightID );
            }
            else // look up the correct IDs
            {
                if ( dockLeftID == 0 && dockRightID == 0 )
                {
                    auto pDockRootNode = ImGui::DockBuilderGetNode( dockspaceID );
                    dockLeftID = pDockRootNode->ChildNodes[0]->ID;
                    dockRightID = pDockRootNode->ChildNodes[1]->ID;
                }
            }

            //-------------------------------------------------------------------------

            //// Dock workspaces
            //for ( auto pWorkspace : model.GetOpenWorkspaces() )
            //{
            //    ImGui::DockBuilderDockWindow( pWorkspace->GetWindowName(), dockRightID );
            //}

            // Create the actual dock space
            ImGui::DockSpace( dockspaceID, viewport->WorkSize, ImGuiDockNodeFlags_None, &mainEditorWindowClass );
        }
        ImGui::End();

        //-------------------------------------------------------------------------
        // Draw Data Browser
        //-------------------------------------------------------------------------

        ImGui::SetNextWindowClass( &mainEditorWindowClass );
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
    }

    bool ResourceEditor::DrawWorkspaceWindow( UpdateContext const& context, Render::ViewportManager& viewportManager, ResourceEditorWorkspace* pWorkspace )
    {
        KRG_ASSERT( pWorkspace != nullptr );

        auto& model = GetModel();
        bool const isActiveWorkspace = model.IsActiveWorkspace( pWorkspace );

        //-------------------------------------------------------------------------
        ImGuiWindowClass workspaceWindowClass;
        workspaceWindowClass.ClassId = ImGui::GetID( pWorkspace->GetWorkspaceName() );
        workspaceWindowClass.DockingAllowUnclassed = false;

        //-------------------------------------------------------------------------

        bool isOpen = true;
        bool isFocused = false;
        ImGuiID const dockspaceID = ImGui::GetID( pWorkspace->GetWorkspaceName() );

        ImGuiWindowFlags windowFlags = 0;
        windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        bool const shouldDrawWindowContents = ImGui::Begin( pWorkspace->GetWindowName(), &isOpen );
        if ( shouldDrawWindowContents )
        {
            isFocused = ImGui::IsWindowFocused();

            //if ( !ImGui::DockBuilderGetNode( dockspaceID ) )
            //{
            //    ImGui::DockBuilderRemoveNode( dockspaceID );
            //    ImGui::DockBuilderAddNode( dockspaceID, ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton );

            //    dockRightID = dockspaceID;
            //    dockLeftID = ImGui::DockBuilderSplitNode( dockRightID, ImGuiDir_Left, 0.2f, nullptr, &dockRightID );

            //    // Disable tab bars for main windows
            //    ImGuiDockNode* pLeftNode = ImGui::DockBuilderGetNode( dockLeftID );
            //    pLeftNode->LocalFlags |= ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoCloseButton | ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton | ImGuiDockNodeFlags_NoDockingOverMe;

            //    ImGuiDockNode* pRightNode = ImGui::DockBuilderGetNode( dockRightID );
            //    pRightNode->LocalFlags |= ImGuiDockNodeFlags_CentralNode | ImGuiDockNodeFlags_NoDockingSplitMe | ImGuiDockNodeFlags_NoDockingOverMe;

            //    // Dock data browser
            //    ImGui::DockBuilderDockWindow( "Data Browser", dockLeftID );

            //    ImGui::DockBuilderFinish( dockRightID );
            //}
            //else // look up the correct IDs
            //{
            //    if ( dockLeftID == 0 && dockRightID == 0 )
            //    {
            //        auto pDockRootNode = ImGui::DockBuilderGetNode( dockspaceID );
            //        dockLeftID = pDockRootNode->ChildNodes[0]->ID;
            //        dockRightID = pDockRootNode->ChildNodes[1]->ID;
            //    }
            //}

            //// Dock workspaces
            //for ( auto pWorkspace : model.GetOpenWorkspaces() )
            //{
            //    
            //}

            //ImGui::DockBuilderDockWindow( GetViewportWindowName(), dockspaceID );

            // Create the actual dock space
            //-------------------------------------------------------------------------

            // Since we share the dock ID across all open workspaces of the same type we dont want to display multiple dockspaces on the frame when we switch tab
            ImGuiDockNodeFlags const dockFlags = isActiveWorkspace ? ImGuiDockNodeFlags_None : ImGuiDockNodeFlags_KeepAliveOnly;
            ImGui::DockSpace( dockspaceID, ImGui::GetContentRegionAvail(), dockFlags, &workspaceWindowClass );
        }
        ImGui::End();

        // Draw workspace and viewport windows
        //-------------------------------------------------------------------------

        if ( shouldDrawWindowContents )
        {
            // Draw workspace windows
            pWorkspace->Draw( context, viewportManager, &workspaceWindowClass );

            // Does the active workspace require a viewport?
            if ( isActiveWorkspace && pWorkspace->RequiresViewportWindow() )
            {
                InlineString<100> viewportName;
                viewportName.sprintf( "Viewport##%s", pWorkspace->GetWorkspaceName() );

                ImGui::SetNextWindowClass( &workspaceWindowClass );
                DrawViewportWindow( context, viewportManager, viewportName.c_str() );
            }
        }

        // Handle tab switches
        //-------------------------------------------------------------------------

        if ( isFocused && !isActiveWorkspace )
        {
            model.SetActiveWorkspace( pWorkspace );
        }

        //-------------------------------------------------------------------------

        return isOpen;
    }
}