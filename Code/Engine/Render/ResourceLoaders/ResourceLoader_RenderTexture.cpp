#include "ResourceLoader_RenderTexture.h"
#include "System/Core/Serialization/BinaryArchive.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    bool TextureLoader::LoadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryMemoryArchive& archive ) const
    {
        KRG_ASSERT( m_pRenderDevice != nullptr && archive.IsValid() );

        Texture* pTextureResource = KRG::New<Texture>();
        KRG_ASSERT( pTextureResource != nullptr );
        archive >> *pTextureResource;

        if ( pTextureResource->m_rawData.empty() )
        {
            KRG_LOG_ERROR( "Resource", "Failed to load texture resource: %s, compiled resource has no data", resID.ToString().c_str() );
            return false;
        }

        pResourceRecord->SetResourceData( pTextureResource );
        return true;
    }

    Resource::InstallResult TextureLoader::Install( ResourceID const& resourceID, Resource::ResourceRecord* pResourceRecord, Resource::InstallDependencyList const& installDependencies ) const
    {
        auto pTextureResource = pResourceRecord->GetResourceData<Texture>();

        // BLOCKING FOR NOW! TODO: request the load and return Resource::InstallResult::InProgress
        m_pRenderDevice->CreateTexture( *pTextureResource, pTextureResource->m_format, pTextureResource->m_rawData );

        ResourceLoader::Install( resourceID, pResourceRecord, installDependencies );
        return Resource::InstallResult::Succeeded;
    }

    void TextureLoader::Uninstall( ResourceID const& resourceID, Resource::ResourceRecord* pResourceRecord ) const
    {
        auto pTextureResource = pResourceRecord->GetResourceData<Texture>();
        if ( pTextureResource != nullptr && pTextureResource->IsValid() )
        {
            m_pRenderDevice->DestroyTexture( *pTextureResource );
        }
    }

    Resource::InstallResult TextureLoader::UpdateInstall( ResourceID const& resourceID, Resource::ResourceRecord* pResourceRecord ) const
    {
        // TODO: implement when moving to theforge
        KRG_UNIMPLEMENTED_FUNCTION();
        return Resource::InstallResult::Failed;
    }
}