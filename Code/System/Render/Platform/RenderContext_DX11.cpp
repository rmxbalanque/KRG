#include "RenderContext_DX11.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        ID3D11DepthStencilState* RenderContext::s_pDepthTestingOn = nullptr;
        ID3D11DepthStencilState* RenderContext::s_pDepthTestingOff = nullptr;
        ID3D11DepthStencilState* RenderContext::s_pDepthTestingReadOnly = nullptr;

        //-------------------------------------------------------------------------

        RenderContext::RenderContext( ID3D11DeviceContext* pDeviceContext )
            : m_pDeviceContext( pDeviceContext )
        {
            KRG_ASSERT( m_pDeviceContext != nullptr );
        }

        //-------------------------------------------------------------------------

        void RenderContext::SetPipelineState( PipelineState const& pipelineState ) const
        {
            // VS
            //-------------------------------------------------------------------------

            if ( pipelineState.m_pVertexShader != nullptr && pipelineState.m_pVertexShader->IsValid() )
            {
                m_pDeviceContext->VSSetShader( (ID3D11VertexShader*) pipelineState.m_pVertexShader->GetShaderHandle().m_pData0, nullptr, 0 );

                auto const numCbuffers = pipelineState.m_pVertexShader->GetNumConstBuffers();
                for ( auto i = 0u; i < numCbuffers; i++ )
                {
                    m_pDeviceContext->VSSetConstantBuffers( (uint32) i, 1, (ID3D11Buffer**) &pipelineState.m_pVertexShader->GetConstBuffer( i ).GetResourceHandle().m_pData0 );
                }
            }
            else
            {
                m_pDeviceContext->VSSetShader( nullptr, nullptr, 0 );
            }

            m_pDeviceContext->IASetInputLayout( nullptr );

            // GS
            //-------------------------------------------------------------------------

            if ( pipelineState.m_pGeometryShader != nullptr && pipelineState.m_pGeometryShader->IsValid() )
            {
                m_pDeviceContext->GSSetShader( (ID3D11GeometryShader*) pipelineState.m_pGeometryShader->GetShaderHandle().m_pData0, nullptr, 0 );

                auto const numCbuffers = pipelineState.m_pGeometryShader->GetNumConstBuffers();
                for ( auto i = 0u; i < numCbuffers; i++ )
                {
                    m_pDeviceContext->GSSetConstantBuffers( (uint32) i, 1, (ID3D11Buffer**) &pipelineState.m_pGeometryShader->GetConstBuffer( i ).GetResourceHandle().m_pData0 );
                }
            }
            else
            {
                m_pDeviceContext->GSSetShader( nullptr, nullptr, 0 );
            }

            // HS
            //-------------------------------------------------------------------------

            if ( pipelineState.m_pHullShader != nullptr && pipelineState.m_pHullShader->IsValid() )
            {
                m_pDeviceContext->HSSetShader( (ID3D11HullShader*) pipelineState.m_pHullShader->GetShaderHandle().m_pData0, nullptr, 0 );

                auto const numCbuffers = pipelineState.m_pHullShader->GetNumConstBuffers();
                for ( auto i = 0u; i < numCbuffers; i++ )
                {
                    m_pDeviceContext->HSSetConstantBuffers( (uint32) i, 1, (ID3D11Buffer**) &pipelineState.m_pHullShader->GetConstBuffer( i ).GetResourceHandle().m_pData0 );
                }
            }
            else
            {
                m_pDeviceContext->HSSetShader( nullptr, nullptr, 0 );
            }

            // CS
            //-------------------------------------------------------------------------

            if ( pipelineState.m_pComputeShader != nullptr && pipelineState.m_pComputeShader->IsValid() )
            {
                m_pDeviceContext->CSSetShader( (ID3D11ComputeShader*) pipelineState.m_pComputeShader->GetShaderHandle().m_pData0, nullptr, 0 );

                auto const numCbuffers = pipelineState.m_pComputeShader->GetNumConstBuffers();
                for ( auto i = 0u; i < numCbuffers; i++ )
                {
                    m_pDeviceContext->CSSetConstantBuffers( (uint32) i, 1, (ID3D11Buffer**) &pipelineState.m_pComputeShader->GetConstBuffer( i ).GetResourceHandle().m_pData0 );
                }
            }
            else
            {
                m_pDeviceContext->CSSetShader( nullptr, nullptr, 0 );
            }

            // PS
            //-------------------------------------------------------------------------

            if ( pipelineState.m_pPixelShader != nullptr && pipelineState.m_pPixelShader->IsValid() )
            {
                m_pDeviceContext->PSSetShader( (ID3D11PixelShader*) pipelineState.m_pPixelShader->GetShaderHandle().m_pData0, nullptr, 0 );

                auto const numCbuffers = pipelineState.m_pPixelShader->GetNumConstBuffers();
                for ( auto i = 0u; i < numCbuffers; i++ )
                {
                    m_pDeviceContext->PSSetConstantBuffers( (uint32) i, 1, (ID3D11Buffer**) &pipelineState.m_pPixelShader->GetConstBuffer( i ).GetResourceHandle().m_pData0 );
                }
            }
            else
            {
                m_pDeviceContext->PSSetShader( nullptr, nullptr, 0 );
            }

            // RS
            //-------------------------------------------------------------------------

            if ( pipelineState.m_pBlendState != nullptr && pipelineState.m_pBlendState->IsValid() )
            {
                m_pDeviceContext->OMSetBlendState( (ID3D11BlendState*) pipelineState.m_pBlendState->GetResourceHandle().m_pData0, nullptr, 0xffffffff );
            }
            else
            {
                m_pDeviceContext->OMSetBlendState( nullptr, nullptr, 0xffffffff );
            }

            if ( pipelineState.m_pRasterizerState != nullptr && pipelineState.m_pRasterizerState->IsValid() )
            {
                m_pDeviceContext->RSSetState( (ID3D11RasterizerState*) pipelineState.m_pRasterizerState->GetResourceHandle().m_pData0 );
                SetRasterizerScissorRectangles( nullptr );
            }
        }

        //-------------------------------------------------------------------------
        void RenderContext::SetShaderInputBinding( ShaderInputBindingHandle const& inputBinding ) const
        {
            KRG_ASSERT( IsValid() );
            m_pDeviceContext->IASetInputLayout( inputBinding.IsValid() ? (ID3D11InputLayout*) inputBinding.m_pData0 : nullptr );
        }

        void RenderContext::SetShaderResource( PipelineStage stage, uint32 slot, ViewSRVHandle const& shaderResourceView ) const
        {
            KRG_ASSERT( IsValid() );

            auto pSRV = shaderResourceView.IsValid() ? (ID3D11ShaderResourceView*) shaderResourceView.m_pData0 : nullptr;

            switch ( stage )
            {
                case PipelineStage::Vertex:
                m_pDeviceContext->VSSetShaderResources( slot, 1, (ID3D11ShaderResourceView* const*) &pSRV );
                break;

                case PipelineStage::Geometry:
                m_pDeviceContext->GSSetShaderResources( slot, 1, (ID3D11ShaderResourceView* const*) &pSRV );
                break;

                case PipelineStage::Pixel:
                m_pDeviceContext->PSSetShaderResources( slot, 1, (ID3D11ShaderResourceView* const*) &pSRV );
                break;

                case PipelineStage::Hull:
                m_pDeviceContext->HSSetShaderResources( slot, 1, (ID3D11ShaderResourceView* const*) &pSRV );
                break;

                case PipelineStage::Compute:
                m_pDeviceContext->CSSetShaderResources( slot, 1, (ID3D11ShaderResourceView* const*) &pSRV );
                break;

                case PipelineStage::None:
                KRG_HALT();
                break;
            }
        }

        void RenderContext::ClearShaderResource( PipelineStage stage, uint32 slot ) const
        {
            KRG_ASSERT( IsValid() );

            ID3D11ShaderResourceView* noSRV[] = { nullptr };

            switch ( stage )
            {
                case PipelineStage::Vertex:
                m_pDeviceContext->VSSetShaderResources( slot, 1, noSRV );
                break;

                case PipelineStage::Geometry:
                m_pDeviceContext->GSSetShaderResources( slot, 1, noSRV );
                break;

                case PipelineStage::Pixel:
                m_pDeviceContext->PSSetShaderResources( slot, 1, noSRV );
                break;

                case PipelineStage::Hull:
                m_pDeviceContext->HSSetShaderResources( slot, 1, noSRV );
                break;

                case PipelineStage::Compute:
                m_pDeviceContext->CSSetShaderResources( slot, 1, noSRV );
                break;

                case PipelineStage::None:
                KRG_HALT();
                break;
            }
        }

        void RenderContext::SetUnorderedAccess( PipelineStage stage, uint32 slot, ViewUAVHandle const& unorderedAccessView ) const
        {
            KRG_ASSERT( IsValid() && unorderedAccessView.IsValid() );

            auto pUAV = (ID3D11UnorderedAccessView*) unorderedAccessView.m_pData0;

            switch ( stage )
            {
            case PipelineStage::Compute:
                m_pDeviceContext->CSSetUnorderedAccessViews( slot, 1, (ID3D11UnorderedAccessView* const*) &pUAV, nullptr );
                break;

            default:
                KRG_HALT();
                break;
            }
        }

        void RenderContext::ClearUnorderedAccess( PipelineStage stage, uint32 slot ) const
        {
            KRG_ASSERT( IsValid() );

            ID3D11UnorderedAccessView* noUAV[] = { nullptr };

            switch ( stage )
            {
            case PipelineStage::Compute:
                m_pDeviceContext->CSSetUnorderedAccessViews( slot, 1, noUAV, nullptr );
                break;

            default:
                KRG_HALT();
                break;
            }
        }

        void RenderContext::SetSampler( PipelineStage stage, uint32 slot, SamplerState const& state ) const
        {
            KRG_ASSERT( IsValid() && state.IsValid() );

            switch ( stage )
            {
                case PipelineStage::None:
                KRG_HALT();
                break;

                case PipelineStage::Vertex:
                m_pDeviceContext->VSSetSamplers( slot, 1, (ID3D11SamplerState* const*) &state.GetResourceHandle().m_pData0 );
                break;

                case PipelineStage::Geometry:
                m_pDeviceContext->GSSetSamplers( slot, 1, (ID3D11SamplerState* const*) &state.GetResourceHandle().m_pData0 );
                break;

                case PipelineStage::Pixel:
                m_pDeviceContext->PSSetSamplers( slot, 1, (ID3D11SamplerState* const*) &state.GetResourceHandle().m_pData0 );
                break;

                case PipelineStage::Hull:
                m_pDeviceContext->HSSetSamplers( slot, 1, (ID3D11SamplerState* const*) &state.GetResourceHandle().m_pData0 );
                break;

                case PipelineStage::Compute:
                m_pDeviceContext->CSSetSamplers( slot, 1, (ID3D11SamplerState* const*) &state.GetResourceHandle().m_pData0 );
                break;
            }
        }

        //-------------------------------------------------------------------------

        void* RenderContext::MapBuffer( RenderBuffer const& buffer ) const
        {
            // Check that we have write access to this buffer and that the data to write is within the buffer size
            KRG_ASSERT( IsValid() );
            KRG_ASSERT( buffer.IsValid() );
            KRG_ASSERT( buffer.m_usage == RenderBuffer::Usage::CPU_and_GPU ); // Buffer does not allow CPU access

            D3D11_MAPPED_SUBRESOURCE mappedData;
            KRG::Memory::MemsetZero( &mappedData, sizeof( D3D11_MAPPED_SUBRESOURCE ) );

            auto result = m_pDeviceContext->Map( (ID3D11Buffer*) buffer.GetResourceHandle().m_pData0, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData );
            KRG_ASSERT( SUCCEEDED( result ) );
            return (Byte*) mappedData.pData;
        }

        void RenderContext::UnmapBuffer( RenderBuffer const& buffer ) const
        {
            // Check that we have write access to this buffer and that the data to write is within the buffer size
            KRG_ASSERT( IsValid() && buffer.IsValid() && buffer.m_usage == RenderBuffer::Usage::CPU_and_GPU );
            m_pDeviceContext->Unmap( (ID3D11Buffer*) buffer.GetResourceHandle().m_pData0, 0 );
        }

        void RenderContext::WriteToBuffer( RenderBuffer const& buffer, void const* pData, size_t const dataSize ) const
        {
            KRG_ASSERT( pData != nullptr && buffer.m_byteSize >= dataSize );
            void* pMappedData = MapBuffer( buffer );
            memcpy( pMappedData, pData, dataSize );
            UnmapBuffer( buffer );
        }

        void RenderContext::SetVertexBuffer( RenderBuffer const& buffer, uint32 offset ) const
        {
            KRG_ASSERT( IsValid() && buffer.IsValid() && buffer.m_type == RenderBuffer::Type::Vertex );
            m_pDeviceContext->IASetVertexBuffers( 0, 1, (ID3D11Buffer**) &buffer.GetResourceHandle().m_pData0, &buffer.m_byteStride, &offset );
        }

        void RenderContext::SetIndexBuffer( RenderBuffer const& buffer, uint32 offset ) const
        {
            KRG_ASSERT( IsValid() && buffer.IsValid() && buffer.m_type == RenderBuffer::Type::Index );
            m_pDeviceContext->IASetIndexBuffer( (ID3D11Buffer*) buffer.GetResourceHandle().m_pData0, buffer.m_byteStride == 4 ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT, offset );
        }

        //-------------------------------------------------------------------------

        void RenderContext::SetViewport( Float2 dimensions, Float2 topLeft, Float2 rangeZ ) const
        {
            D3D11_VIEWPORT dxViewport;
            dxViewport.Width = dimensions.m_x;
            dxViewport.Height = dimensions.m_y;
            dxViewport.TopLeftX = topLeft.m_x;
            dxViewport.TopLeftY = topLeft.m_y;
            dxViewport.MinDepth = rangeZ.m_x;
            dxViewport.MaxDepth = rangeZ.m_y;
            m_pDeviceContext->RSSetViewports( 1, &dxViewport );
        }

        void RenderContext::SetDepthTestMode( DepthTestMode mode ) const
        {
            switch ( mode )
            {
                case DepthTestMode::On:
                m_pDeviceContext->OMSetDepthStencilState( s_pDepthTestingOn, 0 );
                break;

                case DepthTestMode::Off:
                m_pDeviceContext->OMSetDepthStencilState( s_pDepthTestingOff, 0 );
                break;

                case DepthTestMode::ReadOnly:
                m_pDeviceContext->OMSetDepthStencilState( s_pDepthTestingReadOnly, 0 );
                break;
            }
        }

        void RenderContext::SetRasterizerScissorRectangles( ScissorRect const* pScissorRects, uint32 numRects ) const
        {
            KRG_ASSERT( IsValid() );

            if ( numRects != 0 )
            {
                KRG_ASSERT( pScissorRects != nullptr );
                m_pDeviceContext->RSSetScissorRects( numRects, (D3D11_RECT*) pScissorRects );
            }
            else
            {
                m_pDeviceContext->RSSetScissorRects( 0, nullptr );
            }
        }

        void RenderContext::SetBlendState( BlendState const& blendState ) const
        {
            KRG_ASSERT( IsValid() );

            float const blendFactors[4] = { 0.f, 0.f, 0.f, 0.f };

            if ( blendState.IsValid() )
            {
                m_pDeviceContext->OMSetBlendState( (ID3D11BlendState*) blendState.GetResourceHandle().m_pData0, blendFactors, 0xffffffff );
            }
            else
            {
                m_pDeviceContext->OMSetBlendState( nullptr, blendFactors, 0xffffffff );
            }
        }

        void RenderContext::SetRenderTarget( RenderTarget const& renderTarget, bool clearOnSet ) const
        {
            KRG_ASSERT( IsValid() && renderTarget.IsValid() );

            ViewRTHandle const& resourceHandle = renderTarget.GetRenderTargetHandle();
            m_pDeviceContext->OMSetRenderTargets( 1, (ID3D11RenderTargetView**) &resourceHandle.m_pData0, (ID3D11DepthStencilView*) resourceHandle.m_pData1 );

            if ( clearOnSet )
            {
                ClearRenderTargetViews( renderTarget );
            }
        }

        void RenderContext::SetRenderTargetViews( ViewRTHandle const& rtView, ViewDSHandle const& dsView ) const
        {
            KRG_ASSERT( IsValid() );
            m_pDeviceContext->OMSetRenderTargets( rtView.IsValid() ? 1 : 0, rtView.IsValid() ? (ID3D11RenderTargetView**) &rtView.m_pData0 : nullptr, dsView.IsValid() ? (ID3D11DepthStencilView*) dsView.m_pData0 : 0);
        }

        void RenderContext::ClearRenderTargetView( ViewRTHandle const& rtView, Float4 clearColor ) const
        {
            KRG_ASSERT( IsValid() && rtView.IsValid() );

            m_pDeviceContext->ClearRenderTargetView( (ID3D11RenderTargetView*) rtView.m_pData0, &clearColor.m_x );
        }

        void RenderContext::ClearDepthStencilView( ViewDSHandle const& dsView, float depth, uint8 stencil ) const
        {
            KRG_ASSERT( IsValid() && dsView.IsValid() );

            m_pDeviceContext->ClearDepthStencilView( (ID3D11DepthStencilView*) dsView.m_pData0, D3D10_CLEAR_DEPTH, depth, stencil );
        }

        void RenderContext::ClearRenderTargetViews( RenderTarget const& renderTarget ) const
        {
            KRG_ASSERT( IsValid() && renderTarget.IsValid() );

            static Float4 const clearColor = Color( 96, 96, 96 ).ToFloat4();

            ViewRTHandle const& resourceHandle = renderTarget.GetRenderTargetHandle();
            m_pDeviceContext->ClearRenderTargetView( (ID3D11RenderTargetView*) resourceHandle.m_pData0, &clearColor.m_x );
            m_pDeviceContext->ClearDepthStencilView( (ID3D11DepthStencilView*) resourceHandle.m_pData1, D3D10_CLEAR_DEPTH, 1.0f, 0 );
        }

        void RenderContext::ClearRenderTargets() const
        {
            ID3D11RenderTargetView* nullViews[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = { 0 };
            m_pDeviceContext->OMSetRenderTargets( D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, nullViews, nullptr );
        }

        //-------------------------------------------------------------------------

        void RenderContext::SetPrimitiveTopology( Topology topology ) const
        {
            KRG_ASSERT( IsValid() );
            D3D11_PRIMITIVE_TOPOLOGY d3dTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;

            switch ( topology )
            {
                case Topology::PointList:
                d3dTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
                break;

                case Topology::LineList:
                d3dTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
                break;

                case Topology::LineStrip:
                d3dTopology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
                break;

                case Topology::TriangleList:
                d3dTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
                break;

                case Topology::TriangleStrip:
                d3dTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
                break;
            }

            m_pDeviceContext->IASetPrimitiveTopology( d3dTopology );
        }

        void RenderContext::Draw( uint32 vertexCount, uint32 vertexStartIndex ) const
        {
            KRG_ASSERT( IsValid() );
            m_pDeviceContext->Draw( vertexCount, vertexStartIndex );
        }

        void RenderContext::DrawIndexed( uint32 vertexCount, uint32 indexStartIndex, uint32 vertexStartIndex ) const
        {
            KRG_ASSERT( IsValid() );
            m_pDeviceContext->DrawIndexed( vertexCount, indexStartIndex, vertexStartIndex );
        }

        void RenderContext::Dispatch( uint32 numGroupsX, uint32 numGroupsY, uint32 numGroupsZ ) const
        {
            KRG_ASSERT( IsValid() );
            m_pDeviceContext->Dispatch( numGroupsX, numGroupsY, numGroupsZ );
        }

        void RenderContext::Present( RenderWindow& window ) const
        { 
            auto pSwapChain = reinterpret_cast<IDXGISwapChain*>( window.m_pSwapChain );
            pSwapChain->Present( 0, 0 );
        }
    }
}