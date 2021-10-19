#include "ResourceEditor_StaticMesh.h"
#include "System/Render/Imgui/Widgets/InterfaceHelpers.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "Engine/Render/Components/StaticMeshComponent.h"
#include "Engine/Render/Components/SkeletalMeshComponent.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    KRG_RESOURCE_WORKSPACE_FACTORY( StaticMeshWorkspaceFactory, StaticMesh, StaticMeshResourceEditor );

    //-------------------------------------------------------------------------

    char const* const StaticMeshResourceEditor::s_infoWindowName = "Info Window##StaticMesh";

    //-------------------------------------------------------------------------

    void StaticMeshResourceEditor::Activate( EntityWorld* pPreviewWorld )
    {
        KRG_ASSERT( m_pPreviewEntity == nullptr );

        // We dont own the entity as soon as we add it to the map
        auto pPersistentMap = pPreviewWorld->GetPersistentMap();

        m_pPreviewEntity = KRG::New<Entity>( StringID( "Preview" ) );
        pPersistentMap->AddEntity( m_pPreviewEntity );

        auto pStaticMeshComponent = KRG::New<StaticMeshComponent>( StringID( "Static Mesh Component" ) );
        pStaticMeshComponent->SetMesh( m_pResource.GetResourceID() );
        m_pPreviewEntity->AddComponent( pStaticMeshComponent );
    }

    void StaticMeshResourceEditor::Deactivate( EntityWorld* pPreviewWorld )
    {
        KRG_ASSERT( m_pPreviewEntity != nullptr );

        auto pPersistentMap = pPreviewWorld->GetPersistentMap();
        pPersistentMap->DestroyEntity( m_pPreviewEntity->GetID() );
        m_pPreviewEntity = nullptr;
    }

    void StaticMeshResourceEditor::InitializeDockingLayout( ImGuiID dockspaceID ) const
    {
        ImGuiID topDockID = 0;
        ImGuiID bottomDockID = ImGui::DockBuilderSplitNode( dockspaceID, ImGuiDir_Down, 0.5f, nullptr, &topDockID );

        // Dock viewport
        ImGuiDockNode* pTopNode = ImGui::DockBuilderGetNode( topDockID );
        pTopNode->LocalFlags |= ImGuiDockNodeFlags_NoDockingSplitMe | ImGuiDockNodeFlags_NoDockingOverMe;
        ImGui::DockBuilderDockWindow( GetViewportWindowName(), topDockID );

        // Dock windows
        ImGui::DockBuilderDockWindow( s_infoWindowName, bottomDockID );
    }

    void StaticMeshResourceEditor::UpdateAndDraw( UpdateContext const& context, Render::ViewportManager& viewportManager, ImGuiWindowClass* pWindowClass )
    {
        auto DrawWindowContents = [this] ()
        {
            if ( IsWaitingForResource() )
            {
                ImGui::Text( "Loading:" );
                ImGui::SameLine();
                ImGuiX::DrawSpinner( "Loading" );
                return;
            }

            if ( HasLoadingFailed() )
            {
                ImGui::Text( "Loading Failed: %s", m_pResource.GetResourceID().c_str() );
                return;
            }

            //-------------------------------------------------------------------------

            auto pMesh = m_pResource.GetPtr();

            KRG_ASSERT( pMesh != nullptr );

            ImGui::PushStyleVar( ImGuiStyleVar_CellPadding, ImVec2( 4, 2 ) );
            if ( ImGui::BeginTable( "MeshInfoTable", 2, ImGuiTableFlags_Borders ) )
            {
                ImGui::TableSetupColumn( "Label", ImGuiTableColumnFlags_WidthFixed, 110 );
                ImGui::TableSetupColumn( "Data", ImGuiTableColumnFlags_NoHide );

                //-------------------------------------------------------------------------

                ImGui::TableNextRow();

                ImGui::TableNextColumn();
                ImGui::Text( "Data Path:" );

                ImGui::TableNextColumn();
                ImGui::Text( pMesh->GetResourceID().c_str() );

                //-------------------------------------------------------------------------

                ImGui::TableNextRow();

                ImGui::TableNextColumn();
                ImGui::Text( "Num Vertices:" );

                ImGui::TableNextColumn();
                ImGui::Text( "%d", pMesh->GetNumVertices() );

                ImGui::TableNextRow();

                ImGui::TableNextColumn();
                ImGui::Text( "Num Indices:" );

                ImGui::TableNextColumn();
                ImGui::Text( "%d", pMesh->GetNumIndices() );

                ImGui::TableNextRow();

                ImGui::TableNextColumn();
                ImGui::Text( "Geometry Sections:" );

                ImGui::TableNextColumn();
                for ( auto const& section : pMesh->GetSections() )
                {
                    ImGui::Text( section.m_ID.c_str() );
                }

                ImGui::EndTable();
            }
            ImGui::PopStyleVar();
        };

        //-------------------------------------------------------------------------

        ImGui::SetNextWindowClass( pWindowClass );
        if ( ImGui::Begin( s_infoWindowName ) )
        {
            DrawWindowContents();
        }
        ImGui::End();
    }
}