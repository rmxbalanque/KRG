#include "ImguiRenderer.h"
#include "Shaders/ImguiShaders.h"
#include "System/Core/Math/Viewport.h"
#include "System/Core/Threading/Threading.h"
#include "System/Core/Profiling/Profiling.h"

//-------------------------------------------------------------------------

#if KRG_DEBUG_INSTRUMENTATION
namespace KRG
{
    using namespace Render;

    namespace
    {
        static U32 const g_numInitialVertices = 2000;
        static U32 const g_numInitialIndices = 2000;
    }

    //-------------------------------------------------------------------------

    namespace ImGuiX
    {
        bool ImguiRenderer::Initialize( RenderDevice* pRenderDevice )
        {
            KRG_ASSERT( m_pRenderDevice == nullptr && pRenderDevice != nullptr );
            m_pRenderDevice = pRenderDevice;

            //-------------------------------------------------------------------------

            // Create vertex buffer for IMGUI
            m_vertexBuffer.m_byteStride = sizeof( ImDrawVert );
            m_vertexBuffer.m_byteSize = m_vertexBuffer.m_byteStride * g_numInitialVertices;
            m_vertexBuffer.m_type = RenderBuffer::Type::Vertex;
            m_vertexBuffer.m_usage = RenderBuffer::Usage::CPU_and_GPU;
            m_pRenderDevice->CreateBuffer( m_vertexBuffer, nullptr );

            if ( !m_vertexBuffer.IsValid() )
            {
                return false;
            }

            // Create index buffer for IMGUI
            m_indexBuffer.m_byteStride = sizeof( ImDrawIdx );
            m_indexBuffer.m_byteSize = m_indexBuffer.m_byteStride * g_numInitialIndices;
            m_indexBuffer.m_type = RenderBuffer::Type::Index;
            m_indexBuffer.m_usage = RenderBuffer::Usage::CPU_and_GPU;
            m_pRenderDevice->CreateBuffer( m_indexBuffer, nullptr );

            if ( !m_indexBuffer.IsValid() )
            {
                return false;
            }

            // Create blend state
            m_blendState.m_blendEnable = true;
            m_blendState.m_srcValue = BlendValue::SourceAlpha;
            m_blendState.m_dstValue = BlendValue::InverseSourceAlpha;
            m_blendState.m_blendOp = BlendOp::Add;
            m_blendState.m_srcAlphaValue = BlendValue::InverseSourceAlpha;
            m_blendState.m_dstAlphaValue = BlendValue::Zero;
            m_blendState.m_blendOpAlpha = BlendOp::Add;
            m_pRenderDevice->CreateBlendState( m_blendState );

            if ( !m_blendState.IsValid() )
            {
                return false;
            }

            // Set Rasterizer state for drawing
            m_rasterizerState.m_cullMode = CullMode::None;
            m_rasterizerState.m_windingMode = WindingMode::CounterClockwise;
            m_rasterizerState.m_fillMode = FillMode::Solid;
            m_rasterizerState.m_scissorCulling = true;
            m_pRenderDevice->CreateRasterizerState( m_rasterizerState );

            // Create sampler state
            m_pRenderDevice->CreateSamplerState( m_samplerState );

            // Create Shaders
            TVector<RenderBuffer> cbuffers;

            // World transform const buffer
            RenderBuffer buffer;
            buffer.m_byteSize = sizeof( Matrix );
            buffer.m_byteStride = 16; // Vector4 aligned
            buffer.m_usage = RenderBuffer::Usage::CPU_and_GPU;
            buffer.m_type = RenderBuffer::Type::Constant;
            cbuffers.push_back( buffer );

            // Vertex Shader
            VertexLayoutDescriptor vertexLayoutDesc;
            vertexLayoutDesc.m_elementDescriptors.push_back( VertexLayoutDescriptor::ElementDescriptor( DataSemantic::Position, DataTypeFormat::Float_R32G32, 0, 0 ) );
            vertexLayoutDesc.m_elementDescriptors.push_back( VertexLayoutDescriptor::ElementDescriptor( DataSemantic::TexCoord, DataTypeFormat::Float_R32G32, 0, 8 ) );
            vertexLayoutDesc.m_elementDescriptors.push_back( VertexLayoutDescriptor::ElementDescriptor( DataSemantic::Color, DataTypeFormat::UNorm_R8G8B8A8, 0, 16 ) );
            vertexLayoutDesc.CalculateByteSize();

            m_vertexShader = VertexShader( g_byteCode_VS_imgui, sizeof( g_byteCode_VS_imgui ), cbuffers, vertexLayoutDesc );
            m_pRenderDevice->CreateShader( m_vertexShader );

            // Create input binding
            m_pRenderDevice->CreateShaderInputBinding( m_vertexShader, vertexLayoutDesc, m_inputBinding );
            KRG_ASSERT( m_inputBinding.IsValid() );

            // Pixel Shader
            cbuffers.clear();
            m_pixelShader = PixelShader( g_byteCode_PS_imgui, sizeof( g_byteCode_PS_imgui ), cbuffers );
            m_pRenderDevice->CreateShader( m_pixelShader );

            // Set up PSO
            m_PSO.m_pVertexShader = &m_vertexShader;
            m_PSO.m_pPixelShader = &m_pixelShader;
            m_PSO.m_pBlendState = &m_blendState;
            m_PSO.m_pRasterizerState = &m_rasterizerState;

            //-------------------------------------------------------------------------
            // IMGUI Setup
            //-------------------------------------------------------------------------

            // Imgui needs to be initialized before this
            KRG_ASSERT( ::ImGui::GetCurrentContext() != nullptr );
            ImGuiIO& io = ::ImGui::GetIO();

            //-------------------------------------------------------------------------

            // Set render dimensions
            auto const renderTargetDimensions = m_pRenderDevice->GetRenderTargetDimensions();
            io.DisplaySize = ImVec2( (F32) renderTargetDimensions.x, (F32) renderTargetDimensions.y );

            Byte* pPixels = nullptr;
            Int2 dimensions;
            io.Fonts->GetTexDataAsRGBA32( &pPixels, &dimensions.x, &dimensions.y );
            m_fontTexture = Texture::InitializeTexture( TextureFormat::Raw, dimensions, pPixels, dimensions.x * dimensions.y * 4 );
            KRG_ASSERT( pPixels != nullptr );
            m_pRenderDevice->CreateTexture( m_fontTexture );

            io.Fonts->TexID = &m_fontTexture;
            io.Fonts->ClearInputData();
            io.Fonts->ClearTexData();

            //-------------------------------------------------------------------------

            // TODO: Multiple Viewport Support
            if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
            {
                //ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
                //platform_io.Renderer_CreateWindow = ImGui_ImplDX11_CreateWindow;
                //platform_io.Renderer_DestroyWindow = ImGui_ImplDX11_DestroyWindow;
                //platform_io.Renderer_SetWindowSize = ImGui_ImplDX11_SetWindowSize;
                //platform_io.Renderer_RenderWindow = ImGui_ImplDX11_RenderWindow;
                //platform_io.Renderer_SwapBuffers = ImGui_ImplDX11_SwapBuffers;
            }

            //-------------------------------------------------------------------------

            m_initialized = m_fontTexture.IsValid();
            return m_initialized;
        }

