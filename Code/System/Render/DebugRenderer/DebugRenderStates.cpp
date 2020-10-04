#include "DebugRenderStates.h"
#include "System/Render/DebugRenderer/Shaders/DebugShaders.h"
#include "System/Render/ThirdParty/stb/stb_truetype.h"
#include "System/Render/Fonts/FontData_ProggyTiny.h"
#include "System/Render/Fonts/FontData_PixelCarnage.h"
#include "System/Render/Fonts/FontDecompressor.h"

//-------------------------------------------------------------------------

#if KRG_DEBUG_INSTRUMENTATION
namespace KRG
{
    namespace Render
    {
        bool DebugPointRenderState::Initialize( RenderDevice* pRenderDevice )
        {
            KRG_ASSERT( pRenderDevice != nullptr );

            TVector<RenderBuffer> cbuffers;

            //-------------------------------------------------------------------------
            // VERTEX SHADER
            //-------------------------------------------------------------------------

            // World transform const buffer
            RenderBuffer buffer;
            buffer.m_byteSize = 16 * 5; // float4x4 + float2
            buffer.m_byteStride = 16;   // Vector4 aligned
            buffer.m_usage = RenderBuffer::Usage::CPU_and_GPU;
            buffer.m_type = RenderBuffer::Type::Constant;
            cbuffers.push_back( buffer );

            // VS
            VertexLayoutDescriptor vertexLayoutDesc;
            vertexLayoutDesc.m_elementDescriptors.push_back( VertexLayoutDescriptor::ElementDescriptor( DataSemantic::Position, DataTypeFormat::Float_R32G32B32A32, 0, 0 ) );
            vertexLayoutDesc.m_elementDescriptors.push_back( VertexLayoutDescriptor::ElementDescriptor( DataSemantic::Color, DataTypeFormat::Float_R32G32B32A32, 0, 16 ) );
            vertexLayoutDesc.CalculateByteSize();

            m_vertexShader = VertexShader( g_byteCode_VS_DebugRendererPoints, sizeof( g_byteCode_VS_DebugRendererPoints ), cbuffers, vertexLayoutDesc );
            pRenderDevice->CreateShader( m_vertexShader );
            cbuffers.clear();

            // Create VB and input binding
            m_vertexBuffer.m_byteStride = sizeof( Float4 ) * 2;
            m_vertexBuffer.m_byteSize = m_vertexBuffer.m_byteStride * MaxPointsPerDrawCall;
            m_vertexBuffer.m_type = RenderBuffer::Type::Vertex;
            m_vertexBuffer.m_usage = RenderBuffer::Usage::CPU_and_GPU;
            pRenderDevice->CreateBuffer( m_vertexBuffer, nullptr );

            if ( !m_vertexBuffer.IsValid() )
            {
                return false;
            }

            // Create vertex layout and input binding
            pRenderDevice->CreateShaderInputBinding( m_vertexShader, vertexLayoutDesc, m_inputBinding );

            // Allocate staging memory for vertex data
            m_stagingVertexData.resize( m_vertexBuffer.m_byteSize );

            //-------------------------------------------------------------------------
            // GEOMETRY SHADER
            //-------------------------------------------------------------------------

            // View dimensions
            buffer.m_byteSize = 16 * 5; // float4x4 + float2
            buffer.m_byteStride = 16;   // Vector4 aligned
            buffer.m_usage = RenderBuffer::Usage::CPU_and_GPU;
            buffer.m_type = RenderBuffer::Type::Constant;
            cbuffers.push_back( buffer );

            // GS
            m_geometryShader = GeometryShader( g_byteCode_GS_DebugRendererPoints, sizeof( g_byteCode_GS_DebugRendererPoints ), cbuffers );
            pRenderDevice->CreateShader( m_geometryShader );
            cbuffers.clear();

            //-------------------------------------------------------------------------
            // PIXEL SHADER
            //-------------------------------------------------------------------------

            // PS
            m_pixelShader = PixelShader( g_byteCode_PS_DebugRendererPoints, sizeof( g_byteCode_PS_DebugRendererPoints ), cbuffers );
            pRenderDevice->CreateShader( m_pixelShader );
            cbuffers.clear();

            //-------------------------------------------------------------------------
            // RASTERIZER
            //-------------------------------------------------------------------------

            // Blend state for transparency
            {
                m_blendState.m_srcValue = BlendValue::SourceAlpha;
                m_blendState.m_dstValue = BlendValue::InverseSourceAlpha;
                m_blendState.m_blendOp = BlendOp::Add;
                m_blendState.m_srcAlphaValue = BlendValue::Zero;
                m_blendState.m_dstAlphaValue = BlendValue::Zero;
                m_blendState.m_blendOpAlpha = BlendOp::Add;
                m_blendState.m_blendEnable = true;

                pRenderDevice->CreateBlendState( m_blendState );

                if ( !m_blendState.IsValid() )
                {
                    return false;
                }
            }

            // Set Rasterizer state for drawing
            {
                m_rasterizerState.m_cullMode = CullMode::None;
                m_rasterizerState.m_windingMode = WindingMode::CounterClockwise;
                m_rasterizerState.m_fillMode = FillMode::Solid;

                pRenderDevice->CreateRasterizerState( m_rasterizerState );

                if ( !m_rasterizerState.IsValid() )
                {
                    return false;
                }
            }

            // Set up PSO
            m_PSO.m_pVertexShader = &m_vertexShader;
            m_PSO.m_pGeometryShader = &m_geometryShader;
            m_PSO.m_pPixelShader = &m_pixelShader;
            m_PSO.m_pBlendState = &m_blendState;
            m_PSO.m_pRasterizerState = &m_rasterizerState;

            return true;
        }

