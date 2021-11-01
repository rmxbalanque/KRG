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

    bool MeshLoader::LoadInternal( ResourceID const& resourceID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryMemoryArchive& archive ) const
    {
        KRG_ASSERT( m_pRenderDevice != nullptr );
        KRG_ASSERT( archive.IsValid() );

        Mesh* pMeshResource = nullptr;

        // Static Mesh
        if ( resourceID.GetResourceTypeID() == StaticMesh::GetStaticResourceTypeID() )
        {
            StaticMesh* pStaticMesh = KRG::New<StaticMesh>();
            archive >> *pStaticMesh;
            pMeshResource = pStaticMesh;
        }
        else // Skeletal Mesh
        {
            SkeletalMesh* pSkeletalMesh = KRG::New<SkeletalMesh>();
            archive >> *pSkeletalMesh;
            pMeshResource = pSkeletalMesh;
        }

        KRG_ASSERT( !pMeshResource->m_vertices.empty() );
        KRG_ASSERT( !pMeshResource->m_indices.empty() );

        //-------------------------------------------------------------------------

        pResourceRecord->SetResourceData( pMeshResource );

        return true;
    }

    Resource::InstallResult MeshLoader::Install( ResourceID const& resourceID, Resource::ResourceRecord* pResourceRecord, Resource::InstallDependencyList const& installDependencies ) const
    {
        auto pMesh = pResourceRecord->GetResourceData<Mesh>();

        // Create GPU buffers
        //-------------------------------------------------------------------------
        // BLOCKING FOR NOW! TODO: request the load and return Resource::InstallResult::InProgress

        m_pRenderDevice->LockDevice();
        {
            m_pRenderDevice->CreateBuffer( pMesh->m_vertexBuffer, pMesh->m_vertices.data() );
            KRG_ASSERT( pMesh->m_vertexBuffer.IsValid() );

            m_pRenderDevice->CreateBuffer( pMesh->m_indexBuffer, pMesh->m_indices.data() );
            KRG_ASSERT( pMesh->m_indexBuffer.IsValid() );
        }
        m_pRenderDevice->UnlockDevice();

        // Set materials
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
                KRG_LOG_ERROR( "Resource", "Failed to install resource: %s", resourceID.ToString().c_str() );
                return Resource::InstallResult::Failed;
            }
        }

        //-------------------------------------------------------------------------

        ResourceLoader::Install( resourceID, pResourceRecord, installDependencies );
        return Resource::InstallResult::Succeeded;
    }

    void MeshLoader::Uninstall( ResourceID const& resourceID, Resource::ResourceRecord* pResourceRecord ) const
    {
        auto pMesh = pResourceRecord->GetResourceData<Mesh>();
        if ( pMesh != nullptr )
        {
            m_pRenderDevice->LockDevice();
            m_pRenderDevice->DestroyBuffer( pMesh->m_vertexBuffer );
            m_pRenderDevice->DestroyBuffer( pMesh->m_indexBuffer );
            m_pRenderDevice->UnlockDevice();
        }
    }

    Resource::InstallResult MeshLoader::UpdateInstall( ResourceID const& resourceID, Resource::ResourceRecord* pResourceRecord ) const
    {
        // TODO: implement when moving to theforge
        KRG_UNIMPLEMENTED_FUNCTION();
        return Resource::InstallResult::Failed;
    }
}