#include "WorldSystem_Navmesh.h"
#include "Engine/Navmesh/NavPower.h"
#include "Engine/Navmesh/Components/Component_Navmesh.h"
#include "Engine/Core/Entity/Entity.h"
#include "Engine/Core/Entity/EntityUpdateContext.h"
#include "System/Render/RenderViewport.h"
#include "System/Core/Profiling/Profiling.h"

#include <bfxNavigatorSpace.h>
#include <bfxMoverSpace.h>
#include "../Math/Random.h"
#include "../Threading/TaskSystem.h"

//-------------------------------------------------------------------------

namespace KRG::Navmesh
{
    void NavmeshWorldSystem::InitializeSystem( SystemRegistry const& systemRegistry )
    {
        auto pNavmeshSystem = systemRegistry.GetSystem<NavmeshSystem>();

        m_pInstance = bfx::SystemCreate( bfx::SystemParams( 2.0f, bfx::Z_UP ), &pNavmeshSystem->m_allocator );
        bfx::SetCurrentInstance( nullptr );

        bfx::RegisterPlannerSystem( m_pInstance );
        bfx::RegisterNavigatorSystem( m_pInstance );
        bfx::RegisterMoverSystem( m_pInstance );
        bfx::SystemStart( m_pInstance );

        #if KRG_DEVELOPMENT_TOOLS
        bfx::SetRenderer( m_pInstance, &m_renderer );
        #endif
    }

    void NavmeshWorldSystem::ShutdownSystem()
    {
        KRG_ASSERT( m_registeredNavmeshes.empty() );

        #if KRG_DEVELOPMENT_TOOLS
        bfx::SetRenderer( m_pInstance, nullptr );
        #endif

        bfx::SystemStop( m_pInstance );
        bfx::SystemDestroy( m_pInstance );
        m_pInstance = nullptr;
    }

    //-------------------------------------------------------------------------

    void NavmeshWorldSystem::RegisterComponent( Entity const* pEntity, EntityComponent* pComponent )
    {
        if ( auto pNavmeshComponent = TryCast<NavmeshComponent>( pComponent ) )
        {
            m_navmeshComponents.emplace_back( pNavmeshComponent );

            if ( pNavmeshComponent->m_pNavmeshData->HasNavmeshData() )
            {
                RegisterNavmesh( pNavmeshComponent );
            }
        }
    }

    void NavmeshWorldSystem::UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent )
    {
        if ( auto pNavmeshComponent = TryCast<NavmeshComponent>( pComponent ) )
        {
            if ( pNavmeshComponent->m_pNavmeshData->HasNavmeshData() )
            {
                UnregisterNavmesh( pNavmeshComponent );
            }

            m_navmeshComponents.erase_first_unsorted( pNavmeshComponent );
        }
    }

    void NavmeshWorldSystem::RegisterNavmesh( NavmeshComponent* pComponent )
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

        Transform const& componentWorldTransform = pComponent->GetWorldTransform();

        bfx::ResourceOffset offset;
        offset.m_positionOffset = ToBfx( componentWorldTransform.GetTranslation() );
        offset.m_rotationOffset = ToBfx( componentWorldTransform.GetRotation() );

        bfx::SpaceHandle space = bfx::GetDefaultSpaceHandle( m_pInstance );
        bfx::AddResource( space, pNavmesh, offset );

        // Add record
        m_registeredNavmeshes.emplace_back( RegisteredNavmesh( pComponent->GetID(), pNavmesh ) );
    }

    void NavmeshWorldSystem::UnregisterNavmesh( NavmeshComponent* pComponent )
    {
        KRG_ASSERT( pComponent != nullptr );
        for ( auto i = 0u; i < m_registeredNavmeshes.size(); i++ )
        {
            if ( pComponent->GetID() == m_registeredNavmeshes[i].m_componentID )
            {
                bfx::SpaceHandle space = bfx::GetDefaultSpaceHandle( m_pInstance );
                bfx::RemoveResource( space, m_registeredNavmeshes[i].m_pNavmesh );

                //-------------------------------------------------------------------------

                KRG::Free( m_registeredNavmeshes[i].m_pNavmesh );
                m_registeredNavmeshes.erase_unsorted( m_registeredNavmeshes.begin() + i );
                return;
            }
        }

        KRG_UNREACHABLE_CODE();
    }

    //-------------------------------------------------------------------------

    bfx::Mover* NavmeshWorldSystem::CreateMover( TaskSystem* pTaskSystem )
    {/*
        struct TestTask : public IPinnedTask
        {
            using IPinnedTask::IPinnedTask;

            virtual void Execute() override final
            {
                bfx::MoverTune m_tune;
                auto spaceHandle = bfx::GetDefaultSpaceHandle( m_pInstance );
                m_pMover = bfx::CreateMover( spaceHandle, Navmesh::ToBfx( Vector::Zero ), Navmesh::ToBfx( Quaternion::Identity ), &m_tune );
            }

            bfx::Instance* m_pInstance;
            bfx::Mover* m_pMover;
        };

        TestTask tt( 4 );
        tt.m_pInstance = m_pInstance;
        pTaskSystem->ScheduleTask( &tt );
        pTaskSystem->WaitForTask( &tt );

        return tt.m_pMover;*/

        auto spaceHandle = GetSpaceHandle();
        bfx::MoverTune m_tune;
        return bfx::CreateMover( spaceHandle, Navmesh::ToBfx( Vector::Zero ), Navmesh::ToBfx( Quaternion::Identity ), &m_tune );
    }

    void NavmeshWorldSystem::SetMoverGoal( bfx::Mover* pMover, Vector const& pos )
    {
        pMover->GotoPos( Navmesh::ToBfx( pos ) );
    }

    void NavmeshWorldSystem::DestroyMover( bfx::Mover* pMover )
    {
        KRG_ASSERT( pMover != nullptr );
        bfx::DestroyMover( pMover );
    }

    //-------------------------------------------------------------------------

    void NavmeshWorldSystem::UpdateSystem( EntityUpdateContext const& ctx )
    {
        {
            KRG_PROFILE_SCOPE_NAVIGATION( "Navmesh Simulate" );
            bfx::SystemSimulate( m_pInstance, ctx.GetDeltaTime() );
        }

        if ( m_pMover == nullptr )
        {
            m_pMover = CreateMover( ctx.GetSystem<TaskSystem>() );
        }

        if ( m_pMover != nullptr && !m_pMover->IsGotoPos() )
        {
            Vector randomPos( Math::GetRandomFloat( -40, 40 ), Math::GetRandomFloat( -40, 40 ), 0.2f );
            m_pMover->GotoPos( Navmesh::ToBfx( randomPos ) );
        }

        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        {
            KRG_PROFILE_SCOPE_NAVIGATION( "Navmesh Debug Drawing" );
            m_renderer.SetDebugDrawingSystem( ctx.GetDebugDrawingSystem() );
            m_renderer.Reset();

            Render::Viewport const* pViewport = ctx.GetViewport();
        
            bfx::DrawCullParams cullParams;
            cullParams.m_cameraPos = ToBfx( pViewport->GetViewOrigin() );
            cullParams.m_cameraDir = ToBfx( pViewport->GetViewForwardDirection() );
            cullParams.m_farClipDist = pViewport->GetViewVolume().GetDepthRange().m_end;
            cullParams.m_fov = pViewport->GetViewVolume().IsPerspective() ? pViewport->GetViewVolume().GetFOV().ToDegrees().ToFloat() : 0.0f;

            bfx::SystemDraw( m_pInstance, &cullParams );
        }
        #endif
    }
}