        void DebugPointRenderState::Shutdown( RenderDevice* pRenderDevice )
        {
            KRG_ASSERT( pRenderDevice != nullptr );

            m_PSO.Clear();

            if ( m_blendState.IsValid() )
            {
                pRenderDevice->DestroyBlendState( m_blendState );
            }

            if ( m_rasterizerState.IsValid() )
            {
                pRenderDevice->DestroyRasterizerState( m_rasterizerState );
            }

            if ( m_vertexBuffer.IsValid() )
            {
                pRenderDevice->DestroyBuffer( m_vertexBuffer );
            }

            pRenderDevice->DestroyShaderInputBinding( m_inputBinding );
            pRenderDevice->DestroyShader( m_vertexShader );
            pRenderDevice->DestroyShader( m_geometryShader );
            pRenderDevice->DestroyShader( m_pixelShader );
        }

        void DebugPointRenderState::SetState( RenderContext const& renderContext, Math::Viewport const& viewport )
        {
            renderContext.SetPipelineState( m_PSO );
            renderContext.SetVertexBuffer( m_vertexBuffer );
            renderContext.SetShaderInputBinding( m_inputBinding );

            struct CB
            {
                Matrix m_viewProjectionMatrix;
                Float4 m_viewport;
            };

            CB cb;
            cb.m_viewProjectionMatrix = viewport.GetViewVolume().GetViewProjectionMatrix();
            cb.m_viewport.x = ( F32 ) viewport.GetSize().x;
            cb.m_viewport.y = ( F32 ) viewport.GetSize().y;

            renderContext.WriteToBuffer( m_vertexShader.GetConstBuffer( 0 ), &cb, sizeof( CB ) );
            renderContext.WriteToBuffer( m_geometryShader.GetConstBuffer( 0 ), &cb, sizeof( CB ) );
        }

        //-------------------------------------------------------------------------

        bool DebugLineRenderState::Initialize( RenderDevice* pRenderDevice )
        {
            KRG_ASSERT( pRenderDevice != nullptr );

            TVector<RenderBuffer> cbuffers;

            //-------------------------------------------------------------------------
            // VERTEX SHADER
            //-------------------------------------------------------------------------

            // World transform const buffer
            RenderBuffer buffer;
            buffer.m_byteSize = 16 * 5; // float4x4 + float2
            buffer.m_byteStride = 16;   // Vector4 aligned
            buffer.m_usage = RenderBuffer::Usage::CPU_and_GPU;
            buffer.m_type = RenderBuffer::Type::Constant;
            cbuffers.push_back( buffer );

            // VS
            VertexLayoutDescriptor vertexLayoutDesc;
            vertexLayoutDesc.m_elementDescriptors.push_back( VertexLayoutDescriptor::ElementDescriptor( DataSemantic::Position, DataTypeFormat::Float_R32G32B32A32, 0, 0 ) );
            vertexLayoutDesc.m_elementDescriptors.push_back( VertexLayoutDescriptor::ElementDescriptor( DataSemantic::Color, DataTypeFormat::Float_R32G32B32A32, 0, 16 ) );
            vertexLayoutDesc.CalculateByteSize();

            m_vertexShader = VertexShader( g_byteCode_VS_DebugRendererLines, sizeof( g_byteCode_VS_DebugRendererLines ), cbuffers, vertexLayoutDesc );
            pRenderDevice->CreateShader( m_vertexShader );
            cbuffers.clear();

            // Create VB and input binding
            m_vertexBuffer.m_byteStride = sizeof( Float4 ) * 2;
            m_vertexBuffer.m_byteSize = m_vertexBuffer.m_byteStride * 2 * MaxLinesPerDrawCall;
            m_vertexBuffer.m_type = RenderBuffer::Type::Vertex;
            m_vertexBuffer.m_usage = RenderBuffer::Usage::CPU_and_GPU;
            pRenderDevice->CreateBuffer( m_vertexBuffer, nullptr );

            if ( !m_vertexBuffer.IsValid() )
            {
                return false;
            }

            // Create vertex layout and input binding
            pRenderDevice->CreateShaderInputBinding( m_vertexShader, vertexLayoutDesc, m_inputBinding );

            // Allocate staging memory for vertex data
            m_stagingVertexData.resize( m_vertexBuffer.m_byteSize );

            //-------------------------------------------------------------------------
            // GEOMETRY SHADER
            //-------------------------------------------------------------------------

            // View dimensions
            buffer.m_byteSize = 16 * 5; // float4x4 + float2
            buffer.m_byteStride = 16;   // Vector4 aligned
            buffer.m_usage = RenderBuffer::Usage::CPU_and_GPU;
            buffer.m_type = RenderBuffer::Type::Constant;
            cbuffers.push_back( buffer );

            // GS
            m_geometryShader = GeometryShader( g_byteCode_GS_DebugRendererLines, sizeof( g_byteCode_GS_DebugRendererLines ), cbuffers );
            pRenderDevice->CreateShader( m_geometryShader );
            cbuffers.clear();

            //-------------------------------------------------------------------------
            // PIXEL SHADER
            //-------------------------------------------------------------------------

            // Selection ID buffer
            buffer.m_byteSize = 16;
            buffer.m_byteStride = 16;
            buffer.m_usage = RenderBuffer::Usage::CPU_and_GPU;
            buffer.m_type = RenderBuffer::Type::Constant;
            cbuffers.push_back( buffer );

            // PS
            m_pixelShader = PixelShader( g_byteCode_PS_DebugRendererLines, sizeof( g_byteCode_PS_DebugRendererLines ), cbuffers );
            pRenderDevice->CreateShader( m_pixelShader );
            cbuffers.clear();

            //-------------------------------------------------------------------------
            // RASTERIZER
            //-------------------------------------------------------------------------

            // Blend state for transparency
            {
                m_blendState.m_srcValue = BlendValue::SourceAlpha;
                m_blendState.m_dstValue = BlendValue::InverseSourceAlpha;
                m_blendState.m_blendOp = BlendOp::Add;
                m_blendState.m_srcAlphaValue = BlendValue::Zero;
                m_blendState.m_dstAlphaValue = BlendValue::Zero;
                m_blendState.m_blendOpAlpha = BlendOp::Add;
                m_blendState.m_blendEnable = true;

                pRenderDevice->CreateBlendState( m_blendState );

                if ( !m_blendState.IsValid() )
                {
                    return false;
                }
            }

            // Set Rasterizer state for drawing
            {
                m_rasterizerState.m_cullMode = CullMode::None;
                m_rasterizerState.m_windingMode = WindingMode::CounterClockwise;
                m_rasterizerState.m_fillMode = FillMode::Solid;

                pRenderDevice->CreateRasterizerState( m_rasterizerState );

                if ( !m_rasterizerState.IsValid() )
                {
                    return false;
                }
            }

            // Set up PSO
            m_PSO.m_pVertexShader = &m_vertexShader;
            m_PSO.m_pGeometryShader = &m_geometryShader;
            m_PSO.m_pPixelShader = &m_pixelShader;
            m_PSO.m_pBlendState = &m_blendState;
            m_PSO.m_pRasterizerState = &m_rasterizerState;

            return true;
        }

