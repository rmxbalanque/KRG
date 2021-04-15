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
            if ( IsPreviewingStaticMesh() )
            {
                DrawStaticMeshInfo( context );
            }
            else if ( IsPreviewingSkeletalMesh() )
            {
                DrawSkeletalMeshInfo( context );
            }
            else
            {
                ImGui::Text( "No Mesh Previewed" );
            }
        }
        ImGui::End();
    }

    void MeshInfo::DrawStaticMeshInfo( UpdateContext const& context )
    {
        KRG_ASSERT( IsPreviewingStaticMesh() );

        ImGui::Text( m_currentlyPreviewedMesh.c_str() );

        if ( m_pStaticMesh.IsLoaded() )
        {
            ImGui::Text( "Num Vertices: %d", m_pStaticMesh->GetNumVertices() );
            ImGui::Text( "Num Indices: %d", m_pStaticMesh->GetNumIndices() );
            ImGui::Text( "Num Geometry Sections: %d", m_pStaticMesh->GetNumSections() );
        }
        else
        {
            ImGuiX::DrawSpinner( "Loading" );
        }
    }

    void MeshInfo::DrawSkeletalMeshInfo( UpdateContext const& context )
    {
        KRG_ASSERT( IsPreviewingSkeletalMesh() );

        ImGui::Text( m_currentlyPreviewedMesh.c_str() );

        if ( m_pSkeletalMesh.IsLoaded() )
        {
            ImGui::Text( "Num Vertices: %d", m_pSkeletalMesh->GetNumVertices() );
            ImGui::Text( "Num Indices: %d", m_pSkeletalMesh->GetNumIndices() );
            ImGui::Text( "Num Sections: %d", m_pSkeletalMesh->GetNumSections() );
            ImGui::Text( "Num Bones: %d", m_pSkeletalMesh->GetNumBones() );
        }
        else
        {
            ImGuiX::DrawSpinner( "Loading" );
        }
    }
}