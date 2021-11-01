#include "WorldRenderer.h"
#include "Engine/Render/Components/StaticMeshComponent.h"
#include "Engine/Render/Components/SkeletalMeshComponent.h"
#include "Engine/Render/Shaders/EngineShaders.h"
#include "Engine/Core/Entity/Entity.h"
#include "Engine/Core/Entity/EntityUpdateContext.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "Engine/Render/WorldSystems/WorldRendererSystem.h"
#include "System/Render/RenderDefaultResources.h"
#include "System/Render/RenderViewport.h"
#include "System/Core/Profiling/Profiling.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    bool WorldRenderer::Initialize( RenderDevice* pRenderDevice )
    {
        KRG_ASSERT( m_pRenderDevice == nullptr && pRenderDevice != nullptr );
        m_pRenderDevice = pRenderDevice;

        TVector<RenderBuffer> cbuffers;
        RenderBuffer buffer;

        // Create Static Mesh Vertex Shader
        //-------------------------------------------------------------------------

        cbuffers.clear();

        // World transform const buffer
        buffer.m_byteSize = sizeof( Matrix );
        buffer.m_byteStride = sizeof( Matrix ); // Vector4 aligned
        buffer.m_usage = RenderBuffer::Usage::CPU_and_GPU;
        buffer.m_type = RenderBuffer::Type::Constant;
        buffer.m_slot = 0;
        cbuffers.push_back( buffer );

        // Shaders
        auto const vertexLayoutDescStatic = VertexLayoutRegistry::GetDescriptorForFormat( VertexFormat::StaticMesh );
        m_vertexShaderStatic = VertexShader( g_byteCode_VS_StaticPrimitive, sizeof( g_byteCode_VS_StaticPrimitive ), cbuffers, vertexLayoutDescStatic );
        m_pRenderDevice->CreateShader( m_vertexShaderStatic );

        // Create Skeletal Mesh Vertex Shader
        //-------------------------------------------------------------------------

        cbuffers.clear();

        // Vertex shader constant buffer - contains the world view projection matrix and bone transforms
        buffer.m_byteSize = sizeof( Matrix ) * 256; // ( 1 WVP matrix + 255 bone matrices )
        buffer.m_byteStride = sizeof( Matrix ); // Vector4 aligned
        buffer.m_usage = RenderBuffer::Usage::CPU_and_GPU;
        buffer.m_type = RenderBuffer::Type::Constant;
        buffer.m_slot = 0;
        cbuffers.push_back( buffer );

        // Shaders
        auto const vertexLayoutDescSkeletal = VertexLayoutRegistry::GetDescriptorForFormat( VertexFormat::SkeletalMesh );
        m_vertexShaderSkeletal = VertexShader( g_byteCode_VS_SkinnedPrimitive, sizeof( g_byteCode_VS_SkinnedPrimitive ), cbuffers, vertexLayoutDescSkeletal );
        pRenderDevice->CreateShader( m_vertexShaderSkeletal );

        // Create Pixel Shader
        //-------------------------------------------------------------------------

        cbuffers.clear();

        m_pixelShader = PixelShader( g_byteCode_PS_Textured, sizeof( g_byteCode_PS_Textured ), cbuffers );
        m_pRenderDevice->CreateShader( m_pixelShader );

        if ( !( m_pixelShader.IsValid() && m_vertexShaderStatic.IsValid() ) )
        {
            return false;
        }

        // Create blend state
        //-------------------------------------------------------------------------

        m_blendState.m_srcValue = BlendValue::SourceAlpha;
        m_blendState.m_dstValue = BlendValue::InverseSourceAlpha;
        m_blendState.m_blendOp = BlendOp::Add;
        m_blendState.m_blendEnable = true;

        m_pRenderDevice->CreateBlendState( m_blendState );

        if ( !m_blendState.IsValid() )
        {
            return false;
        }

        // Create rasterizer state
        //-------------------------------------------------------------------------

        m_rasterizerState.m_cullMode = CullMode::BackFace;
        m_rasterizerState.m_windingMode = WindingMode::CounterClockwise;
        m_rasterizerState.m_fillMode = FillMode::Solid;
        m_pRenderDevice->CreateRasterizerState( m_rasterizerState );
        if ( !m_rasterizerState.IsValid() )
        {
            return false;
        }

        // Set up sampler
        m_pRenderDevice->CreateSamplerState( m_samplerState );
        if ( !m_samplerState.IsValid() )
        {
            return false;
        }

        // Set up input bindings
        m_pRenderDevice->CreateShaderInputBinding( m_vertexShaderStatic, vertexLayoutDescStatic, m_inputBindingStatic );
        if ( !m_inputBindingStatic.IsValid() )
        {
            return false;
        }

        m_pRenderDevice->CreateShaderInputBinding( m_vertexShaderSkeletal, vertexLayoutDescSkeletal, m_inputBindingSkeletal );
        if ( !m_inputBindingSkeletal.IsValid() )
        {
            return false;
        }

        // Set up pipeline states
        m_pipelineStateStatic.m_pVertexShader = &m_vertexShaderStatic;
        m_pipelineStateStatic.m_pPixelShader = &m_pixelShader;
        m_pipelineStateStatic.m_pBlendState = &m_blendState;
        m_pipelineStateStatic.m_pRasterizerState = &m_rasterizerState;

        m_pipelineStateSkeletal.m_pVertexShader = &m_vertexShaderSkeletal;
        m_pipelineStateSkeletal.m_pPixelShader = &m_pixelShader;
        m_pipelineStateSkeletal.m_pBlendState = &m_blendState;
        m_pipelineStateSkeletal.m_pRasterizerState = &m_rasterizerState;

        m_initialized = true;
        return true;
    }

    void WorldRenderer::Shutdown()
    {
        m_pipelineStateStatic.Clear();
        m_pipelineStateSkeletal.Clear();

        if ( m_inputBindingStatic.IsValid() )
        {
            m_pRenderDevice->DestroyShaderInputBinding( m_inputBindingStatic );
        }

        if ( m_inputBindingSkeletal.IsValid() )
        {
            m_pRenderDevice->DestroyShaderInputBinding( m_inputBindingSkeletal );
        }

        if ( m_rasterizerState.IsValid() )
        {
            m_pRenderDevice->DestroyRasterizerState( m_rasterizerState );
        }

        if ( m_blendState.IsValid() )
        {
            m_pRenderDevice->DestroyBlendState( m_blendState );
        }

        if ( m_samplerState.IsValid() )
        {
            m_pRenderDevice->DestroySamplerState( m_samplerState );
        }

        if ( m_vertexShaderStatic.IsValid() )
        {
            m_pRenderDevice->DestroyShader( m_vertexShaderStatic );
        }

        if ( m_vertexShaderSkeletal.IsValid() )
        {
            m_pRenderDevice->DestroyShader( m_vertexShaderSkeletal );
        }

        if ( m_pixelShader.IsValid() )
        {
            m_pRenderDevice->DestroyShader( m_pixelShader );
        }

        m_pRenderDevice = nullptr;
        m_initialized = false;
    }

    //-------------------------------------------------------------------------

    void SubmitMeshDrawCall( RenderContext const& renderContext, StaticMesh const* pMesh, TVector<Material const*> const& materials )
    {
        renderContext.SetVertexBuffer( pMesh->GetVertexBuffer() );
        renderContext.SetIndexBuffer( pMesh->GetIndexBuffer() );

        auto const numSubMeshes = pMesh->GetNumSections();
        for ( auto i = 0u; i < numSubMeshes; i++ )
        {
            if ( i < materials.size() )
            {
                if ( materials[i] != nullptr )
                {
                    renderContext.SetShaderResource( PipelineStage::Pixel, 0, materials[i]->GetAlbedoTexture()->GetShaderResourceView() );
                }
                else
                {
                    renderContext.SetShaderResource( PipelineStage::Pixel, 0, DefaultResources::GetDefaultTexture()->GetShaderResourceView() );
                }
            }

            auto const& subMesh = pMesh->GetSection( i );
            renderContext.DrawIndexed( subMesh.m_numIndices, subMesh.m_startIndex );
        }
    }

    void WorldRenderer::RenderStaticMeshes( Viewport const& viewport, TVector<StaticMeshComponent const*> const& meshComponents )
    {
        KRG_PROFILE_FUNCTION_RENDER();

        auto const& renderContext = m_pRenderDevice->GetImmediateContext();
        Matrix const& viewProjectionMatrix = viewport.GetViewVolume().GetViewProjectionMatrix();

        //-------------------------------------------------------------------------

        // Set primary render state and clear the render buffer
        renderContext.SetViewport( Float2( viewport.GetDimensions() ), Float2( viewport.GetTopLeftPosition() ) );
        renderContext.SetDepthTestMode( DepthTestMode::On );
        renderContext.SetSampler( PipelineStage::Pixel, 0, m_samplerState );
        renderContext.SetPipelineState( m_pipelineStateStatic );
        renderContext.SetShaderInputBinding( m_inputBindingStatic );
        renderContext.SetPrimitiveTopology( Topology::TriangleList );

        //-------------------------------------------------------------------------

        for ( StaticMeshComponent const* pMeshComponent : meshComponents )
        {
            auto pMesh = pMeshComponent->GetMesh();
            Matrix const worldMatrix = pMeshComponent->GetWorldTransform().ToMatrix();
            Matrix const wvp = worldMatrix * viewProjectionMatrix;
            renderContext.WriteToBuffer( m_vertexShaderStatic.GetConstBuffer( 0 ), &wvp, sizeof( Matrix ) );

            SubmitMeshDrawCall( renderContext, pMesh, pMeshComponent->GetMaterials() );
        }
    }

    void WorldRenderer::RenderSkeletalMeshes( Viewport const& viewport, TVector<SkeletalMeshComponent const*> const& meshComponents )
    {
        KRG_PROFILE_FUNCTION_RENDER();

        auto const& renderContext = m_pRenderDevice->GetImmediateContext();
        Matrix const& viewProjectionMatrix = viewport.GetViewVolume().GetViewProjectionMatrix();

        //-------------------------------------------------------------------------

        // Set primary render state and clear the render buffer
        renderContext.SetViewport( Float2( viewport.GetDimensions() ), Float2( viewport.GetTopLeftPosition() ) );
        renderContext.SetDepthTestMode( DepthTestMode::On );
        renderContext.SetSampler( PipelineStage::Pixel, 0, m_samplerState );
        renderContext.SetPipelineState( m_pipelineStateSkeletal );
        renderContext.SetShaderInputBinding( m_inputBindingSkeletal );
        renderContext.SetPrimitiveTopology( Topology::TriangleList );

        //-------------------------------------------------------------------------

        SkeletalMesh const* pCurrentMesh = nullptr;
        Texture const* pCurrentTexture = nullptr;

        for ( SkeletalMeshComponent const* pMeshComponent : meshComponents )
        {
            if ( pMeshComponent->GetMesh() != pCurrentMesh )
            {
                pCurrentMesh = pMeshComponent->GetMesh();
                KRG_ASSERT( pCurrentMesh != nullptr && pCurrentMesh->IsValid() );

                renderContext.SetVertexBuffer( pCurrentMesh->GetVertexBuffer() );
                renderContext.SetIndexBuffer( pCurrentMesh->GetIndexBuffer() );
            }

            // Update Bones and Transforms
            //-------------------------------------------------------------------------

            Matrix const worldViewProjectionMatrix = pMeshComponent->GetWorldTransform().ToMatrix() * viewProjectionMatrix;
            auto const& boneTransforms = pMeshComponent->GetSkinningTransforms();
            KRG_ASSERT( boneTransforms.size() == pCurrentMesh->GetNumBones() );

            auto const& vertexShaderConstBuffer = m_vertexShaderSkeletal.GetConstBuffer( 0 );
            Matrix* pBufferData = (Matrix*) renderContext.MapBuffer( vertexShaderConstBuffer );
            {
                *pBufferData = worldViewProjectionMatrix;
                memcpy( pBufferData, &worldViewProjectionMatrix, sizeof( Matrix ) );
                pBufferData++;
                memcpy( pBufferData, boneTransforms.data(), sizeof( Matrix ) * pCurrentMesh->GetNumBones() );
            }
            renderContext.UnmapBuffer( vertexShaderConstBuffer );

            // Draw sub-meshes
            //-------------------------------------------------------------------------

            TVector<Material const*> const& materials = pMeshComponent->GetMaterials();

            auto const numSubMeshes = pCurrentMesh->GetNumSections();
            for ( auto i = 0u; i < numSubMeshes; i++ )
            {
                // Only set texture if needed
                Texture const*  pTextureToSet = nullptr;

                if ( i < materials.size() && materials[i] != nullptr )
                {
                    pTextureToSet = materials[i]->GetAlbedoTexture();
                }
                else
                {
                    pTextureToSet = DefaultResources::GetDefaultTexture();
                }

                if ( pTextureToSet != pCurrentTexture )
                {
                    pCurrentTexture = pTextureToSet;
                    renderContext.SetShaderResource( PipelineStage::Pixel, 0, pTextureToSet->GetShaderResourceView() );
                }

                // Draw mesh
                auto const& subMesh = pCurrentMesh->GetSection( i );
                renderContext.DrawIndexed( subMesh.m_numIndices, subMesh.m_startIndex );
            }
        }
    }

    void WorldRenderer::RenderWorld( Viewport const& viewport, EntityWorld* pWorld )
    {
        KRG_ASSERT( IsInitialized() && Threading::IsMainThread() );
        KRG_PROFILE_FUNCTION_RENDER();

        if ( !viewport.IsValid() )
        {
            return;
        }

        //-------------------------------------------------------------------------

        auto pWorldSystem = pWorld->GetWorldSystem<WorldRendererSystem>();
        KRG_ASSERT( pWorldSystem != nullptr );

        RenderStaticMeshes( viewport, pWorldSystem->m_visibleStaticMeshComponents );
        RenderSkeletalMeshes( viewport, pWorldSystem->m_visibleSkeletalMeshComponents );
    }
}