        void DebugLineRenderState::Shutdown( RenderDevice* pRenderDevice )
        {
            KRG_ASSERT( pRenderDevice != nullptr );

            m_PSO.Clear();

            if ( m_blendState.IsValid() )
            {
                pRenderDevice->DestroyBlendState( m_blendState );
            }

            if ( m_rasterizerState.IsValid() )
            {
                pRenderDevice->DestroyRasterizerState( m_rasterizerState );
            }

            if ( m_vertexBuffer.IsValid() )
            {
                pRenderDevice->DestroyBuffer( m_vertexBuffer );
            }

            pRenderDevice->DestroyShaderInputBinding( m_inputBinding );
            pRenderDevice->DestroyShader( m_vertexShader );
            pRenderDevice->DestroyShader( m_geometryShader );
            pRenderDevice->DestroyShader( m_pixelShader );
        }

        void DebugLineRenderState::SetState( RenderContext const& renderContext, Math::Viewport const& viewport )
        {
            renderContext.SetPipelineState( m_PSO );
            renderContext.SetVertexBuffer( m_vertexBuffer );
            renderContext.SetShaderInputBinding( m_inputBinding );

            struct CB
            {
                Matrix m_viewProjectionMatrix;
                Float4 m_viewport;
            };

            CB cb;
            cb.m_viewProjectionMatrix = viewport.GetViewVolume().GetViewProjectionMatrix();
            cb.m_viewport.x = ( F32 ) viewport.GetSize().x;
            cb.m_viewport.y = ( F32 ) viewport.GetSize().y;

            renderContext.WriteToBuffer( m_vertexShader.GetConstBuffer( 0 ), &cb, sizeof( CB ) );
            renderContext.WriteToBuffer( m_geometryShader.GetConstBuffer( 0 ), &cb, sizeof( CB ) );
        }

        //-------------------------------------------------------------------------

