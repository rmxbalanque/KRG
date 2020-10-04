#pragma once
#ifdef _WIN32

//-------------------------------------------------------------------------

struct ID3D11Resource;
struct ID3D11ShaderResourceView;

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        namespace DX11
        {
            struct TextureSRV
            {
                ID3D11Resource*             m_pTextureResource = nullptr;
                ID3D11ShaderResourceView*   m_pTextureSRV = nullptr;
            };
        }
    }
}

#endif