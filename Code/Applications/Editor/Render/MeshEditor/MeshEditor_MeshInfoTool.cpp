#include "MeshEditor_MeshInfoTool.h"
#include "System/Resource/ResourceSystem.h"
#include "System/DevTools/CommonWidgets/InterfaceHelpers.h"

//-------------------------------------------------------------------------

namespace KRG::Render::MeshEditor
{
    void MeshInfo::Shutdown( UpdateContext const& context )
    {
        UnloadAnyLoadedResources( context );

        TEditorTool<Model>::Shutdown( context );
    }

    void MeshInfo::UnloadAnyLoadedResources( UpdateContext const& context )
    {
        auto pResourceSystem = context.GetSystem<Resource::ResourceSystem>();

        if ( m_pStaticMesh.IsValid() )
        {
            pResourceSystem->UnloadResource( m_pStaticMesh );
            m_pStaticMesh = nullptr;
        }

        if ( m_pSkeletalMesh.IsValid() )
        {
            pResourceSystem->UnloadResource( m_pSkeletalMesh );
            m_pSkeletalMesh = nullptr;
        }
    }

    //-------------------------------------------------------------------------

    void MeshInfo::FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
        DataPath const& previewedMesh = m_model.GetPreviewedMeshPath();
        if ( previewedMesh.IsValid() && previewedMesh != m_currentlyPreviewedMesh )
        {
            // Unload any previously loaded meshes
            //-------------------------------------------------------------------------

            if ( m_currentlyPreviewedMesh.IsValid() )
            {
                UnloadAnyLoadedResources( context );
            }

            m_currentlyPreviewedMesh = previewedMesh;

            // Load mesh
            //-------------------------------------------------------------------------

            auto pResourceSystem = context.GetSystem<Resource::ResourceSystem>();
            ResourceID const resourceID( previewedMesh );
            if ( resourceID.GetResourceTypeID() == StaticMesh::GetStaticResourceTypeID() )
            {
                m_pStaticMesh = resourceID;
                pResourceSystem->LoadResource( m_pStaticMesh );
            }
            else
            {
                m_pSkeletalMesh = resourceID;
                pResourceSystem->LoadResource( m_pSkeletalMesh );
            }
        }
        else
        {
            // Clear previewed mesh and unload any loaded meshes
            if ( !previewedMesh.IsValid() && m_currentlyPreviewedMesh.IsValid() )
            {
                m_currentlyPreviewedMesh = DataPath();
                UnloadAnyLoadedResources( context );
            }
        }

        //-------------------------------------------------------------------------

        if ( ImGui::Begin( "Mesh Info" ) )
        {
            if ( m_currentlyPreviewedMesh.IsValid() )
            {
                constexpr static ImGuiTableFlags const flags = ImGuiTableFlags_Borders;

                ImGui::PushStyleVar( ImGuiStyleVar_CellPadding, ImVec2( 4, 2 ) );
                if ( ImGui::BeginTable( "MeshInfoTable", 2, flags ) )
                {
                    ImGui::TableSetupColumn( "Label", ImGuiTableColumnFlags_WidthFixed, 110 );
                    ImGui::TableSetupColumn( "Data", ImGuiTableColumnFlags_NoHide );

                    //-------------------------------------------------------------------------

                    ImGui::TableNextRow();

                    ImGui::TableNextColumn();
                    ImGui::Text( "Data Path:" );

                    ImGui::TableNextColumn();
                    ImGui::Text( m_currentlyPreviewedMesh.c_str() );

                    //-------------------------------------------------------------------------

                    // Loading indicator
                    if ( IsLoadingMesh() )
                    {
                        ImGui::TableNextColumn();
                        ImGui::Text( "Loading:" );

                        ImGui::TableNextColumn();
                        ImGuiX::DrawSpinner( "Loading" );
                    }
                    else // Draw basic mesh info
                    {
                        Mesh const* pMesh = IsPreviewingStaticMesh() ? static_cast<Mesh const*>( m_pStaticMesh.GetPtr() ) : static_cast<Mesh const*>( m_pSkeletalMesh.GetPtr() );

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

                        if ( IsPreviewingSkeletalMesh() )
                        {
                            ImGui::TableNextColumn();
                            ImGui::Text( "Num Bones:" );

                            ImGui::TableNextColumn();
                            ImGui::Text( "%d", m_pSkeletalMesh->GetNumBones() );
                        }
                    }

                    ImGui::EndTable();
                }
                ImGui::PopStyleVar();

                //-------------------------------------------------------------------------

                if ( IsPreviewingSkeletalMesh() && !IsLoadingMesh() )
                {
                    DrawSkeletalMeshInfo( context );
                }
            }
            else
            {
                ImGui::Text( "No Mesh Previewed" );
            }
        }
        ImGui::End();
    }

    void MeshInfo::DrawSkeletalMeshInfo( UpdateContext const& context )
    {
        ImGui::PushStyleVar( ImGuiStyleVar_ChildRounding, 3.0f );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 8, 2 ) );
        ImGui::BeginChild( "SkeletonView", ImVec2( 0, 0 ), true, ImGuiWindowFlags_AlwaysVerticalScrollbar );
        {
            for ( auto i = 0; i < m_pSkeletalMesh->GetNumBones(); i++ )
            {
                ImGui::Text( m_pSkeletalMesh->GetBoneID( i ).c_str() );
            }
        }
        ImGui::EndChild();
        ImGui::PopStyleVar( 2 );
    }
}