        void ImguiRenderer::Shutdown()
        {
            ImGui::DestroyPlatformWindows();

            //-------------------------------------------------------------------------

            m_PSO.Clear();

            if( m_inputBinding.IsValid() )
            {
                m_pRenderDevice->DestroyShaderInputBinding( m_inputBinding );
            }

            if( m_vertexShader.IsValid() )
            {
                m_pRenderDevice->DestroyShader( m_vertexShader );
            }

            if( m_pixelShader.IsValid() )
            {
                m_pRenderDevice->DestroyShader( m_pixelShader );
            }

            if ( m_samplerState.IsValid() )
            {
                m_pRenderDevice->DestroySamplerState( m_samplerState );
            }

            if ( m_vertexBuffer.IsValid() )
            {
                m_pRenderDevice->DestroyBuffer( m_vertexBuffer );
            }

            if ( m_indexBuffer.IsValid() )
            {
                m_pRenderDevice->DestroyBuffer( m_indexBuffer );
            }

            if ( m_fontTexture.IsValid() )
            {
                m_pRenderDevice->DestroyTexture( m_fontTexture );
            }

            if ( m_rasterizerState.IsValid() )
            {
                m_pRenderDevice->DestroyRasterizerState( m_rasterizerState );
            }

            if ( m_blendState.IsValid() )
            {
                m_pRenderDevice->DestroyBlendState( m_blendState );
            }

            m_pRenderDevice = nullptr;
            m_initialized = false;
        }

