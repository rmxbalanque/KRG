#include "ResourceEditor.h"
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
        auto& viewport = viewportManager.GetActiveViewports()[0];

        //ImTextureID vp = (void*) const_cast<Render::Texture const*>( viewport.GetCustomRenderTarget().GetRenderTargetTexture() );

        if ( ImGui::Begin( "Temp" ) )
        {
            auto size = ImGui::GetWindowSize();
            auto region = ImGui::GetContentRegionAvail();
            Int2 a = Int2( 0, 0 );
            Int2 b = Int2( region );
            viewport.Resize( a, b );

           // ImGui::Image( vp, size );

            //m_mouseWithinEditorViewport = ImGui::IsWindowHovered();
        }
        ImGui::End();



        //// Create main dock
        ////-------------------------------------------------------------------------

        //ImGuiID dockspaceID = ImGui::GetID( "EditorDockSpace" );

        //ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking;
        //windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        //windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        //ImGuiViewport const* viewport = ImGui::GetMainViewport();
        //ImGui::SetNextWindowPos( viewport->WorkPos );
        //ImGui::SetNextWindowSize( viewport->WorkSize );
        //ImGui::SetNextWindowViewport( viewport->ID );

        //ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
        //ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );
        //ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0.0f, 0.0f ) );

        //ImGui::SetNextWindowBgAlpha( 0.0f );
        //ImGui::Begin( "EditorDockSpaceWindow", nullptr, windowFlags );
        //{
        //    if ( !ImGui::DockBuilderGetNode( dockspaceID ) ) 
        //    {
        //        ImGui::DockBuilderRemoveNode( dockspaceID );
        //        ImGui::DockBuilderAddNode( dockspaceID, ImGuiDockNodeFlags_None );

        //        ImGuiID dock_main_id = dockspaceID;
        //        ImGuiID dock_left_id = ImGui::DockBuilderSplitNode( dock_main_id, ImGuiDir_Left, 0.2f, nullptr, &dock_main_id );

        //        ImGui::DockBuilderDockWindow( "Data Browser", dock_left_id );

        //        if ( ImGuiDockNode* pCentralNode = ImGui::DockBuilderGetCentralNode( dockspaceID ) )
        //        {
        //            pCentralNode->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
        //            ImGui::DockBuilderDockWindow( "EditorViewport", pCentralNode->ID );
        //        }

        //        // Disable tab bar for custom toolbar
        //        ImGuiDockNode* node = ImGui::DockBuilderGetNode( dock_left_id );
        //        node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoCloseButton | ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton | ImGuiDockNodeFlags_NoDockingOverMe;

        //        ImGui::DockBuilderFinish( dock_main_id );
        //    }

        //    // Always ensure that the viewport is docked into the central node
        //    /*if ( ImGuiDockNode* pCentralNode = ImGui::DockBuilderGetCentralNode( dockspaceID ) )
        //    {
        //        pCentralNode->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
        //        ImGui::DockBuilderDockWindow( "EditorViewport", pCentralNode->ID );
        //    }*/

        //    //// Create the actual dock space
        //    //ImGuiDockNodeFlags const dockSpaceFlags = ImGuiDockNodeFlags_NoDockingInCentralNode;
        //    //ImGui::DockSpace( dockspaceID, viewport->WorkSize, dockSpaceFlags );
        //}
        //ImGui::PopStyleVar( 3 );
        //ImGui::End();

        //// Draw windows
        ////-------------------------------------------------------------------------

        //m_pDataBrowser->FrameStartUpdate( context );

        //DrawViewportWindow( context, viewportManager );
    }
}