        bool DebugPrimitiveRenderState::Initialize( RenderDevice* pRenderDevice )
        {
            KRG_ASSERT( pRenderDevice != nullptr );

            TVector<RenderBuffer> cbuffers;

            // Global const buffer
            RenderBuffer buffer;
            buffer.m_byteSize = 16 * 5; // float4x4 + float2
            buffer.m_byteStride = 16;   // Vector4 aligned
            buffer.m_usage = RenderBuffer::Usage::CPU_and_GPU;
            buffer.m_type = RenderBuffer::Type::Constant;
            cbuffers.push_back( buffer );

            // VS
            VertexLayoutDescriptor vertexLayoutDesc;
            vertexLayoutDesc.m_elementDescriptors.push_back( VertexLayoutDescriptor::ElementDescriptor( DataSemantic::Position, DataTypeFormat::Float_R32G32B32A32, 0, 0 ) );
            vertexLayoutDesc.m_elementDescriptors.push_back( VertexLayoutDescriptor::ElementDescriptor( DataSemantic::Color, DataTypeFormat::Float_R32G32B32A32, 0, 16 ) );
            vertexLayoutDesc.CalculateByteSize();

            m_vertexShader = VertexShader( g_byteCode_VS_DebugRendererTriangles, sizeof( g_byteCode_VS_DebugRendererTriangles ), cbuffers, vertexLayoutDesc );
            pRenderDevice->CreateShader( m_vertexShader );
            cbuffers.clear();

            // Create vertex buffer
            m_vertexBuffer.m_byteStride = sizeof( Float4 ) * 2;
            m_vertexBuffer.m_byteSize = m_vertexBuffer.m_byteStride * 3 * MaxTrianglesPerDrawCall;
            m_vertexBuffer.m_type = RenderBuffer::Type::Vertex;
            m_vertexBuffer.m_usage = RenderBuffer::Usage::CPU_and_GPU;
            pRenderDevice->CreateBuffer( m_vertexBuffer, nullptr );

            if ( !m_vertexBuffer.IsValid() )
            {
                return false;
            }

            // Create vertex layout and input binding
            pRenderDevice->CreateShaderInputBinding( m_vertexShader, vertexLayoutDesc, m_inputBinding );

            // PS
            m_pixelShader = PixelShader( g_byteCode_PS_DebugRendererTriangles, sizeof( g_byteCode_PS_DebugRendererTriangles ), cbuffers );
            pRenderDevice->CreateShader( m_pixelShader );
            cbuffers.clear();

            // Allocate staging memory for vertex data
            m_stagingVertexData.resize( m_vertexBuffer.m_byteSize );

            // Blend state for transparency
            {
                m_blendState.m_srcValue = BlendValue::SourceAlpha;
                m_blendState.m_dstValue = BlendValue::InverseSourceAlpha;
                m_blendState.m_blendOp = BlendOp::Add;
                m_blendState.m_srcAlphaValue = BlendValue::Zero;
                m_blendState.m_dstAlphaValue = BlendValue::Zero;
                m_blendState.m_blendOpAlpha = BlendOp::Add;
                m_blendState.m_blendEnable = true;

                pRenderDevice->CreateBlendState( m_blendState );

                if ( !m_blendState.IsValid() )
                {
                    return false;
                }
            }

            // Set Rasterizer state for drawing
            {
                m_rasterizerState.m_cullMode = CullMode::None;
                m_rasterizerState.m_windingMode = WindingMode::CounterClockwise;
                m_rasterizerState.m_fillMode = FillMode::Solid;

                pRenderDevice->CreateRasterizerState( m_rasterizerState );

                if ( !m_rasterizerState.IsValid() )
                {
                    return false;
                }
            }

            // Set up PSO
            m_PSO.m_pVertexShader = &m_vertexShader;
            m_PSO.m_pPixelShader = &m_pixelShader;
            m_PSO.m_pBlendState = &m_blendState;
            m_PSO.m_pRasterizerState = &m_rasterizerState;

            return true;
        }

        void DebugPrimitiveRenderState::Shutdown( RenderDevice* pRenderDevice )
        {
            KRG_ASSERT( pRenderDevice != nullptr );

            m_PSO.Clear();

            if ( m_blendState.IsValid() )
            {
                pRenderDevice->DestroyBlendState( m_blendState );
            }

            if ( m_rasterizerState.IsValid() )
            {
                pRenderDevice->DestroyRasterizerState( m_rasterizerState );
            }

            if ( m_vertexBuffer.IsValid() )
            {
                pRenderDevice->DestroyBuffer( m_vertexBuffer );
            }

            pRenderDevice->DestroyShaderInputBinding( m_inputBinding );
            pRenderDevice->DestroyShader( m_vertexShader );
            pRenderDevice->DestroyShader( m_pixelShader );
        }

        void DebugPrimitiveRenderState::SetState( RenderContext const& renderContext, Math::Viewport const& viewport )
        {
            renderContext.SetPipelineState( m_PSO );

            renderContext.SetVertexBuffer( m_vertexBuffer );
            renderContext.SetShaderInputBinding( m_inputBinding );

            struct CB
            {
                Matrix m_viewProjectionMatrix;
                Float4 m_viewport;
            };

            CB cb;
            cb.m_viewProjectionMatrix = viewport.GetViewVolume().GetViewProjectionMatrix();
            cb.m_viewport.x = ( F32) viewport.GetSize().x;
            cb.m_viewport.y = ( F32) viewport.GetSize().y;
            renderContext.WriteToBuffer( m_vertexShader.GetConstBuffer( 0 ), &cb, sizeof( CB ) );
        }

        //-------------------------------------------------------------------------

        Float4 const DebugTextRenderState::ClipSpaceTopLeft( -1, 1, 0, 0 );

        //-------------------------------------------------------------------------

