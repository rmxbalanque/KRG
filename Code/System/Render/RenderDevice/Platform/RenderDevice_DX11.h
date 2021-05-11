#pragma once
#ifdef _WIN32

#include "System/Render/_Module/API.h"

#include "RenderContext_DX11.h"
#include "System/Core/Types/Color.h"
#include "System/Core/Threading/Threading.h"

#include <D3D11.h>

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class Settings;

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_RENDER_API RenderDevice
    {

    public:

        RenderDevice();
        ~RenderDevice();

        //-------------------------------------------------------------------------

        inline RenderContext const& GetImmediateContext() const { return m_immediateContext; }

        bool IsInitialized() const;
        bool Initialize( Settings const& settings );
        void Shutdown();

        // Device locking: required since we create/destroy resources while rendering
        //-------------------------------------------------------------------------

        void LockDevice() { m_deviceMutex.lock(); }
        void UnlockDevice() { m_deviceMutex.unlock(); }

        // Drawing
        //-------------------------------------------------------------------------

        void PresentFrame();

        void ResizeRenderTargets( Int2 const& dimensions );
        inline Int2 GetRenderTargetDimensions() const { return m_resolution; }

        // Resource and state management
        //-------------------------------------------------------------------------

        // Shaders
        void CreateShader( Shader& shader );
        void DestroyShader( Shader& shader );

        // Buffers
        void CreateBuffer( RenderBuffer& buffer, void const* pInitializationData = nullptr );
        void ResizeBuffer( RenderBuffer& buffer, uint32 newSize );
        void DestroyBuffer( RenderBuffer& buffer );

        // Vertex shader input mappings
        void CreateShaderInputBinding( VertexShader const& shader, VertexLayoutDescriptor const& vertexLayoutDesc, ResourceHandle& inputBinding );
        void DestroyShaderInputBinding( ResourceHandle& inputBinding );

        // Rasterizer
        void CreateRasterizerState( RasterizerState& stateDesc );
        void DestroyRasterizerState( RasterizerState& state );

        void CreateBlendState( BlendState& stateDesc );
        void DestroyBlendState( BlendState& state );

        // Textures and Sampling
        void CreateTexture( Texture& texture );
        void DestroyTexture( Texture& texture );

        void CreateSamplerState( SamplerState& state );
        void DestroySamplerState( SamplerState& state );

    private:

        bool CreateDeviceAndSwapchain();
        void DestroyDeviceAndSwapchain();

        bool CreateRenderTargets();
        void DestroyRenderTargets();

        bool CreateDepthStencilView();
        void DestroyDepthStencilView();

        bool CreateDefaultDepthStencilStates();
        void DestroyDefaultDepthStencilStates();

        void CreateRawTexture( Texture& texture );
        void CreateDDSTexture( Texture& texture );

    private:

        Int2                        m_resolution;
        float                       m_refreshRate;
        bool                        m_fullscreen;

        // Device Core 
        ID3D11Device*               m_pDevice;
        IDXGISwapChain*             m_pSwapChain;
        RenderContext               m_immediateContext;

        // Render targets
        ID3D11RenderTargetView*     m_pRenderTargetView;

        // Depth testing
        ID3D11Texture2D*            m_pDepthStencil;
        ID3D11DepthStencilView*     m_pDepthStencilView;

        // lock to allow loading resources while rendering across different threads
        Threading::Mutex            m_deviceMutex;
    };
}

#endif