#include "EntitySystem_PlayerController.h"
#include "Game/Core/Player/GameplayStates/PlayerGameplayState_Locomotion.h"
#include "Game/Core/Player/GameplayStates/PlayerGameplayState_Jump.h"
#include "Game/Core/Player/Components/Component_PlayerGameplayPhysics.h"
#include "Game/Core/GameplayPhysics/GameplayPhysicsState.h"
#include "Engine/Physics/Systems/WorldSystem_Physics.h"
#include "Engine/Physics/Components/Component_PhysicsCapsule.h"
#include "Engine/Core/Components/Component_Cameras.h"
#include "Engine/Core/Components/Component_Player.h"
#include "System/Core/Types/ScopedValue.h"

// HACK
#include "Game/Core/Player/PhysicsStates/PlayerPhysicsState_Locomotion.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    PlayerController::PlayerController()
    {
        CreateStateMachineStates();
    }

    PlayerController::~PlayerController()
    {
        DestroyStateMachineStates();
    }

    //-------------------------------------------------------------------------

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
            pGameplayPhysicsComponent->CreatePhysicsStates();
            KRG_ASSERT( pGameplayPhysicsComponent->GetActivePhysicsState() != nullptr );
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

        auto pPhysicsSystem = ctx.GetWorldSystem<Physics::PhysicsWorldSystem>();
        if( pPhysicsSystem == nullptr  )
        {
            return;
        }

        //-------------------------------------------------------------------------

        if ( ctx.GetUpdateStage() == UpdateStage::PrePhysics )
        {
            UpdateStateMachine();
            //m_stateMachineContext.m_pCameraComponent->FinalizeCameraPosition();
        }
        else if ( ctx.GetUpdateStage() == UpdateStage::PostPhysics )
        {
            auto pPhysicsWorld = ctx.GetWorldSystem<Physics::PhysicsWorldSystem>();
            auto pLocomotionPhysicsState = m_stateMachineContext.m_pGameplayPhysicsComponent->GetActivePhysicsState<PlayerLocomotionPhysicsState>();

            // Move character
            Transform const finalPosition = m_stateMachineContext.m_pGameplayPhysicsComponent->GetActivePhysicsState()->TryMoveCapsule( pPhysicsSystem, m_stateMachineContext.m_pCapsuleComponent, Quaternion::Identity, pLocomotionPhysicsState->m_deltaMovementHack );
            Transform const fromCapsuleToRoot = m_stateMachineContext.m_pRootComponent->GetWorldTransform() * m_stateMachineContext.m_pCapsuleComponent->GetWorldTransform().GetInverse();

            Transform newTransform = fromCapsuleToRoot * finalPosition;
            newTransform.SetRotation( pLocomotionPhysicsState->m_deltaRotationHack );
            
            m_stateMachineContext.m_pRootComponent->SetWorldTransform( newTransform );

            m_stateMachineContext.m_pCameraComponent->FinalizeCameraPosition();
        }
    }

    //-------------------------------------------------------------------------
    // State Machine
    //-------------------------------------------------------------------------    

    void PlayerController::CreateStateMachineStates()
    {
        m_registeredStates[LocomotionState] = KRG::New<LocomotionGameplayState>();
        m_registeredStates[JumpState] = KRG::New<JumpGameplayState>();

        //-------------------------------------------------------------------------

        m_perStateTransitions[LocomotionState].emplace_back( JumpState, StateTransition::Availability::Always );
        m_perStateTransitions[JumpState].emplace_back( LocomotionState, StateTransition::Availability::OnlyOnCompleted );
    }

    void PlayerController::DestroyStateMachineStates()
    {
        for ( auto& pState : m_registeredStates )
        {
            KRG::Delete( pState );
        }

        m_activeStateID = InvalidState;
    }

    void PlayerController::UpdateStateMachine()
    {
        auto EvaluateTransitions = [this] ( GameplayState::Status const activeStateStatus, TInlineVector<StateTransition, 6> const& transitions )
        {
            for ( auto const& transition : transitions )
            {
                if ( !transition.IsAvailable( activeStateStatus ) )
                {
                    continue;
                }

                if ( m_registeredStates[transition.m_targetStateID]->TryStart( m_stateMachineContext ) )
                {
                    // Stop the currently active state
                    GameplayState::StopReason const stopReason = ( activeStateStatus == GameplayState::Status::Completed ) ? GameplayState::StopReason::StateCompleted : GameplayState::StopReason::TransitionFired;
                    m_registeredStates[m_activeStateID]->Stop( m_stateMachineContext, stopReason );
                    m_activeStateID = InvalidState;

                    // Start the new state
                    m_activeStateID = transition.m_targetStateID;
                    GameplayState::Status const newStateResult = m_registeredStates[m_activeStateID]->Update( m_stateMachineContext );
                    KRG_ASSERT( newStateResult == GameplayState::Status::Running ); // Why did you instantly completed the state you just started, this is likely a mistake!
                    return true;
                }
            }

            return false;
        };

        //-------------------------------------------------------------------------

        if ( m_activeStateID != InvalidState )
        {
            // Evaluate the current active state

            GameplayStateID const initialStateID = m_activeStateID;
            GameplayState::Status const status = m_registeredStates[m_activeStateID]->Update( m_stateMachineContext );

            // Evaluate Transitions
            bool transitionFired = EvaluateTransitions( status, m_globalPreStateTransitions );

            if ( !transitionFired )
            {
                transitionFired = EvaluateTransitions( status, m_perStateTransitions[initialStateID] );
            }

            if ( !transitionFired )
            {
                transitionFired = EvaluateTransitions( status, m_globalPostStateTransitions );
            }

            // Stop the current state if it completed and we didnt fire any transition
            if ( !transitionFired && status == GameplayState::Status::Completed )
            {
                m_registeredStates[m_activeStateID]->Stop( m_stateMachineContext, GameplayState::StopReason::StateCompleted );
                m_activeStateID = InvalidState;
            }
        }

        //-------------------------------------------------------------------------

        // Handle the case where the state completed and we had no valid transition and end up in an invalid state
        if ( m_activeStateID == InvalidState )
        {
            m_activeStateID = DefaultState;
            bool const tryStartResult = m_registeredStates[m_activeStateID]->TryStart( m_stateMachineContext );
            KRG_ASSERT( tryStartResult ); // The default state MUST always be able to start
        }
    }
}