        bool DebugTextRenderState::Initialize( RenderDevice* pRenderDevice )
        {
            TVector<RenderBuffer> cbuffers;

            // Viewport info buffer
            RenderBuffer buffer;
            buffer.m_byteSize = sizeof( Float4 );
            buffer.m_byteStride = buffer.m_byteSize;
            buffer.m_usage = RenderBuffer::Usage::CPU_and_GPU;
            buffer.m_type = RenderBuffer::Type::Constant;
            cbuffers.push_back( buffer );

            // Text VS
            VertexLayoutDescriptor vertexLayoutDesc;
            vertexLayoutDesc.m_elementDescriptors.push_back( VertexLayoutDescriptor::ElementDescriptor( DataSemantic::Position, DataTypeFormat::Float_R32G32, 0, 0 ) );
            vertexLayoutDesc.m_elementDescriptors.push_back( VertexLayoutDescriptor::ElementDescriptor( DataSemantic::TexCoord, DataTypeFormat::Float_R32G32, 0, 8 ) );
            vertexLayoutDesc.m_elementDescriptors.push_back( VertexLayoutDescriptor::ElementDescriptor( DataSemantic::Color, DataTypeFormat::UNorm_R8G8B8A8, 0, 16 ) );
            vertexLayoutDesc.CalculateByteSize();

            m_vertexShader = VertexShader( g_byteCode_VS_DebugRendererText, sizeof( g_byteCode_VS_DebugRendererText ), cbuffers, vertexLayoutDesc );
            pRenderDevice->CreateShader( m_vertexShader );
            cbuffers.clear();

            // Create text VB and input binding
            m_vertexBuffer.m_byteStride = sizeof( DebugFontGlyphVertex );
            m_vertexBuffer.m_byteSize = m_vertexBuffer.m_byteStride * MaxGlyphsPerDrawCall * 4; // 4 verts per glyph
            m_vertexBuffer.m_type = RenderBuffer::Type::Vertex;
            m_vertexBuffer.m_usage = RenderBuffer::Usage::CPU_and_GPU;
            pRenderDevice->CreateBuffer( m_vertexBuffer, nullptr );

            if ( !m_vertexBuffer.IsValid() )
            {
                return false;
            }

            // Create vertex layout and input binding
            pRenderDevice->CreateShaderInputBinding( m_vertexShader, vertexLayoutDesc, m_inputBinding );

            // Create text IB
            m_indexBuffer.m_byteStride = sizeof( U16 );
            m_indexBuffer.m_byteSize = m_vertexBuffer.m_byteStride * MaxGlyphsPerDrawCall * 6; // 6 indices per glyph
            m_indexBuffer.m_type = RenderBuffer::Type::Index;
            m_indexBuffer.m_usage = RenderBuffer::Usage::CPU_and_GPU;
            pRenderDevice->CreateBuffer( m_indexBuffer, nullptr );

            if ( !m_indexBuffer.IsValid() )
            {
                return false;
            }

            // Text PS
            m_pixelShader = PixelShader( g_byteCode_PS_DebugRendererText, sizeof( g_byteCode_PS_DebugRendererText ), cbuffers );
            pRenderDevice->CreateShader( m_pixelShader );

            // Initialize sampler state
            pRenderDevice->CreateSamplerState( m_samplerState );

            // Blend state for transparency
            {
                m_blendState.m_srcValue = BlendValue::SourceAlpha;
                m_blendState.m_dstValue = BlendValue::InverseSourceAlpha;
                m_blendState.m_blendOp = BlendOp::Add;
                m_blendState.m_srcAlphaValue = BlendValue::Zero;
                m_blendState.m_dstAlphaValue = BlendValue::Zero;
                m_blendState.m_blendOpAlpha = BlendOp::Add;
                m_blendState.m_blendEnable = true;

                pRenderDevice->CreateBlendState( m_blendState );

                if ( !m_blendState.IsValid() )
                {
                    return false;
                }
            }

            // Set Rasterizer state for drawing
            {
                m_rasterizerState.m_cullMode = CullMode::None;
                m_rasterizerState.m_windingMode = WindingMode::CounterClockwise;
                m_rasterizerState.m_fillMode = FillMode::Solid;

                pRenderDevice->CreateRasterizerState( m_rasterizerState );

                if ( !m_rasterizerState.IsValid() )
                {
                    return false;
                }
            }

            // Initialize font atlas and create atlas texture
            //-------------------------------------------------------------------------

            DebugTextFontAtlas::FontDesc fonts[] =
            {
                { Fonts::ProggyTiny::GetFontCompressedData(), 10 },
                { Fonts::PixelCarnage::GetFontCompressedData(), 16 },
            };

            if ( !m_fontAtlas.Generate( fonts, 2 ) )
            {
                return false;
            }

            Int2 const fontAtlasDimensions = m_fontAtlas.GetDimensions();
            S32 const fontAtlasDimensionsSq = fontAtlasDimensions.x * fontAtlasDimensions.y;

            m_nonZeroAlphaTexCoords = Float2( 0, 0 );

            // Convert from 8 bit to 32 bit pixels
            TVector<U32> fontdata( fontAtlasDimensionsSq );
            Byte const* pSrc = m_fontAtlas.GetAtlasData();
            for ( int i = 0; i < fontAtlasDimensionsSq; i++ )
            {
                if ( m_nonZeroAlphaTexCoords == Float2::Zero && *pSrc == 0xFF )
                {
                    F32 const u = F32( i % fontAtlasDimensions.x );
                    F32 const v = F32( i / fontAtlasDimensions.x );
                    m_nonZeroAlphaTexCoords = Float2( u, v );
                }

                fontdata[i] = ( ( U32 ) ( *pSrc++ ) << 24 ) | 0x00FFFFFF;
            }

            m_fontAtlasTexture = Texture::InitializeTexture( TextureFormat::Raw, fontAtlasDimensions, ( Byte* ) fontdata.data(), fontAtlasDimensions.x * fontAtlasDimensions.y * 4 );
            pRenderDevice->CreateTexture( m_fontAtlasTexture );

            // Set up PSO
            m_PSO.m_pVertexShader = &m_vertexShader;
            m_PSO.m_pPixelShader = &m_pixelShader;
            m_PSO.m_pBlendState = &m_blendState;
            m_PSO.m_pRasterizerState = &m_rasterizerState;

            return true;
        }

