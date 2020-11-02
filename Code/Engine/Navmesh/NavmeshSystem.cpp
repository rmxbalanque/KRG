#include "NavmeshSystem.h"
#include "Engine/Navmesh/Components/NavmeshComponent.h"
#include "System/Entity/Entity.h"
#include "System/Core/Profiling/Profiling.h"

//-------------------------------------------------------------------------

namespace KRG::Navmesh
{
    UpdatePriorityList const NavmeshSystem::PriorityList = UpdatePriorityList().EnableUpdateStage( UpdateStage::FrameEnd );

    //-------------------------------------------------------------------------

    void NavmeshSystem::Initialize()
    {
        bfx::SystemCreate( bfx::SystemParams( 1.0f, bfx::Z_UP ), &m_allocator );
        bfx::RegisterPlannerSystem();
        bfx::SystemStart();

        #if KRG_DEBUG_INSTRUMENTATION
        bfx::SetRenderer( &m_debugRenderer );
        #endif
    }

    void NavmeshSystem::Shutdown()
    {
        #if KRG_DEBUG_INSTRUMENTATION
        bfx::SetRenderer( nullptr );
        #endif

        bfx::SystemStop();
        bfx::SystemDestroy();
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
        m_debugRenderer.Reset();
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

        bfx::AddResource( pNavmesh, offset );

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
                bfx::RemoveResource( m_registeredNavmeshes[i].m_pNavmesh );

                //-------------------------------------------------------------------------

                KRG::Free( m_registeredNavmeshes[i].m_pNavmesh );
                VectorEraseUnsorted( m_registeredNavmeshes, i );
                return;
            }
        }

        KRG_UNREACHABLE_CODE();
    }
}