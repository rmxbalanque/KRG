#include "RenderTexture.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        Texture Texture::InitializeTexture( TextureFormat format, Int2 dimensions, Byte* pData, size_t dataSize )
        {
            KRG_ASSERT( pData != nullptr );

            Texture texture;
            texture.m_format = format;
            texture.m_dimensions = dimensions;
            texture.m_rawData.resize( dataSize );
            memcpy( texture.m_rawData.data(), pData, dataSize );

            return texture;
        }

        //-------------------------------------------------------------------------

        Texture::Texture()
            : m_format( TextureFormat::DDS )
            , m_dimensions( 0, 0 )
        {}
    }
}