        void DebugTextRenderState::Shutdown( RenderDevice* pRenderDevice )
        {
            KRG_ASSERT( pRenderDevice != nullptr );

            m_PSO.Clear();

            if ( m_blendState.IsValid() )
            {
                pRenderDevice->DestroyBlendState( m_blendState );
            }

            if ( m_rasterizerState.IsValid() )
            {
                pRenderDevice->DestroyRasterizerState( m_rasterizerState );
            }

            if ( m_vertexBuffer.IsValid() )
            {
                pRenderDevice->DestroyBuffer( m_vertexBuffer );
            }

            if ( m_indexBuffer.IsValid() )
            {
                pRenderDevice->DestroyBuffer( m_indexBuffer );
            }

            if ( m_fontAtlasTexture.IsValid() )
            {
                pRenderDevice->DestroyTexture( m_fontAtlasTexture );
            }

            if ( m_samplerState.IsValid() )
            {
                pRenderDevice->DestroySamplerState( m_samplerState );
            }

            pRenderDevice->DestroyShaderInputBinding( m_inputBinding );
            pRenderDevice->DestroyShader( m_vertexShader );
            pRenderDevice->DestroyShader( m_pixelShader );
        }

        void DebugTextRenderState::SetState( RenderContext const& renderContext, Math::Viewport const& viewport )
        {
            renderContext.SetPipelineState( m_PSO );

            renderContext.SetShaderInputBinding( m_inputBinding );
            renderContext.SetSampler( PipelineStage::Pixel, 0, m_samplerState );
            renderContext.SetTexture( PipelineStage::Pixel, 0, m_fontAtlasTexture );
            renderContext.SetPrimitiveTopology( Topology::TriangleList );
            renderContext.SetVertexBuffer( m_vertexBuffer );
            renderContext.SetIndexBuffer( m_indexBuffer );

            // Set viewport info
            Float4 const m_viewportInfo( viewport.GetSize() );
            renderContext.WriteToBuffer( m_vertexShader.GetConstBuffer( 0 ), &m_viewportInfo, sizeof( Float4 ) );
        }

        //-------------------------------------------------------------------------
        // Fonts and Glyphs
        //-------------------------------------------------------------------------

        bool DebugTextFontAtlas::Generate( FontDesc* pFonts, S32 const numFonts )
        {
            static TRange<S32> const glyphRange( 0x20, 0xFF );

            // Allocate temporary atlas data
            //-------------------------------------------------------------------------

            Int2 const fontAtlasDimensions = GetDimensions();
            S32 const fontAtlasDimensionsSq = fontAtlasDimensions.x * fontAtlasDimensions.y;
            m_atlasData.resize( fontAtlasDimensionsSq, 0 );

            // Render debug fonts to atlas
            //-------------------------------------------------------------------------

            stbtt_pack_context context;
            stbtt_PackSetOversampling( &context, 2, 2 );

            if ( !stbtt_PackBegin( &context, m_atlasData.data(), fontAtlasDimensions.x, fontAtlasDimensions.y, 0, 0, nullptr ) )
            {
                return false;
            }

            //-------------------------------------------------------------------------

            TVector<stbtt_packedchar> charInfo;
            TVector<Byte> fontData;

            for ( auto i = 0; i < numFonts; i++ )
            {
                // Decompress font data
                //-------------------------------------------------------------------------

                Fonts::GetDecompressedFontData( pFonts[i].m_compressedFontData, fontData );

                stbtt_fontinfo stbFontInfo;
                stbtt_InitFont( &stbFontInfo, fontData.data(), 0 );
                F32 const fontScale = stbtt_ScaleForPixelHeight( &stbFontInfo, 10 );

                S32 unscaledAscent, unscaledDescent, unscaledLineGap;
                stbtt_GetFontVMetrics( &stbFontInfo, &unscaledAscent, &unscaledDescent, &unscaledLineGap );

                // Create font info
                //-------------------------------------------------------------------------

                m_fonts.push_back( FontInfo() );
                FontInfo& fontInfo = m_fonts.back();
                fontInfo.m_ascent = unscaledAscent * fontScale;
                fontInfo.m_descent = -unscaledDescent * fontScale;
                fontInfo.m_lineGap = unscaledLineGap * fontScale;

                // Pack font into the atlas
                //-------------------------------------------------------------------------

                charInfo.resize( glyphRange.GetLength() );
                if ( !stbtt_PackFontRange( &context, fontData.data(), 0, pFonts[i].m_fontSize, glyphRange.m_min, glyphRange.GetLength(), charInfo.data() ) )
                {
                    return false;
                }

                // Create Glyph lookup table
                //-------------------------------------------------------------------------

                for ( auto g = glyphRange.m_min; g < glyphRange.m_max; g++ )
                {
                    S32 charIdx = g - glyphRange.m_min;
                    stbtt_aligned_quad q;
                    Float2 pos( 0.0f );
                    stbtt_GetPackedQuad( charInfo.data(), fontAtlasDimensions.x, fontAtlasDimensions.y, charIdx, &pos.x, &pos.y, &q, 0 );

                    // Glyphs positions are stored relative of their baseline positions (i.e. bottom left)
                    DebugFontGlyph glyph;
                    glyph.m_positionTL = Float2( q.x0, q.y0 );
                    glyph.m_positionBR = Float2( q.x1, q.y1 );
                    glyph.m_texCoordsTL = Float2( q.s0, q.t0 );
                    glyph.m_texCoordsBR = Float2( q.s1, q.t1 );
                    glyph.m_advanceX = pos.x + 0.5f;
                    fontInfo.m_glyphs.emplace_back( std::move( glyph ) );
                }
            }

            stbtt_PackEnd( &context );
            return true;
        }

