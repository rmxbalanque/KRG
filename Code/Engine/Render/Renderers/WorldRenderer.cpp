#include "WorldRenderer.h"
#include "Engine/Render/Components/EnvironmentMapComponents.h"
#include "Engine/Render/Components/LightComponents.h"
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
    float sqr(float x) {return x* x;}

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
        buffer.m_byteSize = sizeof( Transforms );
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

        // Vertex shader constant buffer - contains the world view projection matrix and bone transforms
        buffer.m_byteSize = sizeof( Matrix ) * 255; // ( 1 WVP matrix + 255 bone matrices )
        buffer.m_byteStride = sizeof( Matrix ); // Vector4 aligned
        buffer.m_usage = RenderBuffer::Usage::CPU_and_GPU;
        buffer.m_type = RenderBuffer::Type::Constant;
        buffer.m_slot = 0;
        cbuffers.push_back( buffer );

        // Shaders
        auto const vertexLayoutDescSkeletal = VertexLayoutRegistry::GetDescriptorForFormat( VertexFormat::SkeletalMesh );
        m_vertexShaderSkeletal = VertexShader( g_byteCode_VS_SkinnedPrimitive, sizeof( g_byteCode_VS_SkinnedPrimitive ), cbuffers, vertexLayoutDescSkeletal );
        pRenderDevice->CreateShader( m_vertexShaderSkeletal );

        // Create Skybox Vertex Shader
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
        auto const vertexLayoutDescNone = VertexLayoutRegistry::GetDescriptorForFormat( VertexFormat::None );
        m_vertexShaderSkybox = VertexShader( g_byteCode_VS_Cube, sizeof( g_byteCode_VS_Cube ), cbuffers, vertexLayoutDescNone );
        m_pRenderDevice->CreateShader( m_vertexShaderSkybox );


        // Create Pixel Shader
        //-------------------------------------------------------------------------

        cbuffers.clear();

        // Picel shader constant buffer - contains light info
        buffer.m_byteSize = sizeof(LightData);
        buffer.m_byteStride = sizeof( Vector );
        buffer.m_usage = RenderBuffer::Usage::CPU_and_GPU;
        buffer.m_type = RenderBuffer::Type::Constant;
        buffer.m_slot = 0;
        cbuffers.push_back( buffer );

        buffer.m_byteSize = sizeof( MaterialData );
        buffer.m_byteStride = sizeof( Vector );
        buffer.m_usage = RenderBuffer::Usage::CPU_and_GPU;
        buffer.m_type = RenderBuffer::Type::Constant;
        buffer.m_slot = 1;
        cbuffers.push_back( buffer );

        m_pixelShader = PixelShader( g_byteCode_PS_Lit, sizeof( g_byteCode_PS_Lit ), cbuffers );
        m_pRenderDevice->CreateShader( m_pixelShader );

        if ( !( m_pixelShader.IsValid() && m_vertexShaderStatic.IsValid() ) )
        {
            return false;
        }

        // Create Skybox Pixel Shader
        //-------------------------------------------------------------------------

        // Shaders
        m_pixelShaderSkybox = PixelShader( g_byteCode_PS_Skybox, sizeof( g_byteCode_PS_Skybox ), cbuffers );
        m_pRenderDevice->CreateShader( m_pixelShaderSkybox );

        if ( !m_pixelShaderSkybox.IsValid() )
        {
            return false;
        }

        // Create Empty Pixel Shader
        //-------------------------------------------------------------------------
        cbuffers.clear();

        // Shaders
        m_emptyPixelShader = PixelShader( g_byteCode_PS_Empty, sizeof( g_byteCode_PS_Empty ), cbuffers );
        m_pRenderDevice->CreateShader( m_emptyPixelShader );

        if ( !m_emptyPixelShader.IsValid() )
        {
            return false;
        }


        // Create BRDF Integration Compute Shader
        //-------------------------------------------------------------------------
        cbuffers.clear();

        m_precomputeDFGComputeShader = ComputeShader( g_byteCode_CS_PrecomputeDFG, sizeof(g_byteCode_CS_PrecomputeDFG), cbuffers);

        m_pRenderDevice->CreateShader( m_precomputeDFGComputeShader );
        if ( !m_precomputeDFGComputeShader.IsValid() )
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
        m_pRenderDevice->CreateSamplerState( m_bilinearSampler );
        if ( !m_bilinearSampler.IsValid() )
        {
            return false;
        }

        m_bilinearClampedSampler.m_addressModeU = TextureAddressMode::Clamp;
        m_bilinearClampedSampler.m_addressModeV = TextureAddressMode::Clamp;
        m_bilinearClampedSampler.m_addressModeW = TextureAddressMode::Clamp;
        m_pRenderDevice->CreateSamplerState( m_bilinearClampedSampler );
        if ( !m_bilinearClampedSampler.IsValid() )
        {
            return false;
        }


        m_shadowSampler.m_addressModeU = TextureAddressMode::Border;
        m_shadowSampler.m_addressModeV = TextureAddressMode::Border;
        m_shadowSampler.m_addressModeW = TextureAddressMode::Border;
        m_shadowSampler.m_borderColor = Float4(1.0f);
        m_pRenderDevice->CreateSamplerState( m_shadowSampler );
        if ( !m_shadowSampler.IsValid() )
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

        m_pipelineStateStaticShadow.m_pVertexShader = &m_vertexShaderStatic;
        m_pipelineStateStaticShadow.m_pPixelShader = &m_emptyPixelShader;
        m_pipelineStateStaticShadow.m_pBlendState = &m_blendState;
        m_pipelineStateStaticShadow.m_pRasterizerState = &m_rasterizerState;

        m_pipelineStateSkeletalShadow.m_pVertexShader = &m_vertexShaderSkeletal;
        m_pipelineStateSkeletalShadow.m_pPixelShader = &m_emptyPixelShader;
        m_pipelineStateSkeletalShadow.m_pBlendState = &m_blendState;
        m_pipelineStateSkeletalShadow.m_pRasterizerState = &m_rasterizerState;

        m_pipelineSkybox.m_pVertexShader = &m_vertexShaderSkybox;
        m_pipelineSkybox.m_pPixelShader = &m_pixelShaderSkybox;

        m_pRenderDevice->CreateTexture(m_precomputedBRDF, DataTypeFormat::Float_R16G16, Float2(512, 512), USAGE_UAV|USAGE_SRV); // TODO: load from memory?
        m_pipelinePrecomputeBRDF.m_pComputeShader = &m_precomputeDFGComputeShader;

        // TODO create on directional light add and destroy on remove
        m_pRenderDevice->CreateTexture(m_shadowMap, DataTypeFormat::Float_D32, Float2(1536, 1536), USAGE_SRV|USAGE_RT_DS);

        {
            auto const& renderContext = m_pRenderDevice->GetImmediateContext();
            renderContext.SetPipelineState( m_pipelinePrecomputeBRDF );
            renderContext.SetUnorderedAccess( PipelineStage::Compute, 0, m_precomputedBRDF.GetUnorderedAccessView() );
            renderContext.Dispatch(512/16, 512/16, 1);
            renderContext.ClearUnorderedAccess( PipelineStage::Compute, 0 );
        }

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

        if ( m_bilinearSampler.IsValid() )
        {
            m_pRenderDevice->DestroySamplerState( m_bilinearSampler );
        }

        if ( m_shadowSampler.IsValid() )
        {
            m_pRenderDevice->DestroySamplerState( m_shadowSampler );
        }

        if ( m_bilinearClampedSampler.IsValid() )
        {
            m_pRenderDevice->DestroySamplerState( m_bilinearClampedSampler );
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

        if ( m_emptyPixelShader.IsValid() )
        {
            m_pRenderDevice->DestroyShader( m_emptyPixelShader );
        }

        if ( m_vertexShaderSkybox.IsValid() )
        {
            m_pRenderDevice->DestroyShader( m_vertexShaderSkybox );
        }

        if ( m_pixelShaderSkybox.IsValid() )
        {
            m_pRenderDevice->DestroyShader( m_pixelShaderSkybox );
        }

        if ( m_precomputeDFGComputeShader.IsValid() )
        {
            m_pRenderDevice->DestroyShader( m_precomputeDFGComputeShader );
        }

        if ( m_precomputedBRDF.IsValid() )
        {
            m_pRenderDevice->DestroyTexture( m_precomputedBRDF );
        }

        if ( m_shadowMap.IsValid() )
        {
            m_pRenderDevice->DestroyTexture( m_shadowMap );
        }

        m_pRenderDevice = nullptr;
        m_initialized = false;
    }

    void WorldRenderer::SetupMaterial( RenderContext const& renderContext, PixelShader& pixelShader, const Material* material )
    {
        const ViewSRVHandle& defaultSRV = DefaultResources::GetDefaultTexture()->GetShaderResourceView();

        // TODO: cache on GPU in buffer
        MaterialData materialData{};

        materialData.m_surfaceFlags |= material->HasAlbedoTexture() ? MATERIAL_USE_ALBEDO_TEXTURE : materialData.m_surfaceFlags;
        materialData.m_surfaceFlags |= material->HasMetalnessTexture() ? MATERIAL_USE_METALNESS_TEXTURE : materialData.m_surfaceFlags;
        materialData.m_surfaceFlags |= material->HasRoughnessTexture() ? MATERIAL_USE_ROUGHNESS_TEXTURE : materialData.m_surfaceFlags;
        materialData.m_surfaceFlags |= material->HasNormalMapTexture() ? MATERIAL_USE_NORMAL_TEXTURE : materialData.m_surfaceFlags;
        materialData.m_surfaceFlags |= material->HasAOTexture() ? MATERIAL_USE_AO_TEXTURE : materialData.m_surfaceFlags;
        materialData.m_metalness = material->GetMetalnessValue();
        materialData.m_roughness = material->GetRoughnessValue();
        materialData.m_normalScaler = material->GetNormalScalerValue();
        materialData.m_albedo = (Float4)material->GetAlbedoValue();

        renderContext.WriteToBuffer( pixelShader.GetConstBuffer( 1 ), &materialData, sizeof( materialData ) );

        renderContext.SetShaderResource( PipelineStage::Pixel, 0, (materialData.m_surfaceFlags & MATERIAL_USE_ALBEDO_TEXTURE) ? material->GetAlbedoTexture()->GetShaderResourceView() : defaultSRV);
        renderContext.SetShaderResource( PipelineStage::Pixel, 1, (materialData.m_surfaceFlags & MATERIAL_USE_NORMAL_TEXTURE) ? material->GetNormalMapTexture()->GetShaderResourceView() : defaultSRV);
        renderContext.SetShaderResource( PipelineStage::Pixel, 2, (materialData.m_surfaceFlags & MATERIAL_USE_METALNESS_TEXTURE) ? material->GetMetalnessTexture()->GetShaderResourceView() : defaultSRV);
        renderContext.SetShaderResource( PipelineStage::Pixel, 3, (materialData.m_surfaceFlags & MATERIAL_USE_ROUGHNESS_TEXTURE) ? material->GetRoughnessTexture()->GetShaderResourceView() : defaultSRV);
        renderContext.SetShaderResource( PipelineStage::Pixel, 4, (materialData.m_surfaceFlags & MATERIAL_USE_AO_TEXTURE) ? material->GetAOTexture()->GetShaderResourceView() : defaultSRV);
    }



    //-------------------------------------------------------------------------
    void WorldRenderer::SetupRenderStates( Viewport const& viewport, const RenderData& data )
    {
        auto const& renderContext = m_pRenderDevice->GetImmediateContext();
        renderContext.SetViewport( Float2( viewport.GetDimensions() ), Float2( viewport.GetTopLeftPosition() ) );
        renderContext.SetDepthTestMode( DepthTestMode::On );
        renderContext.SetSampler( PipelineStage::Pixel, 0, m_bilinearSampler );
        renderContext.SetSampler( PipelineStage::Pixel, 1, m_bilinearClampedSampler );
        renderContext.SetSampler( PipelineStage::Pixel, 2, m_shadowSampler );

        renderContext.WriteToBuffer( m_pixelShader.GetConstBuffer( 0 ), &data.m_lightData, sizeof( data.m_lightData ) );

        renderContext.SetShaderResource( PipelineStage::Pixel, 10, (data.m_lightData.m_lightingFlags & LIGHTING_ENABLE_SUN_SHADOW) ? m_shadowMap.GetShaderResourceView() : DefaultResources::GetDefaultTexture()->GetShaderResourceView() );

        if (data.m_pSkyboxRadianceTexture)
        {
            renderContext.SetShaderResource( PipelineStage::Pixel, 11, m_precomputedBRDF.GetShaderResourceView() );
            renderContext.SetShaderResource( PipelineStage::Pixel, 12, data.m_pSkyboxRadianceTexture->GetShaderResourceView() );
        }
        else
        {
            renderContext.SetShaderResource( PipelineStage::Pixel, 11, DefaultResources::GetDefaultTexture()->GetShaderResourceView() );
            renderContext.SetShaderResource( PipelineStage::Pixel, 12, ViewSRVHandle{} ); // TODO: fix add default cubemap resource
        }
    }

    void WorldRenderer::RenderStaticMeshes( Viewport const& viewport, const RenderData& data )
    {
        KRG_PROFILE_FUNCTION_RENDER();

        auto const& renderContext = m_pRenderDevice->GetImmediateContext();
        Vector const& camPosition = viewport.GetViewVolume().GetViewPosition();
        //-------------------------------------------------------------------------

        SetupRenderStates( viewport, data );

        // Set primary render state and clear the render buffer
        renderContext.SetPipelineState( m_pipelineStateStatic );
        renderContext.SetShaderInputBinding( m_inputBindingStatic );
        renderContext.SetPrimitiveTopology( Topology::TriangleList );

        //-------------------------------------------------------------------------
        for ( StaticMeshComponent const* pMeshComponent : data.m_staticMeshComponents )
        {
            auto pMesh = pMeshComponent->GetMesh();
            Matrix worldTransform = pMeshComponent->GetWorldTransform().ToMatrix();
            Transforms transforms = data.m_transforms;
            transforms.m_worldTransform = worldTransform;
            transforms.m_worldTransform.SetTranslation(worldTransform.GetTranslation() - camPosition);
            transforms.m_normalTransform = transforms.m_worldTransform.GetInverse().Transpose();
            renderContext.WriteToBuffer( m_vertexShaderStatic.GetConstBuffer( 0 ), &transforms, sizeof( transforms ) );

            renderContext.SetVertexBuffer( pMesh->GetVertexBuffer() );
            renderContext.SetIndexBuffer( pMesh->GetIndexBuffer() );

            TVector<Material const*> const& materials = pMeshComponent->GetMaterials();

            auto const numSubMeshes = pMesh->GetNumSections();
            for ( auto i = 0u; i < numSubMeshes; i++ )
            {
                if ( i < materials.size() && materials[i])
                {
                    SetupMaterial(renderContext, m_pixelShader, materials[i]);
                }

                auto const& subMesh = pMesh->GetSection( i );
                renderContext.DrawIndexed( subMesh.m_numIndices, subMesh.m_startIndex );
            }
        }
        renderContext.ClearShaderResource( PipelineStage::Pixel, 10 );
    }

    void WorldRenderer::RenderSkeletalMeshes( Viewport const& viewport, const RenderData& data )
    {
        KRG_PROFILE_FUNCTION_RENDER();

        auto const& renderContext = m_pRenderDevice->GetImmediateContext();
        Matrix const& viewProjectionMatrix = viewport.GetViewVolume().GetViewProjectionMatrix();
        Vector const& camPosition = viewport.GetViewVolume().GetViewPosition();

        //-------------------------------------------------------------------------
        SetupRenderStates( viewport, data );

        // Set primary render state and clear the render buffer
        renderContext.SetPipelineState( m_pipelineStateSkeletal );
        renderContext.SetShaderInputBinding( m_inputBindingSkeletal );
        renderContext.SetPrimitiveTopology( Topology::TriangleList );

        //-------------------------------------------------------------------------

        SkeletalMesh const* pCurrentMesh = nullptr;

        for ( SkeletalMeshComponent const* pMeshComponent : data.m_skeletalMeshComponents )
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

            Matrix worldTransform = pMeshComponent->GetWorldTransform().ToMatrix();
            Transforms transforms = data.m_transforms;
            transforms.m_worldTransform = worldTransform;
            transforms.m_worldTransform.SetTranslation(worldTransform.GetTranslation() - camPosition);
            transforms.m_normalTransform = transforms.m_worldTransform.GetInverse().Transpose();
            renderContext.WriteToBuffer( m_vertexShaderSkeletal.GetConstBuffer( 0 ), &transforms, sizeof( transforms ) );


            auto const& bonesConstBuffer = m_vertexShaderSkeletal.GetConstBuffer( 1 );
            auto const& boneTransforms = pMeshComponent->GetSkinningTransforms();
            KRG_ASSERT( boneTransforms.size() == pCurrentMesh->GetNumBones() );
            renderContext.WriteToBuffer( bonesConstBuffer, boneTransforms.data(), sizeof( Matrix ) * pCurrentMesh->GetNumBones() );

            // Draw sub-meshes
            //-------------------------------------------------------------------------

            TVector<Material const*> const& materials = pMeshComponent->GetMaterials();

            auto const numSubMeshes = pCurrentMesh->GetNumSections();
            for ( auto i = 0u; i < numSubMeshes; i++ )
            {
                if ( i < materials.size() && materials[i])
                {
                    SetupMaterial(renderContext, m_pixelShader, materials[i]);
                }

                // Draw mesh
                auto const& subMesh = pCurrentMesh->GetSection( i );
                renderContext.DrawIndexed( subMesh.m_numIndices, subMesh.m_startIndex );
            }
        }
        renderContext.ClearShaderResource( PipelineStage::Pixel, 10 );
    }

    void WorldRenderer::RenderSkybox( Viewport const& viewport, const RenderData& data )
    {
        auto const& renderContext = m_pRenderDevice->GetImmediateContext();
        if (data.m_pSkyboxTexture)
        {
            renderContext.SetViewport( Float2( viewport.GetDimensions() ), Float2( viewport.GetTopLeftPosition() ), Float2(1, 1)/*TODO: fix for inv z*/ );
            renderContext.SetPipelineState( m_pipelineSkybox );
            renderContext.SetShaderInputBinding( ShaderInputBindingHandle() );
            renderContext.SetPrimitiveTopology( Topology::TriangleStrip );
            renderContext.WriteToBuffer( m_vertexShaderSkybox.GetConstBuffer( 0 ), &data.m_transforms.m_viewprojTransform, sizeof( data.m_transforms.m_viewprojTransform ) );
            renderContext.WriteToBuffer( m_pixelShaderSkybox.GetConstBuffer( 0 ), &data.m_lightData, sizeof( data.m_lightData ) );
            renderContext.SetShaderResource( PipelineStage::Pixel, 0, data.m_pSkyboxTexture->GetShaderResourceView() );
            renderContext.Draw( 14, 0 );
        }
    }

    Matrix ComputeShadowMatrix(Viewport const& viewport, const Transform& lightWorldTransform, float shadowDistance, float guardFactor = 0.01f)
    {
        Vector const& camPosition = viewport.GetViewVolume().GetViewPosition();
        Transform lightTransform = lightWorldTransform;
        lightTransform.SetTranslation(Vector(0.0));
        lightTransform.Inverse();

        Vector corners[8];
        viewport.GetViewVolume().GetCorners(corners);
        Vector nearCorners[4];

        nearCorners[0] = lightTransform.ApplyTransform(corners[0] - camPosition);
        nearCorners[1] = lightTransform.ApplyTransform(corners[1] - camPosition);
        nearCorners[2] = lightTransform.ApplyTransform(corners[2] - camPosition);
        nearCorners[3] = lightTransform.ApplyTransform(corners[3] - camPosition);

        float const distScale = shadowDistance/viewport.GetViewVolume().GetDepthRange().m_start;

        // TODO: optimize - can be done with 4 min + 4 max
        Vector vmin = Vector::Min(nearCorners[0], nearCorners[0] * distScale);
        vmin = Vector::Min(vmin, nearCorners[1]);
        vmin = Vector::Min(vmin, nearCorners[1] * distScale);
        vmin = Vector::Min(vmin, nearCorners[2]);
        vmin = Vector::Min(vmin, nearCorners[2] * distScale);
        vmin = Vector::Min(vmin, nearCorners[3]);
        vmin = Vector::Min(vmin, nearCorners[3] * distScale);

        Vector vmax = Vector::Max(nearCorners[0], nearCorners[0] * distScale);
        vmax = Vector::Max(vmax, nearCorners[1]);
        vmax = Vector::Max(vmax, nearCorners[1] * distScale);
        vmax = Vector::Max(vmax, nearCorners[2]);
        vmax = Vector::Max(vmax, nearCorners[2] * distScale);
        vmax = Vector::Max(vmax, nearCorners[3]);
        vmax = Vector::Max(vmax, nearCorners[3] * distScale);

        // TODO: safe guards should be derived from pixel space, configurable?
        vmin -= Vector(shadowDistance * guardFactor, 1000.0f/*TODO: should be based on world/shadow frustum intersection*/, shadowDistance * guardFactor);
        vmax += Vector(shadowDistance * guardFactor, 1000.0f/*TODO: should be based on world/shadow frustum intersection*/, shadowDistance * guardFactor);

        Vector delta = vmax - vmin;
        float dim = std::max(delta.m_x, delta.m_z);
        Matrix offsetMatrix{};
        offsetMatrix.SetTranslation(Float3(0.5f*(vmin.m_x+vmax.m_x), -vmax.m_y, 0.5f*(vmin.m_z+vmax.m_z)));
        Matrix lightWorldMatrix = offsetMatrix.GetInverse() * lightTransform.GetInverse().ToMatrix();
        Math::ViewVolume lightViewVolume(Float2(dim), FloatRange(0, delta.m_y), lightWorldMatrix);

        Matrix viewProjMatrix = lightViewVolume.GetViewProjectionMatrix();
        Matrix viewMatrix = lightViewVolume.GetViewMatrix();
        Matrix projMatrix = lightViewVolume.GetProjectionMatrix();

        Matrix viewProj = lightViewVolume.GetViewProjectionMatrix(); // TODO: inverse z???

#if 0
        Vector result0 = viewProj.ApplyTransform(Vector(0, 0, 0));
        Vector result1 = viewProj.ApplyTransform(Vector(0, -vmin.m_y, 0));
        Vector result2 = viewProj.ApplyTransform(Vector(0, -vmax.m_y, 0));

        Vector testCorners[8];

        testCorners[0] = (corners[0] - camPosition);
        testCorners[1] = (corners[1] - camPosition);
        testCorners[2] = (corners[2] - camPosition);
        testCorners[3] = (corners[3] - camPosition);

        testCorners[4] = ((corners[0] - camPosition) * distScale);
        testCorners[5] = ((corners[1] - camPosition) * distScale);
        testCorners[6] = ((corners[2] - camPosition) * distScale);
        testCorners[7] = ((corners[3] - camPosition) * distScale);


        testCorners[0] = viewMatrix.ApplyTransform((corners[0] - camPosition).SetW1());
        testCorners[1] = viewMatrix.ApplyTransform((corners[1] - camPosition).SetW1());
        testCorners[2] = viewMatrix.ApplyTransform((corners[2] - camPosition).SetW1());
        testCorners[3] = viewMatrix.ApplyTransform((corners[3] - camPosition).SetW1());

        testCorners[4] = viewMatrix.ApplyTransform(((corners[0] - camPosition) * distScale).SetW1());
        testCorners[5] = viewMatrix.ApplyTransform(((corners[1] - camPosition) * distScale).SetW1());
        testCorners[6] = viewMatrix.ApplyTransform(((corners[2] - camPosition) * distScale).SetW1());
        testCorners[7] = viewMatrix.ApplyTransform(((corners[3] - camPosition) * distScale).SetW1());



        testCorners[0] = viewProj.ApplyTransform((corners[0] - camPosition).SetW1());
        testCorners[1] = viewProj.ApplyTransform((corners[1] - camPosition).SetW1());
        testCorners[2] = viewProj.ApplyTransform((corners[2] - camPosition).SetW1());
        testCorners[3] = viewProj.ApplyTransform((corners[3] - camPosition).SetW1());

        testCorners[4] = viewProj.ApplyTransform(((corners[0] - camPosition) * distScale).SetW1());
        testCorners[5] = viewProj.ApplyTransform(((corners[1] - camPosition) * distScale).SetW1());
        testCorners[6] = viewProj.ApplyTransform(((corners[2] - camPosition) * distScale).SetW1());
        testCorners[7] = viewProj.ApplyTransform(((corners[3] - camPosition) * distScale).SetW1());
#endif

        return viewProj;
    }

    void WorldRenderer::RenderSunShadows( Viewport const& viewport, DirectionalLightComponent* pDirectionalLightComponent, const RenderData& data )
    {
        KRG_PROFILE_FUNCTION_RENDER();
        auto const& renderContext = m_pRenderDevice->GetImmediateContext();
        Vector const& camPosition = viewport.GetViewVolume().GetViewPosition();

        if (!pDirectionalLightComponent || !pDirectionalLightComponent->GetShadowed()) return;

       //-------------------------------------------------------------------------

        // Set primary render state and clear the render buffer

        renderContext.ClearDepthStencilView(m_shadowMap.GetDepthStencilView(), 1.0f/*TODO: inverse z*/, 0);
        renderContext.SetRenderTargetViews({}, m_shadowMap.GetDepthStencilView());
        renderContext.SetViewport( Float2((float)m_shadowMap.GetDimensions().m_x, (float)m_shadowMap.GetDimensions().m_y), Float2( 0.0f, 0.0f ) );
        renderContext.SetDepthTestMode( DepthTestMode::On );

        Transforms transforms{Matrix{ZeroInit{}}, Matrix{ZeroInit{}}, Matrix{ZeroInit{}}};
        transforms.m_viewprojTransform = data.m_lightData.m_sunShadowMapMatrix;

        //-------------------------------------------------------------------------
        renderContext.SetPipelineState( m_pipelineStateStaticShadow );
        renderContext.SetShaderInputBinding( m_inputBindingStatic );
        renderContext.SetPrimitiveTopology( Topology::TriangleList );
        for ( StaticMeshComponent const* pMeshComponent : data.m_staticMeshComponents )
        {
            auto pMesh = pMeshComponent->GetMesh();
            Matrix worldTransform = pMeshComponent->GetWorldTransform().ToMatrix();
            transforms.m_worldTransform = worldTransform;
            transforms.m_worldTransform.SetTranslation(worldTransform.GetTranslation() - camPosition); // TODO: move to shader
            renderContext.WriteToBuffer( m_vertexShaderStatic.GetConstBuffer( 0 ), &transforms, sizeof( transforms ) );

            renderContext.SetVertexBuffer( pMesh->GetVertexBuffer() );
            renderContext.SetIndexBuffer( pMesh->GetIndexBuffer() );

            auto const numSubMeshes = pMesh->GetNumSections();
            for ( auto i = 0u; i < numSubMeshes; i++ )
            {
                // Draw mesh
                auto const& subMesh = pMesh->GetSection( i );
                renderContext.DrawIndexed( subMesh.m_numIndices, subMesh.m_startIndex );
            }
        }

        renderContext.SetPipelineState( m_pipelineStateSkeletalShadow );
        renderContext.SetShaderInputBinding( m_inputBindingSkeletal );
        renderContext.SetPrimitiveTopology( Topology::TriangleList );
        for ( SkeletalMeshComponent const* pMeshComponent : data.m_skeletalMeshComponents )
        {
            auto pMesh = pMeshComponent->GetMesh();
            // Update Bones and Transforms
            //-------------------------------------------------------------------------

            Matrix worldTransform = pMeshComponent->GetWorldTransform().ToMatrix();
            transforms.m_worldTransform = worldTransform;
            transforms.m_worldTransform.SetTranslation(worldTransform.GetTranslation() - camPosition);
            renderContext.WriteToBuffer( m_vertexShaderSkeletal.GetConstBuffer( 0 ), &transforms, sizeof( transforms ) );


            auto const& bonesConstBuffer = m_vertexShaderSkeletal.GetConstBuffer( 1 );
            auto const& boneTransforms = pMeshComponent->GetSkinningTransforms();
            KRG_ASSERT( boneTransforms.size() == pMesh->GetNumBones() );
            renderContext.WriteToBuffer( bonesConstBuffer, boneTransforms.data(), sizeof( Matrix ) * pMesh->GetNumBones() );

            renderContext.SetVertexBuffer( pMesh->GetVertexBuffer() );
            renderContext.SetIndexBuffer( pMesh->GetIndexBuffer() );

            // Draw sub-meshes
            //-------------------------------------------------------------------------
            auto const numSubMeshes = pMesh->GetNumSections();
            for ( auto i = 0u; i < numSubMeshes; i++ )
            {
                // Draw mesh
                auto const& subMesh = pMesh->GetSection( i );
                renderContext.DrawIndexed( subMesh.m_numIndices, subMesh.m_startIndex );
            }
        }
    }

    void WorldRenderer::RenderWorld( RenderTarget const& target, Viewport const& viewport, EntityWorld* pWorld )
    {
        KRG_ASSERT( IsInitialized() && Threading::IsMainThread() );
        KRG_PROFILE_FUNCTION_RENDER();

        if ( !viewport.IsValid() )
        {
            return;
        }

        //-------------------------------------------------------------------------
        auto const& immediateContext = m_pRenderDevice->GetImmediateContext();

        auto pWorldSystem = pWorld->GetWorldSystem<WorldRendererSystem>();
        KRG_ASSERT( pWorldSystem != nullptr );

        RenderData renderData{
            Transforms{Matrix{ZeroInit{}}, Matrix{ZeroInit{}}, Matrix{ZeroInit{}}},
            LightData{Vector{0.0f}, Vector{0.0f}, Matrix{ZeroInit{}}, 0, 0},
            nullptr,
            nullptr,
            pWorldSystem->m_visibleStaticMeshComponents,
            pWorldSystem->m_visibleSkeletalMeshComponents,
        };

        Matrix viewProjMatrix = viewport.GetViewVolume().GetViewProjectionMatrix();
        Matrix viewMatrix = viewport.GetViewVolume().GetViewMatrix();
        Matrix projMatrix = viewport.GetViewVolume().GetProjectionMatrix();
        Vector const& camPosition = viewport.GetViewVolume().GetViewPosition();
        viewMatrix.SetTranslation(Float3(0.0f));
        renderData.m_transforms.m_viewprojTransform = viewMatrix * projMatrix;
        uint32 lightingFlags = 0;

        DirectionalLightComponent* pDirectionalLightComponent = nullptr;
        if (!pWorldSystem->m_registeredDirectionLightComponents.empty())
        {
            pDirectionalLightComponent = pWorldSystem->m_registeredDirectionLightComponents[0];
            lightingFlags |= LIGHTING_ENABLE_SUN;
            lightingFlags |= pDirectionalLightComponent->GetShadowed() ? LIGHTING_ENABLE_SUN_SHADOW : 0;
            renderData.m_lightData.m_SunDirIndirectIntensity = -pDirectionalLightComponent->GetLightDirection();
            Float4 colorIntensity = pDirectionalLightComponent->GetLightColor();
            renderData.m_lightData.m_SunColorRoughnessOneLevel = colorIntensity * pDirectionalLightComponent->GetLightIntensity();
            // TODO: conditional
            renderData.m_lightData.m_sunShadowMapMatrix = ComputeShadowMatrix(viewport, pDirectionalLightComponent->GetWorldTransform(), 50.0f/*TODO: configure*/);
        }

        renderData.m_lightData.m_SunColorRoughnessOneLevel.m_w = 0;
        if (!pWorldSystem->m_registeredGlobalEnvironmentMaps.empty())
        {
            GlobalEnvironmentMapComponent* pGlobalEnvironmentMapComponent = pWorldSystem->m_registeredGlobalEnvironmentMaps[0];
            lightingFlags |= LIGHTING_ENABLE_SKYLIGHT;
            renderData.m_pSkyboxRadianceTexture = pGlobalEnvironmentMapComponent->GetSkyboxRadianceTexture();
            renderData.m_pSkyboxTexture = pGlobalEnvironmentMapComponent->GetSkyboxTexture();
            renderData.m_lightData.m_SunColorRoughnessOneLevel.m_w = std::max(floor(log2f((float)renderData.m_pSkyboxRadianceTexture->GetDimensions().m_x)) - 1.0f, 0.0f);
            renderData.m_lightData.m_SunDirIndirectIntensity.m_w = pGlobalEnvironmentMapComponent->GetSkyboxIntensity();
        }

        uint32_t numPointLights = (uint32)std::min<size_t>(pWorldSystem->m_registeredPointLightComponents.size(), MAX_PUNCTUAL_LIGHTS);
        uint32_t lightIndex = 0;
        for (uint32_t i = 0; i <  numPointLights; ++i)
        {
            KRG_ASSERT(lightIndex < MAX_PUNCTUAL_LIGHTS);
            PointLightComponent* pPointLightComponent = pWorldSystem->m_registeredPointLightComponents[i];
            renderData.m_lightData.m_punctualLights[lightIndex].m_positionInvRadiusSqr = pPointLightComponent->GetLightPosition() - camPosition;
            renderData.m_lightData.m_punctualLights[lightIndex].m_positionInvRadiusSqr.m_w = sqr(1.0f/pPointLightComponent->GetLightRadius());
            renderData.m_lightData.m_punctualLights[lightIndex].m_dir = Vector(0.0f);
            renderData.m_lightData.m_punctualLights[lightIndex].m_color = (Vector)pPointLightComponent->GetLightColor() * pPointLightComponent->GetLightIntensity();
            renderData.m_lightData.m_punctualLights[lightIndex].m_spotAngles = Vector(-1.0f, 1.0f, 0.0f);
            ++lightIndex;
        }

        uint32_t numSpotLights = (uint32)std::min<size_t>(pWorldSystem->m_registeredSpotLightComponents.size(), MAX_PUNCTUAL_LIGHTS - numPointLights);
        for (uint32_t i = 0; i <  numSpotLights; ++i)
        {
            KRG_ASSERT(lightIndex < MAX_PUNCTUAL_LIGHTS);
            SpotLightComponent* pSpotLightComponent = pWorldSystem->m_registeredSpotLightComponents[i];
            renderData.m_lightData.m_punctualLights[lightIndex].m_positionInvRadiusSqr = pSpotLightComponent->GetLightPosition() - camPosition;
            renderData.m_lightData.m_punctualLights[lightIndex].m_positionInvRadiusSqr.m_w = sqr(1.0f/pSpotLightComponent->GetLightRadius());
            renderData.m_lightData.m_punctualLights[lightIndex].m_dir = -pSpotLightComponent->GetLightDirection();
            renderData.m_lightData.m_punctualLights[lightIndex].m_color = (Vector)pSpotLightComponent->GetLightColor() * pSpotLightComponent->GetLightIntensity();
            Degrees innerAngle = pSpotLightComponent->GetLightInnerUmbraAngle().ToRadians();
            Degrees outerAngle = pSpotLightComponent->GetLightOuterUmbraAngle().ToRadians();
            innerAngle.Clamp(0, 90);
            outerAngle.Clamp(0, 90);

            float cosInner = cos(innerAngle.ToRadians());
            float cosOuter = cos(outerAngle.ToRadians());
            renderData.m_lightData.m_punctualLights[lightIndex].m_spotAngles = Vector(cosOuter, 1.0f/std::max(cosInner - cosOuter, 0.001f), 0.0f);
            ++lightIndex;
        }

        renderData.m_lightData.m_lightingFlags = lightingFlags | (pWorldSystem->GetVisualizationMode() << WorldRendererSystem::VIS_MODE_BITS_SHIFT);
        renderData.m_lightData.m_numPunctualLights = lightIndex;

        RenderSunShadows(viewport, pDirectionalLightComponent, renderData);
        immediateContext.SetRenderTarget( target );
        {
            RenderStaticMeshes( viewport, renderData );
            RenderSkeletalMeshes( viewport, renderData );
        }
        RenderSkybox( viewport, renderData );
    }
}