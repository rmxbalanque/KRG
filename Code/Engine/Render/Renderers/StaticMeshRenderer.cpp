#include "StaticMeshRenderer.h"
#include "System/Entity/Entity.h"
#include "Engine/Render/Components/StaticMeshComponent.h"
#include "Engine/Render/Shaders/EngineShaders.h"
#include "System/Render/RenderDevice/RenderDefaultResources.h"
#include "System/Core/Math/Viewport.h"
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
            #if KRG_DEVELOPMENT_TOOLS
            static DebugSettingBool g_showStaticMeshBounds( "ShowStaticMeshBounds", "Rendering/Static Meshes", "", false );
            #endif
        }

        //-------------------------------------------------------------------------

        bool StaticMeshRenderer::Initialize(  RenderDevice* pRenderDevice )
        {
            KRG_ASSERT( m_pRenderDevice == nullptr && pRenderDevice != nullptr );
            m_pRenderDevice = pRenderDevice;

            TVector<RenderBuffer> cbuffers;

            // World transform const buffer
            RenderBuffer buffer;
            buffer.m_byteSize = sizeof( Matrix );
            buffer.m_byteStride = sizeof( Matrix ); // Vector4 aligned
            buffer.m_usage = RenderBuffer::Usage::CPU_and_GPU;
            buffer.m_type = RenderBuffer::Type::Constant;
            buffer.m_slot = 0;
            cbuffers.push_back( buffer );

            // Bones const buffer
            buffer.m_byteSize = sizeof( Matrix ) * 256;
            buffer.m_byteStride = sizeof( Matrix );
            buffer.m_usage = RenderBuffer::Usage::CPU_and_GPU;
            buffer.m_type = RenderBuffer::Type::Constant;
            buffer.m_slot = 0;
            cbuffers.push_back( buffer );

            // Shaders
            auto const vertexLayoutDesc = VertexLayoutRegistry::GetDescriptorForFormat( VertexFormat::StaticMesh );
            m_vertexShader = VertexShader( g_byteCode_VS_StaticPrimitive, sizeof( g_byteCode_VS_StaticPrimitive ), cbuffers, vertexLayoutDesc );
            m_pRenderDevice->CreateShader( m_vertexShader );

            cbuffers.clear();

            m_pixelShader = PixelShader( g_byteCode_PS_Textured, sizeof( g_byteCode_PS_Textured ), cbuffers );
            m_pRenderDevice->CreateShader( m_pixelShader );

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
            m_pRenderDevice->CreateBlendState( m_blendState );

            if ( !m_blendState.IsValid() )
            {
                return false;
            }

            // Create rasterizer state
            m_rasterizerState.m_cullMode = CullMode::BackFace;
            m_rasterizerState.m_windingMode = WindingMode::CounterClockwise;
            m_rasterizerState.m_fillMode = FillMode::Solid;
            m_pRenderDevice->CreateRasterizerState( m_rasterizerState );
            if( !m_rasterizerState.IsValid() )
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

        void StaticMeshRenderer::Shutdown()
        {
            KRG_ASSERT( m_meshComponents.IsEmpty() );

            m_pipelineState.Clear();

            if( m_inputBinding.IsValid() )
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

            if( m_vertexShader.IsValid() )
            {
                m_pRenderDevice->DestroyShader( m_vertexShader );
            }

            if( m_pixelShader.IsValid() )
            {
                m_pRenderDevice->DestroyShader( m_pixelShader );
            }

            m_pRenderDevice = nullptr;
            m_initialized = false;
        }

        void StaticMeshRenderer::ShutdownEntitySystem()
        {
            KRG_ASSERT( m_dynamicComponents.empty() && m_meshComponents.IsEmpty() && m_mobilityUpdateList.empty() && m_staticComponents.empty() );
        }

        //-------------------------------------------------------------------------

        void StaticMeshRenderer::RegisterComponent( Entity const* pEntity, EntityComponent* pComponent )
        {
            auto pMeshComponent = ComponentCast<StaticMeshComponent>( pComponent );
            if ( pMeshComponent != nullptr )
            {
                auto& registeredComponent = m_meshComponents.AddRecord( pEntity->GetID() );
                registeredComponent.m_pComponent = pMeshComponent;
                registeredComponent.m_mobilityChangedEventBinding = pMeshComponent->OnMobilityChanged().Bind( [this] ( StaticMeshComponent* pMeshComponent ) { OnMobilityUpdated( pMeshComponent ); } );

                // Add to appropriate sub-list
                if ( pMeshComponent->GetMobility() == Mobility::Dynamic )
                {
                    m_dynamicComponents.push_back( pMeshComponent );
                }
                else
                {
                    //m_staticComponentBVH.InsertBox( pMeshComponent->GetWorldBounds().GetAABB(), pMeshComponent.GetRawPtr() );
                    m_staticComponents.push_back( pMeshComponent );
                }
            }
        }

        void StaticMeshRenderer::UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent )
        {
            auto const pRecord = m_meshComponents[pEntity->GetID()];
            if ( pRecord != nullptr )
            {
                KRG_ASSERT( pRecord->IsSet() );
                auto const pMesh = pRecord->m_pComponent->GetMesh();

                // Unbind mobility change handler and remove from various lists
                pRecord->m_pComponent->OnMobilityChanged().Unbind( pRecord->m_mobilityChangedEventBinding );

                // Get the real mobility of the component
                Mobility realMobility = pRecord->m_pComponent->GetMobility();
                int32 const mobilityListIdx = VectorFindIndex( m_mobilityUpdateList, pRecord->m_pComponent );
                if ( mobilityListIdx != InvalidIndex )
                {
                    realMobility = ( realMobility == Mobility::Dynamic ) ? Mobility::Static : Mobility::Dynamic;
                    m_mobilityUpdateList.erase_unsorted( m_mobilityUpdateList.begin() + mobilityListIdx );
                }

                // Remove from the relevant runtime list
                if ( realMobility == Mobility::Dynamic )
                {
                    m_dynamicComponents.erase_first_unsorted( pRecord->m_pComponent );
                }
                else
                {
                    m_staticComponents.erase_first_unsorted( pRecord->m_pComponent );
                    //m_staticComponentBVH.RemoveBox( pComponent );
                }

                // Remove record
                m_meshComponents.RemoveRecord( pEntity->GetID() );
            }
        }

        //-------------------------------------------------------------------------

        void StaticMeshRenderer::UpdateEntitySystem( UpdateContext const& ctx )
        {
            KRG_PROFILE_FUNCTION_RENDER();

            //-------------------------------------------------------------------------

            for ( auto pMeshComponent : m_mobilityUpdateList )
            {
                Mobility const mobility = pMeshComponent->GetMobility();
                if ( mobility == Mobility::Dynamic )
                {
                    auto foundIter = VectorFind( m_staticComponents, pMeshComponent );
                    KRG_ASSERT( foundIter != m_staticComponents.end() );
                    m_staticComponents.erase( foundIter );
                    //m_staticComponentBVH.RemoveBox( pMeshComponent );

                    m_dynamicComponents.push_back( pMeshComponent );
                }
                else // Make static
                {
                    auto foundIter = VectorFind( m_dynamicComponents, pMeshComponent );
                    KRG_ASSERT( foundIter != m_dynamicComponents.end() );
                    m_dynamicComponents.erase( foundIter );

                    //m_staticComponentBVH.InsertBox( pMeshComponent->GetWorldBounds().GetAABB(), pMeshComponent );
                    m_staticComponents.emplace_back( pMeshComponent );
                }
            }

            m_mobilityUpdateList.clear();

            //-------------------------------------------------------------------------

            #if KRG_DEVELOPMENT_TOOLS
            if ( Settings::g_showStaticMeshBounds )
            {
                Debug::DrawingContext drawCtx = ctx.GetDrawingContext();
                for ( auto const& record : m_meshComponents )
                {
                    if ( !record.IsSet() )
                    {
                        continue;
                    }

                    drawCtx.DrawWireBox( record.m_pComponent->GetWorldBounds(), Colors::Cyan );
                    drawCtx.DrawWireBox( record.m_pComponent->GetWorldBounds().GetAABB(), Colors::LimeGreen );
                }
            }
            #endif
        }

        void StaticMeshRenderer::OnMobilityUpdated( StaticMeshComponent* pComponent )
        {
            KRG_ASSERT( pComponent != nullptr && pComponent->IsInitialized() );

            Threading::ScopeLock lock( m_mobilityUpdateListLock );
            KRG_ASSERT( !VectorContains( m_mobilityUpdateList, pComponent ) );
            m_mobilityUpdateList.emplace_back( pComponent );
        }

        //-------------------------------------------------------------------------

        void SubmitMeshDrawCall( RenderContext const& renderContext, StaticMesh const* pMesh, TVector<TResourcePtr<Material>> const& materials )
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
                        renderContext.SetTexture( PipelineStage::Pixel, 0, *materials[i]->GetDiffuseTexture() );
                    }
                    else
                    {
                        renderContext.SetTexture( PipelineStage::Pixel, 0, *DefaultResources::GetDefaultTexture() );
                    }
                }

                auto const& subMesh = pMesh->GetSection( i );
                renderContext.DrawIndexed( subMesh.m_numIndices, subMesh.m_startIndex );
            }
        }

        void StaticMeshRenderer::RenderStatic( Math::Viewport const& viewport )
        {
            KRG_PROFILE_FUNCTION_RENDER();

            //m_staticComponentBVH.FindOverlaps( cullingBox, m_visibleStaticComponents );

            //-------------------------------------------------------------------------

            Matrix const viewProjectionMatrix = viewport.GetViewVolume().GetViewProjectionMatrix();

            auto const& renderContext = m_pRenderDevice->GetImmediateContext();

            // Set primary render state and clear the render buffer
            renderContext.SetViewport( Float2( viewport.GetSize() ), Float2( viewport.GetTopLeftPosition() ) );
            renderContext.SetDepthTestMode( DepthTestMode::On );
            renderContext.SetSampler( PipelineStage::Pixel, 0, m_samplerState );

            renderContext.SetPipelineState( m_pipelineState );
            renderContext.SetShaderInputBinding( m_inputBinding );
            renderContext.SetPrimitiveTopology( Topology::TriangleList );

            //-------------------------------------------------------------------------

            for ( auto const& pComponent : m_staticComponents )
            //for ( auto const& pComponent : m_visibleStaticComponents )
            {
                auto pMesh = pComponent->GetMesh();

                Matrix const wvp = pComponent->GetWorldTransform().ToMatrix() * viewProjectionMatrix;
                renderContext.WriteToBuffer( m_vertexShader.GetConstBuffer( 0 ), &wvp, sizeof( Matrix ) );

                SubmitMeshDrawCall( renderContext, pMesh, pComponent->GetMaterials() );
            }
        }

        void StaticMeshRenderer::RenderDynamic( Math::Viewport const& viewport )
        {
            KRG_PROFILE_FUNCTION_RENDER();

            m_visibleDynamicComponents.clear();

            for( auto pComponent : m_dynamicComponents )
            {
                //if( cullingBox.Overlaps( pComponent->GetWorldBounds() ) )
                {
                    m_visibleDynamicComponents.emplace_back( pComponent );
                }
            }

            //-------------------------------------------------------------------------

            Matrix const viewProjectionMatrix = viewport.GetViewVolume().GetViewProjectionMatrix();

            auto const& renderContext = m_pRenderDevice->GetImmediateContext();

            // Set primary render state and clear the render buffer
            renderContext.SetViewport( Float2( viewport.GetSize() ), Float2( viewport.GetTopLeftPosition() ) );
            renderContext.SetDepthTestMode( DepthTestMode::On );
            renderContext.SetSampler( PipelineStage::Pixel, 0, m_samplerState );

            renderContext.SetPipelineState( m_pipelineState );
            renderContext.SetShaderInputBinding( m_inputBinding );
            renderContext.SetPrimitiveTopology( Topology::TriangleList );

            //-------------------------------------------------------------------------

            for ( auto const& pComponent : m_visibleDynamicComponents )
            {
                auto pMesh = pComponent->GetMesh();
                auto const& worldTransform = pComponent->GetWorldTransform();

                Matrix const wvp = worldTransform.ToMatrix() * viewProjectionMatrix;
                renderContext.WriteToBuffer( m_vertexShader.GetConstBuffer( 0 ), &wvp, sizeof( Matrix ) );

                SubmitMeshDrawCall( renderContext, pMesh, pComponent->GetMaterials() );
            }
        }
    }
}