        void DebugTextFontAtlas::GetGlyphsForString( U32 fontIdx, String const& str, TInlineVector<S32, 100>& outGlyphIndices ) const
        {
            KRG_ASSERT( fontIdx < m_fonts.size() );
            auto const& fontInfo = m_fonts[fontIdx];

            TRange<S32> const glyphRange = fontInfo.GetValidGlyphRange();
            S32 const spaceIdx = ' ' - glyphRange.m_min;

            outGlyphIndices.reserve( (S32) ( str.size() * 1.5f ) );

            for ( char ch : str )
            {
                if ( ch == '\t' )
                {
                    outGlyphIndices.push_back( spaceIdx );
                    outGlyphIndices.push_back( spaceIdx );
                    outGlyphIndices.push_back( spaceIdx );
                    outGlyphIndices.push_back( spaceIdx );
                }
                else if ( ch == '\n' )
                {
                    outGlyphIndices.push_back( '\n' );
                }
                else if ( glyphRange.IsInsideInclusive( ch ) )
                {
                    outGlyphIndices.push_back( ch - glyphRange.m_min );
                }
            }
        }

        void DebugTextFontAtlas::GetGlyphsForString( U32 fontIdx, char const* pStr, TInlineVector<S32, 100>& outGlyphIndices ) const
        {
            KRG_ASSERT( fontIdx < m_fonts.size() );
            auto const& fontInfo = m_fonts[fontIdx];

            TRange<S32> const glyphRange = fontInfo.GetValidGlyphRange();
            S32 const spaceIdx = ' ' - glyphRange.m_min;

            S32 const strLen = (S32) strlen( pStr );
            outGlyphIndices.reserve( (S32) ( strLen * 1.5f ) );

            for ( S32 i = 0; i < strLen; i++ )
            {
                if ( pStr[i] == '\t' )
                {
                    outGlyphIndices.push_back( spaceIdx );
                    outGlyphIndices.push_back( spaceIdx );
                    outGlyphIndices.push_back( spaceIdx );
                    outGlyphIndices.push_back( spaceIdx );
                }
                else if ( pStr[i] == '\n' )
                {
                    outGlyphIndices.push_back( '\n' );
                }
                else if ( glyphRange.IsInsideInclusive( pStr[i] ) )
                {
                    outGlyphIndices.push_back( pStr[i] - glyphRange.m_min );
                }
            }
        }

        Int2 DebugTextFontAtlas::GetTextExtents( U32 fontIdx, char const* pText ) const
        {
            KRG_ASSERT( pText != nullptr );

            KRG_ASSERT( fontIdx < m_fonts.size() );
            auto const& fontInfo = m_fonts[fontIdx];

            auto const glyphRange = fontInfo.GetValidGlyphRange();
            auto const spaceGlyphIdx = fontInfo.GetGlyphIndex( ' ' );
            auto const numChars = strlen( pText );

            F32 const lineHeight = fontInfo.m_ascent + fontInfo.m_descent + fontInfo.m_lineGap;
            F32 currentLineWidth = 0;
            F32 maxWidth = 0;
            U32 numLines = 1;

            for ( auto i = 0; i < numChars; i++ )
            {
                // Increment Y offset
                if ( pText[i] == '\n' )
                {
                    numLines++;
                    maxWidth = Math::Max( currentLineWidth, maxWidth );
                    currentLineWidth = 0;
                }
                else if ( pText[i] == '\t' )
                {
                    currentLineWidth += fontInfo.m_glyphs[spaceGlyphIdx].m_advanceX * 4;
                }
                else // Regular characters increment x offset
                {
                    if ( glyphRange.IsInsideInclusive( pText[i] ) )
                    {
                        auto const glyphIdx = pText[i] - glyphRange.m_min;
                        auto const& glyph = fontInfo.m_glyphs[glyphIdx];
                        currentLineWidth += glyph.m_advanceX;
                    }
                }
            }

            maxWidth = Math::Max( currentLineWidth, maxWidth );
            Int2 const extents( (S32) Math::Floor( maxWidth - 0.5f ) - 1, (S32) Math::Floor( ( lineHeight * numLines ) - 0.5f ) );
            return extents;
        }

