#include "MeshLoader.h"
#include "System/Render/Mesh/StaticMesh.h"
#include "System/Render/Mesh/SkeletalMesh.h"
#include "System/Render/RenderDevice/RenderDevice.h"
#include "System/Core/Serialization/BinaryArchive.h"

//-------------------------------------------------------------------------
namespace KRG
{
    namespace Render
    {
        MeshLoader::MeshLoader()
        {
            m_loadableTypes.push_back( StaticMesh::GetStaticResourceTypeID() );
            m_loadableTypes.push_back( SkeletalMesh::GetStaticResourceTypeID() );
        }

        void MeshLoader::CreateRenderBuffers( MeshGeometry* pMeshGeometry ) const
        {
            KRG_ASSERT( pMeshGeometry != nullptr && !pMeshGeometry->m_vertexBuffer.IsValid() && !pMeshGeometry->m_indexBuffer.IsValid() );

            if ( pMeshGeometry->m_vertices.empty() )
            {
                return;
            }

            KRG_ASSERT( !pMeshGeometry->m_indices.empty() );

            //-------------------------------------------------------------------------

            m_pRenderDevice->CreateBuffer( pMeshGeometry->m_vertexBuffer, pMeshGeometry->m_vertices.data() );
            KRG_ASSERT( pMeshGeometry->m_vertexBuffer.IsValid() );

            m_pRenderDevice->CreateBuffer( pMeshGeometry->m_indexBuffer, pMeshGeometry->m_indices.data() );
            KRG_ASSERT( pMeshGeometry->m_indexBuffer.IsValid() );
        }

        bool MeshLoader::LoadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryMemoryArchive& archive ) const
        {
            KRG_ASSERT( m_pRenderDevice != nullptr );
            KRG_ASSERT( archive.IsValid() );

            Resource::IResource* pResource = nullptr;

            if ( resID.GetResourceTypeID() == StaticMesh::GetStaticResourceTypeID() )
            {
                StaticMesh* pStaticMesh = KRG::New<StaticMesh>();
                archive >> *pStaticMesh;
                pResource = pStaticMesh;

                CreateRenderBuffers( pStaticMesh );
            }
            else // Skeletal Mesh
            {
                SkeletalMesh* pSkeletalMesh = KRG::New<SkeletalMesh>();
                archive >> *pSkeletalMesh;
                pResource = pSkeletalMesh;

                CreateRenderBuffers( pSkeletalMesh );

                // Create the inverse bind pose
                auto const numBones = pSkeletalMesh->m_bindPose.size();
                pSkeletalMesh->m_inverseBindPose.resize( numBones );
                for ( auto i = 0; i < numBones; i++ )
                {
                    pSkeletalMesh->m_inverseBindPose[i] = pSkeletalMesh->m_bindPose[i].GetInverse();
                }
            }

            //-------------------------------------------------------------------------

            pResourceRecord->SetResourceData( pResource );

            return true;
        }

        void MeshLoader::UnloadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord ) const
        {
            KRG_ASSERT( m_pRenderDevice != nullptr );

            MeshGeometry* pMeshGeometry = nullptr;
            if ( resID.GetResourceTypeID() == StaticMesh::GetStaticResourceTypeID() )
            {
                pMeshGeometry = pResourceRecord->GetResourceData<StaticMesh>();
            }
            else // Skeletal Mesh
            {
                pMeshGeometry = pResourceRecord->GetResourceData<SkeletalMesh>();
            }

            //-------------------------------------------------------------------------

            if ( pMeshGeometry != nullptr )
            {
                m_pRenderDevice->DestroyBuffer( pMeshGeometry->m_vertexBuffer );
                m_pRenderDevice->DestroyBuffer( pMeshGeometry->m_indexBuffer );
            }

            ResourceLoader::UnloadInternal( resID, pResourceRecord );
        }
    }
}