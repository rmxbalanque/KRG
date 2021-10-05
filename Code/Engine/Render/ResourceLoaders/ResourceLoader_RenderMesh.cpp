#include "ResourceLoader_RenderMesh.h"
#include "Engine/Render/Mesh/StaticMesh.h"
#include "Engine/Render/Mesh/SkeletalMesh.h"
#include "System/Render/RenderDevice.h"
#include "System/Core/Serialization/BinaryArchive.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    MeshLoader::MeshLoader()
    {
        m_loadableTypes.push_back( StaticMesh::GetStaticResourceTypeID() );
        m_loadableTypes.push_back( SkeletalMesh::GetStaticResourceTypeID() );
    }

    void MeshLoader::CreateRenderBuffers( Mesh* pMesh ) const
    {
        KRG_ASSERT( pMesh != nullptr && !pMesh->m_vertexBuffer.IsValid() && !pMesh->m_indexBuffer.IsValid() );

        if ( pMesh->m_vertices.empty() )
        {
            return;
        }

        KRG_ASSERT( !pMesh->m_indices.empty() );

        //-------------------------------------------------------------------------

        m_pRenderDevice->CreateBuffer( pMesh->m_vertexBuffer, pMesh->m_vertices.data() );
        KRG_ASSERT( pMesh->m_vertexBuffer.IsValid() );

        m_pRenderDevice->CreateBuffer( pMesh->m_indexBuffer, pMesh->m_indices.data() );
        KRG_ASSERT( pMesh->m_indexBuffer.IsValid() );
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
        }

        //-------------------------------------------------------------------------

        pResourceRecord->SetResourceData( pResource );

        return true;
    }

    void MeshLoader::UnloadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord ) const
    {
        KRG_ASSERT( m_pRenderDevice != nullptr );

        Mesh* pMesh = nullptr;

        if ( resID.GetResourceTypeID() == StaticMesh::GetStaticResourceTypeID() )
        {
            pMesh = pResourceRecord->GetResourceData<StaticMesh>();
        }
        else // Skeletal Mesh
        {
            pMesh = pResourceRecord->GetResourceData<SkeletalMesh>();
        }

        //-------------------------------------------------------------------------

        if ( pMesh != nullptr )
        {
            m_pRenderDevice->DestroyBuffer( pMesh->m_vertexBuffer );
            m_pRenderDevice->DestroyBuffer( pMesh->m_indexBuffer );
        }

        ResourceLoader::UnloadInternal( resID, pResourceRecord );
    }

    bool MeshLoader::Install( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Resource::InstallDependencyList const& installDependencies ) const
    {
        Mesh* pMesh = pResourceRecord->GetResourceData<Mesh>();

        //-------------------------------------------------------------------------

        for ( auto& pMaterial : pMesh->m_materials )
        {
            // Default materials are allowed to be unset
            if ( !pMaterial.GetResourceID().IsValid() )
            {
                continue;
            }

            // If the material is set, we need to retrieve it and ensure that it is correctly loaded
            pMaterial = GetInstallDependency( installDependencies, pMaterial.GetResourceID() );
            if ( !pMaterial->IsValid() )
            {
                KRG_LOG_ERROR( "Resource", "Failed to install resource: %s", resID.ToString().c_str() );
                return false;
            }
        }

        //-------------------------------------------------------------------------

        ResourceLoader::Install( resID, pResourceRecord, installDependencies );
        return true;
    }
}