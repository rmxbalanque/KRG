#include "MeshRenderer.h"
#include "Engine/Core/Entity/Entity.h"
#include "Engine/Render/Components/StaticMeshComponent.h"
#include "Engine/Render/Components/SkeletalMeshComponent.h"
#include "Engine/Render/Shaders/EngineShaders.h"
#include "System/Render/RenderDefaultResources.h"
#include "System/Render/RenderViewport.h"
#include "System/Core/Settings/DebugSettings.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/Core/Profiling/Profiling.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    #if KRG_DEVELOPMENT_TOOLS
    static DebugSettingBool g_showStaticMeshBounds( "ShowStaticMeshBounds", "Rendering/Static Meshes", "", false );
    static DebugSettingBool g_showSkeletalMeshBounds( "ShowSkeletalMeshBounds", "Rendering/Skeletal Meshes", "", false );
    static DebugSettingBool g_showSkeletalMeshBones( "ShowSkeletalMeshBones", "Rendering/Skeletal Meshes", "", false );
    static DebugSettingBool g_showSkeletalMeshBindPoses( "ShowSkeletalMeshBindPoses", "Rendering/Skeletal Meshes", "", false );
    #endif

    //-------------------------------------------------------------------------

    bool MeshRenderer::Initialize(  RenderDevice* pRenderDevice )
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
        return m_initialized;
    }

    void MeshRenderer::Shutdown()
    {
        KRG_ASSERT( m_registeredStaticMeshComponents.IsEmpty() );

        m_pipelineStateStatic.Clear();
        m_pipelineStateSkeletal.Clear();

        if( m_inputBindingStatic.IsValid() )
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

        if( m_vertexShaderStatic.IsValid() )
        {
            m_pRenderDevice->DestroyShader( m_vertexShaderStatic );
        }

        if ( m_vertexShaderSkeletal.IsValid() )
        {
            m_pRenderDevice->DestroyShader( m_vertexShaderSkeletal );
        }

        if( m_pixelShader.IsValid() )
        {
            m_pRenderDevice->DestroyShader( m_pixelShader );
        }

        m_pRenderDevice = nullptr;
        m_initialized = false;
    }

    void MeshRenderer::ShutdownEntitySystem()
    {
        KRG_ASSERT( m_dynamicComponents.empty() && m_registeredStaticMeshComponents.IsEmpty() && m_mobilityUpdateList.empty() && m_staticComponents.empty() );
        KRG_ASSERT( m_registeredSkeletalMeshComponents.IsEmpty() && m_skeletalMeshGroups.empty() );
    }

    //-------------------------------------------------------------------------

    void MeshRenderer::RegisterComponent( Entity const* pEntity, EntityComponent* pComponent )
    {
        if ( auto pStaticMeshComponent = ComponentCast<StaticMeshComponent>( pComponent ) )
        {
            auto& registeredComponent = m_registeredStaticMeshComponents.AddRecord( pEntity->GetID() );
            registeredComponent.m_pComponent = pStaticMeshComponent;
            registeredComponent.m_mobilityChangedEventBinding = pStaticMeshComponent->OnMobilityChanged().Bind( [this] ( StaticMeshComponent* pMeshComponent ) { OnMobilityUpdated( pMeshComponent ); } );

            // Add to appropriate sub-list
            if ( pStaticMeshComponent->GetMobility() == Mobility::Dynamic )
            {
                m_dynamicComponents.push_back( pStaticMeshComponent );
            }
            else
            {
                //m_staticComponentBVH.InsertBox( pMeshComponent->GetWorldBounds().GetAABB(), pMeshComponent.GetRawPtr() );
                m_staticComponents.push_back( pStaticMeshComponent );
            }
        }
        else if ( auto pSkeletalMeshComponent = ComponentCast<SkeletalMeshComponent>( pComponent ) )
        {
            auto pRecord = m_registeredSkeletalMeshComponents[pEntity->GetID()];
            if ( pRecord == nullptr )
            {
                pRecord = &m_registeredSkeletalMeshComponents.AddRecord( pEntity->GetID() );
            }

            pRecord->m_components.emplace_back( pSkeletalMeshComponent );

            //-------------------------------------------------------------------------

            auto pMesh = pSkeletalMeshComponent->GetMesh();
            KRG_ASSERT( pMesh != nullptr && pMesh->IsValid() );

            // Add to mesh groups
            auto predicate = [] ( SkeletalMeshGroup const& group, SkeletalMesh const* pMesh ) { return group.m_pMesh == pMesh; };
            int32 const foundGroupIdx = VectorFindIndex( m_skeletalMeshGroups, pMesh, predicate );
            if ( foundGroupIdx == InvalidIndex )
            {
                auto& foundGroup = m_skeletalMeshGroups.emplace_back( SkeletalMeshGroup( pMesh ) );
                foundGroup.m_components.emplace_back( pSkeletalMeshComponent );
            }
            else
            {
                m_skeletalMeshGroups[foundGroupIdx].m_components.emplace_back( pSkeletalMeshComponent );
            }
        }
    }

    void MeshRenderer::UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent )
    {
        if ( auto pStaticMeshComponent = ComponentCast<StaticMeshComponent>( pComponent ) )
        {
            auto const pRecord = m_registeredStaticMeshComponents[pEntity->GetID()];
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
                m_registeredStaticMeshComponents.RemoveRecord( pEntity->GetID() );
            }
        }
        else if ( auto pMeshComponent = ComponentCast<SkeletalMeshComponent>( pComponent ) )
        {
            auto const pRecord = m_registeredSkeletalMeshComponents[pEntity->GetID()];
            KRG_ASSERT( pRecord != nullptr && pRecord->IsSet() );

            // Remove from record
            //-------------------------------------------------------------------------

            int32 const foundIdx = VectorFindIndex( pRecord->m_components, pMeshComponent );
            KRG_ASSERT( foundIdx != InvalidIndex );
            pRecord->m_components.erase_unsorted( pRecord->m_components.begin() + foundIdx );

            // Remove empty records
            if ( pRecord->m_components.empty() )
            {
                m_registeredSkeletalMeshComponents.RemoveRecord( pEntity->GetID() );
            }

            // Remove from mesh groups
            //-------------------------------------------------------------------------

            auto pMesh = pMeshComponent->GetMesh();

            auto predicate = [] ( SkeletalMeshGroup const& group, SkeletalMesh const* pMesh ) { return group.m_pMesh == pMesh; };
            int32 const foundGroupIdx = VectorFindIndex( m_skeletalMeshGroups, pMesh, predicate );
            KRG_ASSERT( foundGroupIdx != InvalidIndex );
            m_skeletalMeshGroups[foundGroupIdx].m_components.erase_first( pMeshComponent );

            // Remove empty groups
            if ( m_skeletalMeshGroups[foundGroupIdx].m_components.size() == 0 )
            {
                m_skeletalMeshGroups.erase_unsorted( m_skeletalMeshGroups.begin() + foundGroupIdx );
            }
        }
    }

    //-------------------------------------------------------------------------

    void MeshRenderer::UpdateEntitySystem( UpdateContext const& ctx )
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
        // Debug
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        if ( g_showStaticMeshBounds )
        {
            Debug::DrawingContext drawCtx = ctx.GetDrawingContext();
            for ( auto const& record : m_registeredStaticMeshComponents )
            {
                if ( !record.IsSet() )
                {
                    continue;
                }

                drawCtx.DrawWireBox( record.m_pComponent->GetWorldBounds(), Colors::Cyan );
                drawCtx.DrawWireBox( record.m_pComponent->GetWorldBounds().GetAABB(), Colors::LimeGreen );
            }
        }

        //-------------------------------------------------------------------------

        Debug::DrawingContext drawCtx = ctx.GetDrawingContext();
        for ( auto const& record : m_registeredSkeletalMeshComponents )
        {
            if ( !record.IsSet() )
            {
                continue;
            }

            for ( auto pMeshComponent : record.m_components )
            {
                if ( g_showSkeletalMeshBounds )
                {
                    drawCtx.DrawWireBox( pMeshComponent->GetWorldBounds(), Colors::Cyan );
                    drawCtx.DrawWireBox( pMeshComponent->GetWorldBounds().GetAABB(), Colors::LimeGreen );
                }

                if ( g_showSkeletalMeshBones )
                {
                    pMeshComponent->DrawPose( drawCtx );
                }

                if ( g_showSkeletalMeshBindPoses )
                {
                    pMeshComponent->GetMesh()->DrawBindPose( drawCtx, pMeshComponent->GetWorldTransform() );
                }
            }
        }
        #endif
    }

    void MeshRenderer::OnMobilityUpdated( StaticMeshComponent* pComponent )
    {
        KRG_ASSERT( pComponent != nullptr && pComponent->IsInitialized() );

        Threading::ScopeLock lock( m_mobilityUpdateListLock );
        KRG_ASSERT( !VectorContains( m_mobilityUpdateList, pComponent ) );
        m_mobilityUpdateList.emplace_back( pComponent );
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
                    renderContext.SetShaderResource( PipelineStage::Pixel, 0, materials[i]->GetDiffuseTexture()->GetShaderResourceView() );
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

    void MeshRenderer::RenderStatic( Viewport const& viewport )
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

        renderContext.SetPipelineState( m_pipelineStateStatic );
        renderContext.SetShaderInputBinding( m_inputBindingStatic );
        renderContext.SetPrimitiveTopology( Topology::TriangleList );

        //-------------------------------------------------------------------------

        for ( auto const& pComponent : m_staticComponents )
        //for ( auto const& pComponent : m_visibleStaticComponents )
        {
            auto pMesh = pComponent->GetMesh();
            Matrix const worldMatrix = pComponent->GetWorldTransform().ToMatrix();
            Matrix const wvp = worldMatrix * viewProjectionMatrix;
            renderContext.WriteToBuffer( m_vertexShaderStatic.GetConstBuffer( 0 ), &wvp, sizeof( Matrix ) );

            SubmitMeshDrawCall( renderContext, pMesh, pComponent->GetMaterials() );
        }
    }

    void MeshRenderer::RenderDynamic( Viewport const& viewport )
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

        renderContext.SetPipelineState( m_pipelineStateStatic );
        renderContext.SetShaderInputBinding( m_inputBindingStatic );
        renderContext.SetPrimitiveTopology( Topology::TriangleList );

        //-------------------------------------------------------------------------

        for ( auto const& pComponent : m_visibleDynamicComponents )
        {
            auto pMesh = pComponent->GetMesh();
            auto const& worldTransform = pComponent->GetWorldTransform();

            Matrix const wvp = worldTransform.ToMatrix() * viewProjectionMatrix;
            renderContext.WriteToBuffer( m_vertexShaderStatic.GetConstBuffer( 0 ), &wvp, sizeof( Matrix ) );

            SubmitMeshDrawCall( renderContext, pMesh, pComponent->GetMaterials() );
        }
    }

    void MeshRenderer::RenderSkeletal( Viewport const& viewport )
    {
        KRG_PROFILE_FUNCTION_RENDER();

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

        auto const& renderContext = m_pRenderDevice->GetImmediateContext();

        // Set primary render state and clear the render buffer
        renderContext.SetViewport( Float2( viewport.GetSize() ), Float2( viewport.GetTopLeftPosition() ) );
        renderContext.SetDepthTestMode( DepthTestMode::On );
        renderContext.SetSampler( PipelineStage::Pixel, 0, m_samplerState );

        renderContext.SetPipelineState( m_pipelineStateSkeletal );
        renderContext.SetShaderInputBinding( m_inputBindingSkeletal );
        renderContext.SetPrimitiveTopology( Topology::TriangleList );

        //-------------------------------------------------------------------------

        auto const viewProjectionMatrix = viewport.GetViewVolume().GetViewProjectionMatrix();

        for ( auto const& meshGroup : m_skeletalMeshGroups )
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

                auto const& vertexShaderConstBuffer = m_vertexShaderSkeletal.GetConstBuffer( 0 );
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

                TVector<Material const*> const& materials = pComponent->GetMaterials();

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
                        renderContext.SetShaderResource( PipelineStage::Pixel, 0, pTextureToSet->GetShaderResourceView() );
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