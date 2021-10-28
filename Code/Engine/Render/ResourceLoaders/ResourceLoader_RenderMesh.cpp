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

    bool MeshLoader::LoadInternal( ResourceID const& resourceID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryMemoryArchive& archive ) const
    {
        KRG_ASSERT( m_pRenderDevice != nullptr );
        KRG_ASSERT( archive.IsValid() );

        Resource::IResource* pResource = nullptr;

        // Static Mesh
        if ( resourceID.GetResourceTypeID() == StaticMesh::GetStaticResourceTypeID() )
        {
            StaticMesh* pStaticMesh = KRG::New<StaticMesh>();
            archive >> *pStaticMesh;
            pResource = pStaticMesh;
        }
        else // Skeletal Mesh
        {
            SkeletalMesh* pSkeletalMesh = KRG::New<SkeletalMesh>();
            archive >> *pSkeletalMesh;
            pResource = pSkeletalMesh;
        }

        //-------------------------------------------------------------------------

        pResourceRecord->SetResourceData( pResource );

        return true;
    }

    Resource::InstallResult MeshLoader::Install( ResourceID const& resourceID, Resource::ResourceRecord* pResourceRecord, Resource::InstallDependencyList const& installDependencies ) const
    {
        // Create GPU buffers
        //-------------------------------------------------------------------------

        // BLOCKING FOR NOW! TODO: request the load and return Resource::InstallResult::InProgress
        auto pMesh = pResourceRecord->GetResourceData<Mesh>();
        CreateRenderBuffers( pMesh );

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
            m_pRenderDevice->DestroyBuffer( pMesh->m_vertexBuffer );
            m_pRenderDevice->DestroyBuffer( pMesh->m_indexBuffer );
        }
    }

    Resource::InstallResult MeshLoader::UpdateInstall( ResourceID const& resourceID, Resource::ResourceRecord* pResourceRecord ) const
    {
        // TODO: implement when moving to theforge
        KRG_UNIMPLEMENTED_FUNCTION();
        return Resource::InstallResult::Failed;
    }
}