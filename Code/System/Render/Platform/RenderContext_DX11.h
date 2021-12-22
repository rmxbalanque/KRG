#pragma once
#ifdef _WIN32

#include "System/Render/_Module/API.h"

#include "System/Render/RenderStates.h"
#include "System/Render/RenderShader.h"
#include "System/Render/RenderTexture.h"
#include "System/Render/RenderBuffer.h"
#include "System/Render/RenderTarget.h"
#include "System/Render/RenderWindow.h"
#include "System/Render/RenderPipelineState.h"

#include <D3D11.h>

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        class KRG_SYSTEM_RENDER_API RenderContext
        {
            friend class RenderDevice;

            static ID3D11DepthStencilState* s_pDepthTestingOn;
            static ID3D11DepthStencilState* s_pDepthTestingOff;
            static ID3D11DepthStencilState* s_pDepthTestingReadOnly;

            static Float4 const s_defaultClearColor;

        public:

            RenderContext() = default;

            inline bool IsValid() const { return m_pDeviceContext != nullptr; }

            void SetPipelineState( PipelineState const& pipelineState ) const;

            // Shaders
            void SetShaderInputBinding( ShaderInputBindingHandle const& inputBinding ) const;
            void SetShaderResource( PipelineStage stage, uint32 slot, ViewSRVHandle const& shaderResourceView ) const;
            void ClearShaderResource( PipelineStage stage, uint32 slot ) const;
            void SetUnorderedAccess( PipelineStage stage, uint32 slot, ViewUAVHandle const& shaderResourceView ) const;
            void ClearUnorderedAccess( PipelineStage stage, uint32 slot ) const;
            void SetSampler( PipelineStage stage, uint32 slot, SamplerState const& state ) const;

            // Buffers
            void* MapBuffer( RenderBuffer const& buffer ) const;
            void UnmapBuffer( RenderBuffer const& buffer ) const;
            void WriteToBuffer( RenderBuffer const& buffer, void const* pData, size_t const dataSize ) const;
            void SetVertexBuffer( RenderBuffer const& buffer, uint32 offset = 0 ) const;
            void SetIndexBuffer( RenderBuffer const& buffer, uint32 offset = 0 ) const;

            // Rasterizer
            void SetViewport( Float2 dimensions, Float2 topLeft, Float2 zRange = Float2(0, 1) ) const;
            void SetDepthTestMode( DepthTestMode mode ) const;
            void SetRasterizerScissorRectangles( ScissorRect const* pScissorRects, uint32 numRects = 0 ) const;
            void SetBlendState( BlendState const& blendState ) const;

            // Render Targets
            void SetRenderTarget( RenderTarget const& renderTarget ) const;
            void SetRenderTarget( ViewDSHandle const& dsView ) const;
            void SetRenderTarget( nullptr_t ) const;
            void ClearDepthStencilView( ViewDSHandle const& dsView, float depth, uint8 stencil ) const;
            void ClearRenderTargetViews( RenderTarget const& renderTarget ) const;

            // Drawing
            void SetPrimitiveTopology( Topology topology ) const;
            void Draw( uint32 vertexCount, uint32 vertexStartIndex = 0 ) const;
            void DrawIndexed( uint32 vertexCount, uint32 indexStartIndex = 0, uint32 vertexStartIndex = 0 ) const;

            void Dispatch( uint32 numGroupsX, uint32 numGroupsY, uint32 numGroupsZ ) const;

            // Window
            void Present( RenderWindow& window ) const;

        private:

            RenderContext( ID3D11DeviceContext* pDeviceContext );

        private:

            ID3D11DeviceContext* m_pDeviceContext = nullptr;
        };
    }
}

#endif