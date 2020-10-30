#include "SkeletalMeshRenderer.h"
#include "Engine/Render/Components/SkeletalMeshComponent.h"
#include "Engine/Render/Debug/MeshDebugDrawing.h"
#include "Engine/Render/Shaders/EngineShaders.h"
#include "System/Entity/Entity.h"
#include "System/Core/Math/Viewport.h"
#include "System/Render/RenderDevice/RenderDefaultResources.h"
#include "System/Core/Settings/DebugSettings.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/Core/Profiling/Profiling.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        namespace Settings
        {
            #if KRG_DEBUG_INSTRUMENTATION
            static DebugSettingBool g_showSkeletalMeshBounds( "ShowSkeletalMeshBounds", "Rendering/Skeletal Meshes", "", false );
            static DebugSettingBool g_showSkeletalMeshBones( "ShowSkeletalMeshBones", "Rendering/Skeletal Meshes", "", false );
            static DebugSettingBool g_showSkeletalMeshBindPoses( "ShowSkeletalMeshBindPoses", "Rendering/Skeletal Meshes", "", false );
            #endif
        }

        //-------------------------------------------------------------------------

        UpdatePriorityList const SkeletalMeshRenderer::PriorityList = UpdatePriorityList().EnableUpdateStage( UpdateStage::FrameEnd );

        //-------------------------------------------------------------------------

        bool SkeletalMeshRenderer::Initialize( RenderDevice* pRenderDevice )
        {
            KRG_ASSERT( m_pRenderDevice == nullptr && pRenderDevice != nullptr );
            m_pRenderDevice = pRenderDevice;

            TVector<RenderBuffer> cbuffers;

            // Vertex shader constant buffer - contains the world view projection matrix and bone transforms
            RenderBuffer buffer;
            buffer.m_byteSize = sizeof( Matrix ) * 256; // ( 1 WVP matrix + 255 bone matrices )
            buffer.m_byteStride = sizeof( Matrix ); // Vector4 aligned
            buffer.m_usage = RenderBuffer::Usage::CPU_and_GPU;
            buffer.m_type = RenderBuffer::Type::Constant;
            buffer.m_slot = 0;
            cbuffers.push_back( buffer );

            // Shaders
            auto const vertexLayoutDesc = VertexLayoutRegistry::GetDescriptorForFormat( VertexFormat::SkeletalMesh );
            m_vertexShader = VertexShader( g_byteCode_VS_SkinnedPrimitive, sizeof( g_byteCode_VS_SkinnedPrimitive ), cbuffers, vertexLayoutDesc );
            pRenderDevice->CreateShader( m_vertexShader );

            cbuffers.clear();

            m_pixelShader = PixelShader( g_byteCode_PS_Textured, sizeof( g_byteCode_PS_Textured ), cbuffers );
            pRenderDevice->CreateShader( m_pixelShader );

            if ( !( m_pixelShader.IsValid() && m_vertexShader.IsValid() ) )
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
            pRenderDevice->CreateBlendState( m_blendState );

            if ( !m_blendState.IsValid() )
            {
                return false;
            }

            // Create rasterizer state
            m_rasterizerState.m_cullMode = CullMode::BackFace;
            m_rasterizerState.m_windingMode = WindingMode::CounterClockwise;
            m_rasterizerState.m_fillMode = FillMode::Solid;
            pRenderDevice->CreateRasterizerState( m_rasterizerState );

            // Set up sampler
            m_pRenderDevice->CreateSamplerState( m_samplerState );
            if ( !m_samplerState.IsValid() )
            {
                return false;
            }

            // Set up input bindings
            m_pRenderDevice->CreateShaderInputBinding( m_vertexShader, vertexLayoutDesc, m_inputBinding );
            if ( !m_inputBinding.IsValid() )
            {
                return false;
            }

            // Set up pipeline state
            m_pipelineState.m_pVertexShader = &m_vertexShader;
            m_pipelineState.m_pPixelShader = &m_pixelShader;
            m_pipelineState.m_pBlendState = &m_blendState;
            m_pipelineState.m_pRasterizerState = &m_rasterizerState;

            m_initialized = true;
            return m_initialized;
        }

        void SkeletalMeshRenderer::Shutdown()
        {
            KRG_ASSERT( m_registeredComponents.IsEmpty() );

            m_pipelineState.Clear();

            if ( m_inputBinding.IsValid() )
            {
                m_pRenderDevice->DestroyShaderInputBinding( m_inputBinding );
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

            if ( m_vertexShader.IsValid() )
            {
                m_pRenderDevice->DestroyShader( m_vertexShader );
            }

            if ( m_pixelShader.IsValid() )
            {
                m_pRenderDevice->DestroyShader( m_pixelShader );
            }

            m_pRenderDevice = nullptr;
            m_initialized = false;
        }

        //-------------------------------------------------------------------------

        void SkeletalMeshRenderer::RegisterComponent( Entity const* pEntity, EntityComponent* pComponent )
        {
            if ( auto pMeshComponent = ComponentCast<SkeletalMeshComponent>( pComponent ) )
            {
                auto pRecord = m_registeredComponents[pEntity->GetID()];
                if ( pRecord == nullptr )
                {
                    pRecord = &m_registeredComponents.AddRecord( pEntity->GetID() );
                }

                pRecord->m_components.emplace_back( pMeshComponent );

                //-------------------------------------------------------------------------

                auto pMesh = pMeshComponent->GetMesh();
                KRG_ASSERT( pMesh != nullptr && pMesh->IsValid() );

                // Add to mesh groups
                auto predicate = [] ( MeshGroup const& group, SkeletalMesh const* pMesh ) { return group.m_pMesh == pMesh; };
                S32 const foundGroupIdx = VectorFindIndex( m_meshGroups, pMesh, predicate );
                if ( foundGroupIdx == InvalidIndex )
                {
                    auto& foundGroup = m_meshGroups.emplace_back( MeshGroup( pMesh ) );
                    foundGroup.m_components.emplace_back( pMeshComponent );
                }
                else
                {
                    m_meshGroups[foundGroupIdx].m_components.emplace_back( pMeshComponent );
                }
            }
        }

        void SkeletalMeshRenderer::UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent )
        {
            if ( auto pMeshComponent = ComponentCast<SkeletalMeshComponent>( pComponent ) )
            {
                auto const pRecord = m_registeredComponents[pEntity->GetID()];
                KRG_ASSERT( pRecord != nullptr && pRecord->IsSet() );

                // Remove from record
                //-------------------------------------------------------------------------

                S32 const foundIdx = VectorFindIndex( pRecord->m_components, pMeshComponent );
                KRG_ASSERT( foundIdx != InvalidIndex );
                pRecord->m_components.erase_unsorted( pRecord->m_components.begin() + foundIdx );

                // Remove empty records
                if ( pRecord->m_components.empty() )
                {
                    m_registeredComponents.RemoveRecord( pEntity->GetID() );
                }

                // Remove from mesh groups
                //-------------------------------------------------------------------------

                auto pMesh = pMeshComponent->GetMesh();

                auto predicate = [] ( MeshGroup const& group, SkeletalMesh const* pMesh ) { return group.m_pMesh == pMesh; };
                S32 const foundGroupIdx = VectorFindIndex( m_meshGroups, pMesh, predicate );
                KRG_ASSERT( foundGroupIdx != InvalidIndex );
                m_meshGroups[foundGroupIdx].m_components.erase_first( pMeshComponent );

                // Remove empty groups
                if ( m_meshGroups[foundGroupIdx].m_components.size() == 0 )
                {
                    VectorEraseUnsorted( m_meshGroups, foundGroupIdx );
                }
            }
        }

        //-------------------------------------------------------------------------

        void SkeletalMeshRenderer::UpdateEntitySystem( UpdateContext const& ctx )
        {
            KRG_PROFILE_GROUPED_SCOPE_COLOR( "Render", "SkeletalMeshRenderer::UpdateSystem", MP_PINK );

            #if KRG_DEBUG_INSTRUMENTATION
            Debug::DrawingContext drawCtx = ctx.GetDrawingContext();
            for ( auto const& record : m_registeredComponents )
            {
                if ( !record.IsSet() )
                {
                    continue;
                }

                for ( auto pMeshComponent : record.m_components )
                {

                    if ( Settings::g_showSkeletalMeshBounds )
                    {
                        drawCtx.DrawWireBox( pMeshComponent->GetWorldBounds(), Colors::Cyan );
                        drawCtx.DrawWireBox( pMeshComponent->GetWorldBounds().GetAABB(), Colors::LimeGreen );
                    }

                    if ( Settings::g_showSkeletalMeshBones )
                    {
                        DrawBones( drawCtx, pMeshComponent->GetMesh(), pMeshComponent->GetBoneTransforms(), pMeshComponent->GetWorldTransform() );
                    }

                    if ( Settings::g_showSkeletalMeshBindPoses )
                    {
                        DrawBindPose( drawCtx, pMeshComponent->GetMesh(), pMeshComponent->GetWorldTransform() );
                    }
                }
            }
            #endif
        }

        void SkeletalMeshRenderer::Render( Math::Viewport const& viewport )
        {
            KRG_PROFILE_GROUPED_SCOPE_COLOR( "Render", "SkeletalMeshRenderer::Render", MP_PINK );

            //AABB const cullingBox = viewport.GetCullingVolume().GetAABB();
            //for ( auto const& componentRecord : m_meshComponents )
            //{
            //    /*if ( cullingBox.Overlaps( componentRecord.m_pComponent->GetWorldBounds() ) )
            //    {
            //        m_visibleMeshComponents.emplace_back( componentRecord.m_pComponent );
            //    }*/

            //    m_visibleMeshComponents.emplace_back( componentRecord.m_pComponent );
            //}

            //-------------------------------------------------------------------------

            auto renderContext = m_pRenderDevice->GetImmediateContext();

            // Set primary render state and clear the render buffer
            renderContext.SetViewport( Float2( viewport.GetSize() ), Float2( viewport.GetTopLeftPosition() ) );
            renderContext.SetDepthTestMode( DepthTestMode::On );
            renderContext.SetSampler( PipelineStage::Pixel, 0, m_samplerState );

            renderContext.SetPipelineState( m_pipelineState );
            renderContext.SetShaderInputBinding( m_inputBinding );
            renderContext.SetPrimitiveTopology( Topology::TriangleList );

            //-------------------------------------------------------------------------

            auto const viewProjectionMatrix = viewport.GetViewVolume().GetViewProjectionMatrix();

            for ( auto const& meshGroup : m_meshGroups )
            {
                auto pMesh = meshGroup.m_pMesh;
                KRG_ASSERT( pMesh != nullptr && pMesh->IsValid() );

                renderContext.SetVertexBuffer( pMesh->GetVertexBuffer() );
                renderContext.SetIndexBuffer( pMesh->GetIndexBuffer() );

                Texture const* pLastSetTexture = nullptr;

                // Draw all instances of this mesh
                for ( auto const& pComponent : meshGroup.m_components )
                {
                    // Update GPU buffer
                    //-------------------------------------------------------------------------

                    Matrix const worldViewProjectionMatrix = pComponent->GetWorldTransform().ToMatrix() * viewProjectionMatrix;
                    auto const& boneTransforms = pComponent->GetSkinningTransforms();
                    KRG_ASSERT( boneTransforms.size() == pMesh->GetNumBones() );

                    auto const& vertexShaderConstBuffer = m_vertexShader.GetConstBuffer( 0 );
                    Matrix* pBufferData = (Matrix*) renderContext.MapBuffer( vertexShaderConstBuffer );
                    {
                        *pBufferData = worldViewProjectionMatrix;
                        memcpy( pBufferData, &worldViewProjectionMatrix, sizeof( Matrix ) );
                        pBufferData++;
                        memcpy( pBufferData, boneTransforms.data(), sizeof( Matrix ) * pMesh->GetNumBones() );
                    }
                    renderContext.UnmapBuffer( vertexShaderConstBuffer );

                    // Draw sub-meshes
                    //-------------------------------------------------------------------------

                    TVector<TResourcePtr<Material>> const& materials = pComponent->GetMaterials();

                    auto const numSubMeshes = pMesh->GetNumSections();
                    for ( auto i = 0u; i < numSubMeshes; i++ )
                    {
                        // Only set texture if needed
                        Texture const*  pTextureToSet = nullptr;

                        if ( i < materials.size() && materials[i] != nullptr )
                        {
                            pTextureToSet = materials[i]->GetDiffuseTexture();
                        }
                        else
                        {
                            pTextureToSet = DefaultResources::GetDefaultTexture();
                        }

                        if ( pTextureToSet != pLastSetTexture )
                        {
                            renderContext.SetTexture( PipelineStage::Pixel, 0, *pTextureToSet );
                            pLastSetTexture = pTextureToSet;
                        }

                        // Draw mesh
                        auto const& subMesh = pMesh->GetSection( i );
                        renderContext.DrawIndexed( subMesh.m_numIndices, subMesh.m_startIndex );
                    }
                }
            }
        }
    }
}