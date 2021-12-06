#include "EntitySystem_PlayerController.h"
#include "Game/Core/Player/Components/Component_MainPlayer.h"
#include "Engine/Animation/Graph/AnimationGraphController.h"
#include "Engine/Physics/PhysicsStateController.h"
#include "Engine/Physics/Systems/WorldSystem_Physics.h"
#include "Engine/Physics/Components/Component_PhysicsCharacter.h"
#include "Engine/Core/Components/Component_Cameras.h"
#include "Engine/Core/Components/Component_Player.h"
#include "System/Input/InputSystem.h"
#include "System/Core/Types/ScopedValue.h"

// Actions
#include "Game/Core/Player/Actions/PlayerAction_Locomotion.h"
#include "Game/Core/Player/Actions/PlayerAction_Jump.h"
#include "Game/Core/Player/Actions/PlayerOverlayAction_Shoot.h"

// Physics States
#include "Game/Core/Player/PhysicsStates/PlayerPhysicsState_Locomotion.h"

// Anim controllers
#include "Game/Core/Player/GraphControllers/PlayerGraphController_PlayerState.h"
#include "Game/Core/Player/GraphControllers/PlayerGraphController_Locomotion.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    class PlayerPhysicsController final : public Physics::PhysicsStateController
    {
        virtual void CreatePhysicsStatesInternal() override
        {
            m_registeredStates.emplace_back( KRG::New<PlayerLocomotionPhysicsState>() );
            m_pActiveState = m_registeredStates.back();
            m_pActiveState->Activate();
        }
    };

    class PlayerAnimationControllers final : public Animation::GraphControllerRegistry
    {
        virtual void CreateControllersInternal( Animation::AnimationGraphComponent* pGraphComponent ) override
        {
            m_controllers.emplace_back( KRG::New<PlayerStateGraphController>( pGraphComponent ) );
            m_controllers.emplace_back( KRG::New<LocomotionGraphController>( pGraphComponent ) );
        }
    };

    //-------------------------------------------------------------------------

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
        // Physics States
        m_actionContext.m_pPhysicsController = KRG::New<PlayerPhysicsController>();
        m_actionContext.m_pPhysicsController->CreatePhysicsStates();

        // Animation Controllers
        m_actionContext.m_pAnimationControllerRegistry = KRG::New<PlayerAnimationControllers>();

        // Player Actions
        InitializeStateMachine();
    }

    void PlayerController::Deactivate()
    {
        ShutdownStateMachine();

        KRG::Delete( m_actionContext.m_pAnimationControllerRegistry );

        m_actionContext.m_pPhysicsController->DestroyPhysicsStates();
        KRG::Delete( m_actionContext.m_pPhysicsController );
    }

    //-------------------------------------------------------------------------

    void PlayerController::RegisterComponent( EntityComponent* pComponent )
    {
        if ( auto pCharacterComponent = TryCast<Physics::CharacterComponent>( pComponent ) )
        {
            // TODO: handle multiple comps per player
            KRG_ASSERT( m_actionContext.m_pCharacterComponent == nullptr );
            m_actionContext.m_pCharacterComponent = pCharacterComponent;
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

        if ( auto pGraphComponent = TryCast<Animation::AnimationGraphComponent>( pComponent ) )
        {
            m_actionContext.m_pAnimationControllerRegistry->CreateControllers( pGraphComponent ); 
        }

        //-------------------------------------------------------------------------

        m_actionContext.m_components.emplace_back( pComponent );
    }

    void PlayerController::UnregisterComponent( EntityComponent* pComponent )
    {
        m_actionContext.m_components.erase_first( pComponent );

        //-------------------------------------------------------------------------

        if ( auto pCharacterComponent = TryCast<Physics::CharacterComponent>( pComponent ) )
        {
            if ( m_actionContext.m_pCharacterComponent == pCharacterComponent )
            {
                m_actionContext.m_pCharacterComponent = nullptr;
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

        if ( auto pGraphComponent = TryCast<Animation::AnimationGraphComponent>( pComponent ) )
        {
            m_actionContext.m_pAnimationControllerRegistry->DestroyControllers();
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

        //-------------------------------------------------------------------------

        UpdateStateMachine();

        // Update animation and get root motion delta
        auto pLocomotionPhysicsState = m_actionContext.m_pPhysicsController->GetActivePhysicsState<PlayerLocomotionPhysicsState>();
        Transform const deltaTransform( pLocomotionPhysicsState->m_facingHack, pLocomotionPhysicsState->m_headingHack );

        // Move character
        auto pPhysicsSystem = ctx.GetWorldSystem<Physics::PhysicsWorldSystem>();
        m_actionContext.m_pPhysicsController->GetActivePhysicsState()->TryMoveCapsule( pPhysicsSystem, m_actionContext.m_pCharacterComponent, ctx.GetDeltaTime(), deltaTransform );

        // Update camera position relative to new character position
        m_actionContext.m_pCameraComponent->FinalizeCameraPosition();
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