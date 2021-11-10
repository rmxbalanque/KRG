#include "Workspace_Texture.h"
#include "Tools/Core/Widgets/InterfaceHelpers.h"
#include "Engine/Core/Entity/EntityWorld.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    KRG_RESOURCE_WORKSPACE_FACTORY( TextureWorkspaceFactory, Texture, TextureWorkspace );

    //-------------------------------------------------------------------------

    void TextureWorkspace::Initialize()
    {
        TResourceWorkspace<Texture>::Initialize();
        m_previewWindowName.sprintf( "Preview##%u", GetID() );
        m_infoWindowName.sprintf( "Info##%u", GetID() );
    }

    void TextureWorkspace::Shutdown()
    {
        TResourceWorkspace<Texture>::Shutdown();
    }

    void TextureWorkspace::InitializeDockingLayout( ImGuiID dockspaceID ) const
    {
        ImGuiID topDockID = 0;
        ImGuiID bottomDockID = ImGui::DockBuilderSplitNode( dockspaceID, ImGuiDir_Down, 0.5f, nullptr, &topDockID );

        // Dock viewport
        ImGuiDockNode* pTopNode = ImGui::DockBuilderGetNode( topDockID );
        pTopNode->LocalFlags |= ImGuiDockNodeFlags_NoDockingSplitMe | ImGuiDockNodeFlags_NoDockingOverMe;
        ImGui::DockBuilderDockWindow( m_previewWindowName.c_str(), topDockID );

        // Dock windows
        ImGui::DockBuilderDockWindow( m_infoWindowName.c_str(), bottomDockID );
    }

    void TextureWorkspace::UpdateAndDrawWindows( UpdateContext const& context, ImGuiWindowClass* pWindowClass )
    {
        ImGui::SetNextWindowClass( pWindowClass );
        if ( ImGui::Begin( m_previewWindowName.c_str() ) )
        {
            if ( IsLoaded() )
            {
                ImTextureID const textureID = (void*) &m_pResource->GetShaderResourceView();
                ImGui::Image( textureID, Float2( m_pResource->GetDimensions() ) );
            }
        }
        ImGui::End();

        //-------------------------------------------------------------------------

        ImGui::SetNextWindowClass( pWindowClass );
        if ( ImGui::Begin( m_infoWindowName.c_str() ) )
        {
            if ( IsWaitingForResource() )
            {
                ImGui::Text( "Loading:" );
                ImGui::SameLine();
                ImGuiX::DrawSpinner( "Loading" );
            }
            else if ( HasLoadingFailed() )
            {
                ImGui::Text( "Loading Failed: %s", m_pResource.GetResourceID().c_str() );
            }
            else
            {
                ImGui::Text( "Dimensions: %.0f x %.0f", m_pResource->GetDimensions().m_x, m_pResource->GetDimensions().m_y );
            }
        }
        ImGui::End();
    }
}