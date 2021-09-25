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

        RenderDevice() = default;
        ~RenderDevice();

        //-------------------------------------------------------------------------

        bool IsInitialized() const;
        bool Initialize( Settings const& settings );
        void Shutdown();

        // Pipeline state changes
        //-------------------------------------------------------------------------

        inline RenderContext const& GetImmediateContext() const { return m_immediateContext; }
        RenderTarget const& GetMainRenderTarget() const& { return m_renderTarget; }

        // Device locking: required since we create/destroy resources while rendering
        //-------------------------------------------------------------------------

        void LockDevice() { m_deviceMutex.lock(); }
        void UnlockDevice() { m_deviceMutex.unlock(); }

        // Drawing
        //-------------------------------------------------------------------------

        void PresentFrame();

        void ResizeMainRenderTarget( Int2 const& dimensions );
        inline Int2 GetMainRenderTargetDimensions() const { return m_resolution; }

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
        void CreateTexture( Texture& texture, TextureFormat format, Byte const* rawData, size_t size );
        inline void CreateTexture( Texture& texture, TextureFormat format, TVector<Byte> const& rawData ) { CreateTexture( texture, format, rawData.data(), rawData.size() ); }
        void DestroyTexture( Texture& texture );

        void CreateSamplerState( SamplerState& state );
        void DestroySamplerState( SamplerState& state );

        // Render Targets
        void CreateRenderTarget( RenderTarget& renderTarget, Int2 const& dimensions );
        void ResizeRenderTarget( RenderTarget& renderTarget, Int2 const& newDimensions );
        void DestroyRenderTarget( RenderTarget& renderTarget );

    private:

        bool CreateDeviceAndSwapchain();
        void DestroyDeviceAndSwapchain();

        bool CreateMainRenderTarget();
        void DestroyMainRenderTarget();

        bool CreateDefaultDepthStencilStates();
        void DestroyDefaultDepthStencilStates();

        void CreateRawTexture( Texture& texture, Byte const* pRawData, size_t rawDataSize );
        void CreateDDSTexture( Texture& texture, Byte const* pRawData, size_t rawDataSize );

    private:

        Int2                        m_resolution = Int2( 0 );
        float                       m_refreshRate = 0;
        bool                        m_fullscreen = false;

        // Device Core 
        ID3D11Device*               m_pDevice = nullptr;
        IDXGISwapChain*             m_pSwapChain = nullptr;
        RenderContext               m_immediateContext;

        // Render targets
        RenderTarget                m_renderTarget;

        // lock to allow loading resources while rendering across different threads
        Threading::Mutex            m_deviceMutex;
    };
}

#endif