        void ImguiRenderer::Render( Math::Viewport const& viewport )
        {
            KRG_ASSERT( IsInitialized() && Threading::IsMainThread() );
            KRG_PROFILE_FUNCTION_RENDER();

            // Clear the previous frame buffers
            m_cmdBuffers.clear();

            //-------------------------------------------------------------------------

            // Update viewport dimensions
            ImGuiIO& io = ImGui::GetIO();
            Int2 const viewportDimensions = viewport.GetSize();
            io.DisplaySize = ImVec2( (F32) viewportDimensions.x, (F32) viewportDimensions.y );

            if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
            {
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
            }

            ImGui::Render();

            auto const pData = ImGui::GetDrawData();
            if ( pData == nullptr )
            {
                return;
            }

            // Buffer management
            //-------------------------------------------------------------------------

            // Check if our vertex and index buffers are large enough, if not then grow them
            if ( (S32) m_vertexBuffer.GetNumElements() < pData->TotalVtxCount )
            {
                m_pRenderDevice->ResizeBuffer( m_vertexBuffer, m_vertexBuffer.m_byteStride * pData->TotalVtxCount );
            }

            if ( (S32) m_indexBuffer.GetNumElements() < pData->TotalIdxCount )
            {
                m_pRenderDevice->ResizeBuffer( m_indexBuffer, m_indexBuffer.m_byteStride * pData->TotalIdxCount );
            }

            // Transfer buffer data
            //-------------------------------------------------------------------------

            auto renderContext = m_pRenderDevice->GetImmediateContext();

            // Copy vertices into our vertex and index buffers and record the command lists
            auto pVertexMemory = renderContext.MapBuffer( m_vertexBuffer );
            auto pVertex = (ImDrawVert*) pVertexMemory;
            auto endVertexMemoryBounds = ( (U8*) pVertexMemory ) + m_vertexBuffer.m_byteSize;

            auto pIndexMemory = renderContext.MapBuffer( m_indexBuffer );
            auto pIndex = (ImDrawIdx*) pIndexMemory;
            auto endIndexMemoryBounds = ( (U8*) pIndexMemory ) + m_indexBuffer.m_byteSize;

            for ( int n = 0; n < pData->CmdListsCount; n++ )
            {
                // Copy vertex / index data
                KRG_ASSERT( (U8*) pVertex < endVertexMemoryBounds && (U8*) pIndex < endIndexMemoryBounds );
                const ImDrawList* cmd_list = pData->CmdLists[n];
                memcpy( pVertex, &cmd_list->VtxBuffer[0], cmd_list->VtxBuffer.size() * sizeof( ImDrawVert ) );
                memcpy( pIndex, &cmd_list->IdxBuffer[0], cmd_list->IdxBuffer.size() * sizeof( ImDrawIdx ) );
                pVertex += cmd_list->VtxBuffer.size();
                pIndex += cmd_list->IdxBuffer.size();

                // Copy command buffer
                m_cmdBuffers.emplace_back( RecordedCmdBuffer() );
                m_cmdBuffers.back().m_numVertices = (U32) cmd_list->VtxBuffer.size();
                m_cmdBuffers.back().m_cmdBuffer.resize( cmd_list->CmdBuffer.size() );
                memcpy( m_cmdBuffers.back().m_cmdBuffer.data(), cmd_list->CmdBuffer.Data, cmd_list->CmdBuffer.size() * sizeof( ImDrawCmd ) );
            }

            renderContext.UnmapBuffer( m_vertexBuffer );
            renderContext.UnmapBuffer( m_indexBuffer );

            //-------------------------------------------------------------------------

            // Set view projection matrix
            auto const dimensions = ImGui::GetIO().DisplaySize;
            auto const matrix = Matrix::OrthographicProjectionMatrixOffCenter( 0.0f, dimensions.x, dimensions.y, 0.0f, 0, 1.0f );
            renderContext.WriteToBuffer( m_vertexShader.GetConstBuffer( 0 ), &matrix, sizeof( Matrix ) );

            // Set pipeline and render state
            renderContext.SetViewport( Float2( viewport.GetSize() ), Float2( viewport.GetTopLeftPosition() ) );
            renderContext.SetPipelineState( m_PSO );
            renderContext.SetShaderInputBinding( m_inputBinding );
            renderContext.SetPrimitiveTopology( Topology::TriangleList );
            renderContext.SetVertexBuffer( m_vertexBuffer );
            renderContext.SetIndexBuffer( m_indexBuffer );
            renderContext.SetSampler( PipelineStage::Pixel, 0, m_samplerState );
            renderContext.SetDepthTestMode( DepthTestMode::Off );

            // Render command lists
            int vertexOffset = 0;
            int indexOffset = 0;
            for ( auto const& recordedBuffer : m_cmdBuffers )
            {
                auto const numCommands = recordedBuffer.m_cmdBuffer.size();
                for ( auto i = 0; i < numCommands; i++ )
                {
                    ImDrawCmd const* pCmd = &recordedBuffer.m_cmdBuffer[i];
                    if ( pCmd->UserCallback )
                    {
                        KRG_UNIMPLEMENTED_FUNCTION();
                    }
                    else
                    {
                        if ( pCmd->TextureId == &m_fontTexture )
                        {
                            renderContext.SetTexture( PipelineStage::Pixel, 0, m_fontTexture );
                        }
                        else
                        {
                            KRG_HALT();
                        }

                        ScissorRect scissorRect = { (S32) pCmd->ClipRect.x, (S32) pCmd->ClipRect.y, (S32) pCmd->ClipRect.z, (S32) pCmd->ClipRect.w };
                        renderContext.SetRasterizerScissorRectangles( &scissorRect, 1 );
                        renderContext.DrawIndexed( pCmd->ElemCount, indexOffset, vertexOffset );
                    }
                    indexOffset += pCmd->ElemCount;
                }
                vertexOffset += recordedBuffer.m_numVertices;
            }
        }
    }
}
#endif