#include "EntitySystem_PlayerController.h"
#include "Game/Core/Player/Components/Component_PlayerGameplayPhysics.h"
#include "Game/Core/GameplayPhysics/GameplayPhysicsState.h"
#include "Engine/Physics/Systems/WorldSystem_Physics.h"
#include "Engine/Physics/Components/Component_PhysicsCapsule.h"
#include "Engine/Core/Components/Component_Cameras.h"
#include "Engine/Core/Components/Component_Player.h"
#include "Engine/Core/Entity/EntityUpdateContext.h"
#include "System/Input/InputSystem.h"
#include "System/Core/Types/ScopedValue.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    PlayerController::PlayerController()
    {
        m_stateMachine.Initialize();
    }

    PlayerController::~PlayerController()
    {
        m_stateMachine.Shutdown();
    }

    void PlayerController::RegisterComponent( EntityComponent* pComponent )
    {
        if ( auto pSpatialComponent = TryCast<SpatialEntityComponent>( pComponent ) )
        {
            if ( pSpatialComponent->IsRootComponent() )
            {
                KRG_ASSERT( m_stateMachineContext.m_pRootComponent == nullptr );
                m_stateMachineContext.m_pRootComponent = pSpatialComponent;
            }
        }

        if ( auto pCapsuleComponent = TryCast<Physics::CapsuleComponent>( pComponent ) )
        {
            // TODO: handle multiple comps per player
            KRG_ASSERT( m_stateMachineContext.m_pCapsuleComponent == nullptr );
            m_stateMachineContext.m_pCapsuleComponent = pCapsuleComponent;
        }

        if ( auto pCameraComponent = TryCast<OrbitCameraComponent>( pComponent ) )
        {
            KRG_ASSERT( m_stateMachineContext.m_pCameraComponent == nullptr );
            m_stateMachineContext.m_pCameraComponent = pCameraComponent;
        }

        if ( auto pPlayerComponent = TryCast<PlayerComponent>( pComponent ) )
        {
            KRG_ASSERT( m_stateMachineContext.m_pPlayerComponent == nullptr );
            m_stateMachineContext.m_pPlayerComponent = pPlayerComponent;
        }

        if ( auto pGameplayPhysicsComponent = TryCast<PlayerGameplayPhysicsComponent>( pComponent ) )
        {
            KRG_ASSERT( m_stateMachineContext.m_pGameplayPhysicsComponent == nullptr );
            m_stateMachineContext.m_pGameplayPhysicsComponent = pGameplayPhysicsComponent;
            KRG_ASSERT( !m_stateMachineContext.m_pGameplayPhysicsComponent->HasCreatedPhysicsStates() );
            pGameplayPhysicsComponent->CreatePhysicsStates();
        }

        //-------------------------------------------------------------------------

        m_stateMachineContext.m_components.emplace_back( pComponent );
    }

    void PlayerController::UnregisterComponent( EntityComponent* pComponent )
    {
        m_stateMachineContext.m_components.erase_first( pComponent );

        //-------------------------------------------------------------------------

        if ( auto pSpatialComponent = TryCast<SpatialEntityComponent>( pComponent ) )
        {
            if ( pSpatialComponent->IsRootComponent() && m_stateMachineContext.m_pRootComponent == pSpatialComponent )
            {
                m_stateMachineContext.m_pRootComponent = nullptr;
            }
        }

        if ( auto pCapsuleComponent = TryCast<Physics::CapsuleComponent>( pComponent ) )
        {
            if ( m_stateMachineContext.m_pCapsuleComponent == pCapsuleComponent )
            {
                m_stateMachineContext.m_pCapsuleComponent = nullptr;
            }
        }

        if ( auto pCameraComponent = TryCast<OrbitCameraComponent>( pComponent ) )
        {
            KRG_ASSERT( m_stateMachineContext.m_pCameraComponent == pCameraComponent );
            m_stateMachineContext.m_pCameraComponent = nullptr;
        }

        if ( auto pPlayerComponent = TryCast<PlayerComponent>( pComponent ) )
        {
            KRG_ASSERT( m_stateMachineContext.m_pPlayerComponent == pPlayerComponent );
            m_stateMachineContext.m_pPlayerComponent = nullptr;
        }

        if ( auto pGameplayPhysicsComponent = TryCast<PlayerGameplayPhysicsComponent>( pComponent ) )
        {
            KRG_ASSERT( m_stateMachineContext.m_pGameplayPhysicsComponent == pGameplayPhysicsComponent );
            m_stateMachineContext.m_pGameplayPhysicsComponent->DestroyPhysicsStates();
            m_stateMachineContext.m_pGameplayPhysicsComponent = nullptr;
        }
    }

    //-------------------------------------------------------------------------

    void PlayerController::Update( EntityUpdateContext const& ctx )
    {
        TScopedGuardValue const contextGuardValue( m_stateMachineContext.m_pEntityUpdateContext, &ctx );

        if ( !m_stateMachineContext.IsValid() )
        {
            return;
        }

        if ( !m_stateMachineContext.m_pPlayerComponent->IsPlayerEnabled() )
        {
            return;
        }

        //-------------------------------------------------------------------------

        if ( ctx.GetUpdateStage() == UpdateStage::PrePhysics )
        {
            m_stateMachine.Update( m_stateMachineContext );
            m_stateMachineContext.m_pCameraComponent->FinalizeCameraPosition();
        }
        else if ( ctx.GetUpdateStage() == UpdateStage::PostPhysics )
        {
            auto pPhysicsWorld = ctx.GetWorldSystem<Physics::PhysicsWorldSystem>();

            // Move character
            Transform const finalPosition = m_stateMachineContext.m_pGameplayPhysicsComponent->GetActivePhysicsState()->TryMoveCapsule( pPhysicsWorld, m_stateMachineContext.m_pCapsuleComponent, Quaternion::Identity, Vector::Zero );
            Transform const fromCapsuleToRoot = m_stateMachineContext.m_pRootComponent->GetWorldTransform() * m_stateMachineContext.m_pCapsuleComponent->GetWorldTransform().GetInverse();
            m_stateMachineContext.m_pRootComponent->SetWorldTransform( fromCapsuleToRoot * finalPosition );
        }
    }
}