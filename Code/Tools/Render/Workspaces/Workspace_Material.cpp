#include "Workspace_Material.h"
#include "Tools/Core/Widgets/InterfaceHelpers.h"
#include "Engine/Render/Components/Component_StaticMesh.h"
#include "Engine/Core/Entity/EntityWorld.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    KRG_RESOURCE_WORKSPACE_FACTORY( MaterialWorkspaceFactory, Material, MaterialWorkspace );

    //-------------------------------------------------------------------------

    void MaterialWorkspace::Initialize( UpdateContext const& context )
    {
        KRG_ASSERT( m_pPreviewComponent == nullptr );

        TResourceWorkspace<Material>::Initialize( context );

        m_materialDetailsWindowName.sprintf( "Material Properties##%u", GetID() );

        //-------------------------------------------------------------------------

        m_pPreviewComponent = KRG::New<StaticMeshComponent>( StringID( "Static Mesh Component" ) );
        m_pPreviewComponent->SetMesh( "data://Editor/MaterialBall/MaterialBall.msh" );
        m_pPreviewComponent->SetWorldTransform( Transform( Quaternion::Identity, Vector( 0, 0, 1 ) ) );
        m_pPreviewComponent->SetMaterialOverride( 0, m_pResource.GetResourceID() );

        // We dont own the entity as soon as we add it to the map
        auto pPreviewEntity = KRG::New<Entity>( StringID( "Preview" ) );
        pPreviewEntity->AddComponent( m_pPreviewComponent );
        m_pWorld->GetPersistentMap()->AddEntity( pPreviewEntity );
    }

    void MaterialWorkspace::Shutdown( UpdateContext const& context )
    {
        m_pPreviewComponent = nullptr;
        TResourceWorkspace<Material>::Shutdown( context );
    }

    void MaterialWorkspace::InitializeDockingLayout( ImGuiID dockspaceID ) const
    {
        ImGuiID topDockID = 0;
        ImGuiID bottomDockID = ImGui::DockBuilderSplitNode( dockspaceID, ImGuiDir_Down, 0.5f, nullptr, &topDockID );

        // Dock viewport
        ImGuiDockNode* pTopNode = ImGui::DockBuilderGetNode( topDockID );
        pTopNode->LocalFlags |= ImGuiDockNodeFlags_NoDockingSplitMe | ImGuiDockNodeFlags_NoDockingOverMe;
        ImGui::DockBuilderDockWindow( GetViewportWindowID(), topDockID );

        // Dock windows
        ImGui::DockBuilderDockWindow( m_descriptorWindowName.c_str(), bottomDockID );
        ImGui::DockBuilderDockWindow( m_materialDetailsWindowName.c_str(), bottomDockID );
    }

    void MaterialWorkspace::UpdateAndDrawWindows( UpdateContext const& context, ImGuiWindowClass* pWindowClass )
    {
        ImGui::SetNextWindowClass( pWindowClass );
        if ( ImGui::Begin( m_materialDetailsWindowName.c_str() ) )
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
                return;
            }
            else
            {
                if ( m_pDescriptor != nullptr )
                {
                    m_descriptorPropertyGrid.DrawGrid();
                }
            }
        }
        ImGui::End();
    }
}