#include "NavmeshSystem.h"
#include "Engine/Navmesh/Components/NavmeshComponent.h"
#include "System/Entity/Entity.h"
#include "System/Core/Profiling/Profiling.h"

//-------------------------------------------------------------------------

namespace KRG::Navmesh
{
    UpdatePriorityList const NavmeshSystem::PriorityList = UpdatePriorityList().EnableUpdateStage( UpdateStage::FrameEnd );

    //-------------------------------------------------------------------------

    #if KRG_DEBUG_INSTRUMENTATION
    void NavmeshSystem::NavmeshDebugRenderer::DrawLineList( bfx::LineSegment const* pLines, U32 numLines, bfx::Color const& color )
    {
        auto ctx = m_pDebugDrawingSystem->GetDrawingContext();
        for ( auto i = 0u; i < numLines; i++ )
        {
            bfx::LineSegment const& line = pLines[i];
            ctx.DrawLine( FromBfx( line.m_v0 ), FromBfx( line.m_v1 ), FromBfx( color ) );
        }
    }

    void NavmeshSystem::NavmeshDebugRenderer::DrawTriList( bfx::Triangle const* pTris, U32 numTris, bfx::Color const& color )
    {
        auto ctx = m_pDebugDrawingSystem->GetDrawingContext();
        for ( auto i = 0u; i < numTris; i++ )
        {
            bfx::Triangle const& tri = pTris[i];
            ctx.DrawTriangle( FromBfx( tri.m_v0 ), FromBfx( tri.m_v1 ), FromBfx( tri.m_v2 ), FromBfx( color ) );
        }
    }

    void NavmeshSystem::NavmeshDebugRenderer::DrawString( bfx::Color const& color, char const* str )
    {
        auto ctx = m_pDebugDrawingSystem->GetDrawingContext();
        ctx.DrawText2D( Float2::Zero, str, FromBfx( color ), Debug::Drawing::TextSize::Small );
    }

    void NavmeshSystem::NavmeshDebugRenderer::DrawString( bfx::Color const& color, bfx::Vector3 const& pos, char const* str )
    {
        auto ctx = m_pDebugDrawingSystem->GetDrawingContext();
        ctx.DrawText3D( FromBfx( pos ), str, FromBfx( color ), Debug::Drawing::TextSize::Small );
    }
    #endif

    //-------------------------------------------------------------------------

    void NavmeshSystem::Initialize()
    {
        NavPower::Initialize();

        // Since we use DLLs we need to set the NavPower instance in every module using BFX calls
        bfx::SetCurrentInstance( NavPower::GetNavpowerInstance() );
    }

    void NavmeshSystem::Shutdown()
    {
        bfx::SetCurrentInstance( nullptr );
        NavPower::Shutdown();
    }

    //-------------------------------------------------------------------------

    void NavmeshSystem::InitializeEntitySystem( SystemRegistry const& systemRegistry )
    {
        #if KRG_DEBUG_INSTRUMENTATION
        auto pDebugDrawingSystem = systemRegistry.GetSystem<Debug::DrawingSystem>();
        KRG_ASSERT( pDebugDrawingSystem != nullptr );
        m_debugRenderer.SetDebugDrawingSystem( pDebugDrawingSystem );
        #endif
    }

    void NavmeshSystem::ShutdownEntitySystem()
    {
        KRG_ASSERT( m_registeredNavmeshes.empty() );

        //-------------------------------------------------------------------------

        #if KRG_DEBUG_INSTRUMENTATION
        m_debugRenderer.SetDebugDrawingSystem( nullptr );
        #endif
    }

    void NavmeshSystem::UpdateEntitySystem( UpdateContext const& ctx )
    {
        bfx::SystemSimulate( ctx.GetDeltaTime() );

        #if KRG_DEBUG_INSTRUMENTATION
        bfx::SystemDraw();
        #endif
    }

    //-------------------------------------------------------------------------

    void NavmeshSystem::RegisterComponent( Entity const* pEntity, EntityComponent* pComponent )
    {
        auto pNavmeshComponent = ComponentCast<NavmeshComponent>( pComponent );
        if ( pNavmeshComponent != nullptr )
        {
            auto& registeredComponent = m_navmeshComponents.AddRecord( pEntity->GetID() );
            registeredComponent.m_pComponent = pNavmeshComponent;

            if ( pNavmeshComponent->m_pNavmeshData->HasNavmeshData() )
            {
                RegisterNavmesh( pNavmeshComponent );
            }
        }
    }

    void NavmeshSystem::UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent )
    {
        auto const pRecord = m_navmeshComponents[pEntity->GetID()];
        if ( pRecord != nullptr )
        {
            auto pNavmeshComponent = pRecord->m_pComponent;
            KRG_ASSERT( pNavmeshComponent != nullptr );

            if ( pNavmeshComponent->m_pNavmeshData->HasNavmeshData() )
            {
                UnregisterNavmesh( pNavmeshComponent );
            }

            // Remove record
            m_navmeshComponents.RemoveRecord( pEntity->GetID() );
        }
    }

    void NavmeshSystem::RegisterNavmesh( NavmeshComponent* pComponent )
    {
        KRG_ASSERT( pComponent != nullptr );

        // Copy resource
        //-------------------------------------------------------------------------
        // NavPower operates on the resource in place so we need to make a copy

        NavmeshData const* pData = pComponent->m_pNavmeshData.GetPtr();
        KRG_ASSERT( pData != nullptr && pData->HasNavmeshData() );

        size_t const requiredMemory = sizeof( char ) * pData->GetGraphImage().size();
        char* pNavmesh = (char*) KRG::Alloc( requiredMemory );
        memcpy( pNavmesh, pData->GetGraphImage().data(), requiredMemory );

        // Add resource
        //-------------------------------------------------------------------------

        auto const componentWorldTransform = pComponent->GetWorldTransform();

        bfx::ResourceOffset offset;
        offset.m_positionOffset = ToBfx( componentWorldTransform.GetTranslation() );
        offset.m_rotationOffset = ToBfx( componentWorldTransform.GetRotation() );

        //bfx::AddResource( pNavmesh, offset );

        // Add record
        m_registeredNavmeshes.emplace_back( RegisteredNavmesh( pComponent->GetID(), pNavmesh ) );
    }

    void NavmeshSystem::UnregisterNavmesh( NavmeshComponent* pComponent )
    {
        KRG_ASSERT( pComponent != nullptr );
        for ( auto i = 0u; i < m_registeredNavmeshes.size(); i++ )
        {
            if ( pComponent->GetID() == m_registeredNavmeshes[i].m_componentID )
            {
                //bfx::RemoveResource( m_registeredNavmeshes[i].m_pNavmesh );

                //-------------------------------------------------------------------------

                KRG::Free( m_registeredNavmeshes[i].m_pNavmesh );
                VectorEraseUnsorted( m_registeredNavmeshes, i );
                return;
            }
        }

        KRG_UNREACHABLE_CODE();
    }
}