        U32 DebugTextFontAtlas::WriteGlyphsToBuffer( DebugFontGlyphVertex* pVertexBuffer, U16 indexStartOffset, U16* pIndexBuffer, U32 fontIdx, TInlineVector<S32, 100> const& glyphIndices, Float2 const& textPosTopLeft, Float4 const& color ) const
        {
            KRG_ASSERT( fontIdx < m_fonts.size() );
            auto const& fontInfo = m_fonts[fontIdx];

            F32 const descent = fontInfo.GetDescent();
            F32 const lineHeight = fontInfo.GetAscent() + fontInfo.GetDescent() + fontInfo.GetLineGap();

            // Draw characters
            //-------------------------------------------------------------------------

            U32 numGlyphsDrawn = 0;
            Float2 textDrawPos = textPosTopLeft;
            textDrawPos.y += ( lineHeight - descent );

            //-------------------------------------------------------------------------

            size_t const numGlyphs = glyphIndices.size();
            for ( auto i = 0; i < numGlyphs; i++ )
            {
                if ( glyphIndices[i] == '\n' )
                {
                    textDrawPos.x = textPosTopLeft.x;
                    textDrawPos.y += lineHeight;
                }
                else
                {
                    DebugFontGlyph const& glyph = fontInfo.GetGlyph( glyphIndices[i] );

                    Float2 const bl( Math::Floor( textDrawPos.x + glyph.m_positionTL.x ), Math::Floor( textDrawPos.y + glyph.m_positionBR.y ) );
                    Float2 const tl( Math::Floor( textDrawPos.x + glyph.m_positionTL.x ), Math::Floor( textDrawPos.y + glyph.m_positionTL.y ) );
                    Float2 const tr( Math::Floor( textDrawPos.x + glyph.m_positionBR.x ), Math::Floor( textDrawPos.y + glyph.m_positionTL.y ) );
                    Float2 const br( Math::Floor( textDrawPos.x + glyph.m_positionBR.x ), Math::Floor( textDrawPos.y + glyph.m_positionBR.y ) );

                    U32 const vertexColor = ( U8( color[3] * 255 ) << 24 ) | ( U8( color[2] * 255 ) << 16 ) | ( U8( color[1] * 255 ) << 8 ) | U8( color[0] * 255 );

                    // 0 = BL, 1 = TL, 2 = TR, 3 = BR
                    pVertexBuffer[0] = DebugFontGlyphVertex{ bl, Float2( glyph.m_texCoordsTL.x, glyph.m_texCoordsBR.y ), vertexColor };
                    pVertexBuffer[1] = DebugFontGlyphVertex{ tl, Float2( glyph.m_texCoordsTL.x, glyph.m_texCoordsTL.y ), vertexColor };
                    pVertexBuffer[2] = DebugFontGlyphVertex{ tr, Float2( glyph.m_texCoordsBR.x, glyph.m_texCoordsTL.y ), vertexColor };
                    pVertexBuffer[3] = DebugFontGlyphVertex{ br, Float2( glyph.m_texCoordsBR.x, glyph.m_texCoordsBR.y ), vertexColor };

                    pIndexBuffer[0] = indexStartOffset + 3;
                    pIndexBuffer[1] = indexStartOffset + 1;
                    pIndexBuffer[2] = indexStartOffset + 0;
                    pIndexBuffer[3] = indexStartOffset + 3;
                    pIndexBuffer[4] = indexStartOffset + 2;
                    pIndexBuffer[5] = indexStartOffset + 1;

                    // Offset buffer ptrs
                    //-------------------------------------------------------------------------

                    pVertexBuffer += 4;
                    indexStartOffset += 4;
                    pIndexBuffer += 6;

                    // Update glyph draw state
                    //-------------------------------------------------------------------------

                    textDrawPos.x += glyph.m_advanceX;
                    numGlyphsDrawn++;
                }
            }

            return numGlyphsDrawn;
        }

        void DebugTextFontAtlas::WriteCustomGlyphToBuffer( DebugFontGlyphVertex* pVertexBuffer, U16 indexStartOffset, U16* pIndexBuffer, U32 fontIdx, S32 firstGlyphIdx, Float2 const& texCoords, Float2 const& textPosTopLeft, Int2 const& textExtents, S32 pixelPadding, Float4 const& color ) const
        {
            KRG_ASSERT( fontIdx < m_fonts.size() );
            auto const& fontInfo = m_fonts[fontIdx];

            F32 const descent = fontInfo.GetDescent();
            F32 const ascent = fontInfo.GetAscent();
            F32 const lineHeight = fontInfo.GetAscent() + fontInfo.GetDescent() + fontInfo.GetLineGap();

            Float2 textDrawPos = textPosTopLeft;
            textDrawPos.y += ( lineHeight - descent );

            DebugFontGlyph const& glyph = fontInfo.GetGlyph( firstGlyphIdx );

            Float2 const tl( Math::Floor( textDrawPos.x + glyph.m_positionTL.x - pixelPadding ), Math::Floor( textDrawPos.y + glyph.m_positionTL.y - pixelPadding ) );
            Float2 const tr( Math::Floor( textDrawPos.x + textExtents.x + pixelPadding ), Math::Floor( textDrawPos.y + glyph.m_positionTL.y - pixelPadding ) );
            Float2 const bl( Math::Floor( textDrawPos.x + glyph.m_positionTL.x - pixelPadding ), Math::Floor( textDrawPos.y - ascent + textExtents.y + pixelPadding ) );
            Float2 const br( Math::Floor( textDrawPos.x + textExtents.x + pixelPadding ), Math::Floor( textDrawPos.y - ascent + textExtents.y + pixelPadding ) );

            U32 const vertexColor = ( U8( color[3] * 255 ) << 24 ) | ( U8( color[2] * 255 ) << 16 ) | ( U8( color[1] * 255 ) << 8 ) | U8( color[0] * 255 );

            // 0 = BL, 1 = TL, 2 = TR, 3 = BR
            pVertexBuffer[0] = DebugFontGlyphVertex{ bl, texCoords, vertexColor };
            pVertexBuffer[1] = DebugFontGlyphVertex{ tl, texCoords, vertexColor };
            pVertexBuffer[2] = DebugFontGlyphVertex{ tr, texCoords, vertexColor };
            pVertexBuffer[3] = DebugFontGlyphVertex{ br, texCoords, vertexColor };

            pIndexBuffer[0] = indexStartOffset + 3;
            pIndexBuffer[1] = indexStartOffset + 1;
            pIndexBuffer[2] = indexStartOffset + 0;
            pIndexBuffer[3] = indexStartOffset + 3;
            pIndexBuffer[4] = indexStartOffset + 2;
            pIndexBuffer[5] = indexStartOffset + 1;
        }
    }
}
#endif