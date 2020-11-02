#include "TextureLoader.h"
#include "System/Core/Serialization/BinaryArchive.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------
namespace KRG
{
    namespace Render
    {
        bool TextureLoader::LoadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryMemoryArchive& archive ) const
        {
            KRG_ASSERT( m_pRenderDevice != nullptr && archive.IsValid() );

            Texture* pTextureResource = KRG::New<Texture>();
            KRG_ASSERT( pTextureResource != nullptr );
            archive >> *pTextureResource;

            pResourceRecord->SetResourceData( pTextureResource );

            if ( !pTextureResource->HasRawData() )
            {
                KRG_LOG_ERROR( "Resource", "Failed to install texture resource: %s, compiled resource has no data", resID.ToString().c_str() );
                return false;
            }

            m_pRenderDevice->CreateTexture( *pTextureResource );
            if ( !pTextureResource->IsValid() )
            {
                KRG_LOG_ERROR( "Resource", "Failed to create texture handle for texture: %s", resID.ToString().c_str() );
                return false;
            }

            return true;
        }

        void TextureLoader::UnloadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord ) const
        {
            KRG_ASSERT( m_pRenderDevice != nullptr );

            auto pTextureResource = pResourceRecord->GetResourceData<Texture>();
            if ( pTextureResource != nullptr && pTextureResource->IsValid() )
            {
                m_pRenderDevice->DestroyTexture( *pTextureResource );
            }

            ResourceLoader::UnloadInternal( resID, pResourceRecord );
        }
    }
}