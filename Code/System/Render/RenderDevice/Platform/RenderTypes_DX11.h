#pragma once
#ifdef _WIN32

//-------------------------------------------------------------------------

struct ID3D11Resource;
struct ID3D11ShaderResourceView;

//-------------------------------------------------------------------------

namespace KRG::Render::DX11
{
    struct TextureSRV
    {
        ID3D11Resource*             m_pTextureResource = nullptr;
        ID3D11ShaderResourceView*   m_pTextureSRV = nullptr;
    };

    struct RenderTargetSRV
    {
        ID3D11RenderTargetView*   m_pRenderTargetView = nullptr;
        ID3D11DepthStencilView*   m_pDepthStencilView = nullptr;
    };
}

#endif