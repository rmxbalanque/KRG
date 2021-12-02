#include "EntitySystem_PlayerController.h"
#include "Game/Core/Player/Actions/PlayerAction_Locomotion.h"
#include "Game/Core/Player/Actions/PlayerAction_Jump.h"
#include "Game/Core/Player/Actions/PlayerOverlayAction_Shoot.h"
#include "Game/Core/Player/Components/Component_PlayerPhysics.h"
#include "Game/Core/Player/Components/Component_MainPlayer.h"
#include "Game/Core/GameplayPhysics/GameplayPhysicsState.h"
#include "Engine/Physics/Systems/WorldSystem_Physics.h"
#include "Engine/Physics/Components/Component_PhysicsCapsule.h"
#include "Engine/Core/Components/Component_Cameras.h"
#include "Engine/Core/Components/Component_Player.h"
#include "System/Input/InputSystem.h"
#include "System/Core/Types/ScopedValue.h"

// HACK
#include "Game/Core/Player/PhysicsStates/PlayerPhysicsState_Locomotion.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    PlayerController::~PlayerController()
    {
        for ( auto pBaseAction : m_baseActions )
        {
            KRG_ASSERT( pBaseAction == nullptr );
        }

        KRG_ASSERT( m_overlayActions.empty() );
    }

    //-------------------------------------------------------------------------

    void PlayerController::Activate()
    {
        InitializeStateMachine();
    }

    void PlayerController::Deactivate()
    {
        ShutdownStateMachine();
    }

    //-------------------------------------------------------------------------

    void PlayerController::RegisterComponent( EntityComponent* pComponent )
    {
        if ( auto pSpatialComponent = TryCast<SpatialEntityComponent>( pComponent ) )
        {
            if ( pSpatialComponent->IsRootComponent() )
            {
                KRG_ASSERT( m_actionContext.m_pRootComponent == nullptr );
                m_actionContext.m_pRootComponent = pSpatialComponent;
            }
        }

        if ( auto pCapsuleComponent = TryCast<Physics::CapsuleComponent>( pComponent ) )
        {
            // TODO: handle multiple comps per player
            KRG_ASSERT( m_actionContext.m_pCapsuleComponent == nullptr );
            m_actionContext.m_pCapsuleComponent = pCapsuleComponent;
        }

        if ( auto pCameraComponent = TryCast<OrbitCameraComponent>( pComponent ) )
        {
            KRG_ASSERT( m_actionContext.m_pCameraComponent == nullptr );
            m_actionContext.m_pCameraComponent = pCameraComponent;
        }

        if ( auto pPlayerComponent = TryCast<MainPlayerComponent>( pComponent ) )
        {
            KRG_ASSERT( m_actionContext.m_pPlayerComponent == nullptr );
            m_actionContext.m_pPlayerComponent = pPlayerComponent;
        }

        if ( auto pGameplayPhysicsComponent = TryCast<PlayerPhysicsComponent>( pComponent ) )
        {
            KRG_ASSERT( m_actionContext.m_pPhysicsComponent == nullptr );
            m_actionContext.m_pPhysicsComponent = pGameplayPhysicsComponent;
            pGameplayPhysicsComponent->CreatePhysicsStates();
            KRG_ASSERT( pGameplayPhysicsComponent->GetActivePhysicsState() != nullptr );
        }

        //-------------------------------------------------------------------------

        m_actionContext.m_components.emplace_back( pComponent );
    }

    void PlayerController::UnregisterComponent( EntityComponent* pComponent )
    {
        m_actionContext.m_components.erase_first( pComponent );

        //-------------------------------------------------------------------------

        if ( auto pSpatialComponent = TryCast<SpatialEntityComponent>( pComponent ) )
        {
            if ( pSpatialComponent->IsRootComponent() && m_actionContext.m_pRootComponent == pSpatialComponent )
            {
                m_actionContext.m_pRootComponent = nullptr;
            }
        }

        if ( auto pCapsuleComponent = TryCast<Physics::CapsuleComponent>( pComponent ) )
        {
            if ( m_actionContext.m_pCapsuleComponent == pCapsuleComponent )
            {
                m_actionContext.m_pCapsuleComponent = nullptr;
            }
        }

        if ( auto pCameraComponent = TryCast<OrbitCameraComponent>( pComponent ) )
        {
            KRG_ASSERT( m_actionContext.m_pCameraComponent == pCameraComponent );
            m_actionContext.m_pCameraComponent = nullptr;
        }

        if ( auto pPlayerComponent = TryCast<MainPlayerComponent>( pComponent ) )
        {
            KRG_ASSERT( m_actionContext.m_pPlayerComponent == pPlayerComponent );
            m_actionContext.m_pPlayerComponent = nullptr;
        }

        if ( auto pGameplayPhysicsComponent = TryCast<PlayerPhysicsComponent>( pComponent ) )
        {
            KRG_ASSERT( m_actionContext.m_pPhysicsComponent == pGameplayPhysicsComponent );
            m_actionContext.m_pPhysicsComponent->DestroyPhysicsStates();
            m_actionContext.m_pPhysicsComponent = nullptr;
        }
    }

    //-------------------------------------------------------------------------

    void PlayerController::Update( EntityUpdateContext const& ctx )
    {
        TScopedGuardValue const contextGuardValue( m_actionContext.m_pEntityUpdateContext, &ctx );
        TScopedGuardValue const inputSystemGuardValue( m_actionContext.m_pInputSystem, ctx.GetSystem<Input::InputSystem>() );

        if ( !m_actionContext.IsValid() )
        {
            return;
        }

        if ( !m_actionContext.m_pPlayerComponent->IsPlayerEnabled() )
        {
            return;
        }

        //-------------------------------------------------------------------------

        // Update gameplay action
        UpdateStateMachine();
        {
            // Convert input into desired displacement
            // Prediction if required (gameplay physic state)
            // update animation controller
        }

        // Update animation component

        // Move character capsule (gameplay physic state)

        // update camera

        auto pPhysicsSystem = ctx.GetWorldSystem<Physics::PhysicsWorldSystem>();
        auto pLocomotionPhysicsState = m_actionContext.m_pPhysicsComponent->GetActivePhysicsState<PlayerLocomotionPhysicsState>();

        // Move character
        Transform const finalPosition = m_actionContext.m_pPhysicsComponent->GetActivePhysicsState()->TryMoveCapsule( pPhysicsSystem, m_actionContext.m_pCapsuleComponent, ctx.GetDeltaTime(), Quaternion::Identity, pLocomotionPhysicsState->m_deltaMovementHack );
        Transform const fromCapsuleToRoot = m_actionContext.m_pRootComponent->GetWorldTransform() * m_actionContext.m_pCapsuleComponent->GetWorldTransform().GetInverse();

        Transform newTransform = fromCapsuleToRoot * finalPosition;
        newTransform.SetRotation( pLocomotionPhysicsState->m_deltaRotationHack );

        m_actionContext.m_pRootComponent->SetWorldTransform( newTransform );
        m_actionContext.m_pCameraComponent->FinalizeCameraPosition();

        //-------------------------------------------------------------------------
        // DEBUG
        pPhysicsSystem->AcquireReadLock();
        Physics::QueryFilter filter;
        Physics::OverlapResults overlapResult;
        filter.SetLayerMask( Physics::CreateLayerMask( Physics::Layers::Environment, Physics::Layers::Characters ) );
        filter.AddIgnoredEntity( m_actionContext.m_pCapsuleComponent->GetEntityID() );
        if( pPhysicsSystem->CapsuleOverlap( m_actionContext.m_pCapsuleComponent->GetCylinderPortionHalfHeight(), m_actionContext.m_pCapsuleComponent->GetRadius(), m_actionContext.m_pCapsuleComponent->GetOrientation(), m_actionContext.m_pCapsuleComponent->GetPosition(), filter, overlapResult ) )
        {
            KRG_LOG_MESSAGE( "Physics", "Ended the frame in overlap" );
        }
        pPhysicsSystem->ReleaseReadLock();
        //-------------------------------------------------------------------------
    }

    //-------------------------------------------------------------------------
    // State Machine
    //-------------------------------------------------------------------------    

    void PlayerController::InitializeStateMachine()
    {
        //-------------------------------------------------------------------------
        // Base Actions
        //-------------------------------------------------------------------------

        m_baseActions[Locomotion] = KRG::New<LocomotionAction>();
        m_baseActions[Jump] = KRG::New<JumpAction>();

        //-------------------------------------------------------------------------

        m_actionTransitions[Locomotion].emplace_back( Jump, Transition::Availability::Always );
        m_actionTransitions[Jump].emplace_back( Locomotion, Transition::Availability::OnlyOnCompleted );

        //-------------------------------------------------------------------------
        // Overlay Actions
        //-------------------------------------------------------------------------

        m_overlayActions.emplace_back( KRG::New<ShootOverlayAction>() );

        //-------------------------------------------------------------------------
        // Initial State
        //-------------------------------------------------------------------------

        // Note: The context will be invalid at this point, so do not use it in the default state!
        m_activeBaseActionID = DefaultAction;
        m_baseActions[DefaultAction]->TryStart( m_actionContext );
    }

    void PlayerController::ShutdownStateMachine()
    {
        for ( auto& pAction : m_overlayActions )
        {
            KRG::Delete( pAction );
        }
        m_overlayActions.clear();

        for ( auto& pAction : m_baseActions )
        {
            KRG::Delete( pAction );
        }

        //-------------------------------------------------------------------------

        for ( auto i = 0; i < NumActions; i++ )
        {
            m_actionTransitions[i].clear();
        }

        m_highPriorityGlobalTransitions.clear();
        m_lowPriorityGlobalTransitions.clear();

        m_activeBaseActionID = InvalidAction;
    }

    void PlayerController::UpdateStateMachine()
    {
        auto EvaluateTransitions = [this] ( Action::Status const activeStateStatus, TInlineVector<Transition, 6> const& transitions )
        {
            for ( auto const& transition : transitions )
            {
                if ( !transition.IsAvailable( activeStateStatus ) )
                {
                    continue;
                }

                if ( m_baseActions[transition.m_targetStateID]->TryStart( m_actionContext ) )
                {
                    // Stop the currently active state
                    Action::StopReason const stopReason = ( activeStateStatus == Action::Status::Completed ) ? Action::StopReason::ActionCompleted : Action::StopReason::TransitionFired;
                    m_baseActions[m_activeBaseActionID]->Stop( m_actionContext, stopReason );
                    m_activeBaseActionID = InvalidAction;

                    // Start the new state
                    m_activeBaseActionID = transition.m_targetStateID;
                    Action::Status const newActionStatus = m_baseActions[m_activeBaseActionID]->Update( m_actionContext );
                    KRG_ASSERT( newActionStatus == Action::Status::Running ); // Why did you instantly completed the action you just started, this is likely a mistake!
                    return true;
                }
            }

            return false;
        };

        //-------------------------------------------------------------------------
        // Evaluate active base action
        //-------------------------------------------------------------------------

        if ( m_activeBaseActionID != InvalidAction )
        {
            // Evaluate the current active state
            ActionID const initialStateID = m_activeBaseActionID;
            Action::Status const status = m_baseActions[m_activeBaseActionID]->Update( m_actionContext );

            // Evaluate Transitions
            bool transitionFired = EvaluateTransitions( status, m_highPriorityGlobalTransitions );

            if ( !transitionFired )
            {
                transitionFired = EvaluateTransitions( status, m_actionTransitions[initialStateID] );
            }

            if ( !transitionFired )
            {
                transitionFired = EvaluateTransitions( status, m_lowPriorityGlobalTransitions );
            }

            // Stop the current state if it completed and we didnt fire any transition
            if ( !transitionFired && status == Action::Status::Completed )
            {
                m_baseActions[m_activeBaseActionID]->Stop( m_actionContext, Action::StopReason::ActionCompleted );
                m_activeBaseActionID = InvalidAction;
            }
        }

        // Handle the case where the state completed and we had no valid transition and end up in an invalid state

        if ( m_activeBaseActionID == InvalidAction )
        {
            KRG_LOG_ERROR( "Player", "Ended up with no state, starting default state!" );

            m_activeBaseActionID = DefaultAction;
            bool const tryStartResult = m_baseActions[m_activeBaseActionID]->TryStart( m_actionContext );
            KRG_ASSERT( tryStartResult ); // The default state MUST always be able to start
        }

        //-------------------------------------------------------------------------
        // Evaluate overlay actions
        //-------------------------------------------------------------------------

        for ( auto pOverlayAction : m_overlayActions )
        {
            // Update running actions
            if ( pOverlayAction->IsActive() )
            {
                if ( pOverlayAction->Update( m_actionContext ) == Action::Status::Completed )
                {
                    pOverlayAction->Stop( m_actionContext, Action::StopReason::ActionCompleted );
                }
            }
            // Try to start action
            else if( pOverlayAction->TryStart( m_actionContext ) )
            {
                Action::Status const newActionStatus = pOverlayAction->Update( m_actionContext );
                KRG_ASSERT( newActionStatus == Action::Status::Running ); // Why did you instantly completed the action you just started, this is likely a mistake!
            }
        }
    }
}