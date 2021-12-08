#include "PlayerActionStateMachine.h"
#include "System/Core/Logging/Log.h"

// Actions
#include "Game/Core/Player/Actions/PlayerAction_Locomotion.h"
#include "Game/Core/Player/Actions/PlayerAction_Jump.h"
#include "Game/Core/Player/Actions/PlayerOverlayAction_Shoot.h"
#include "Game/Core/Player/Actions/PlayerAction_Falling.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    ActionStateMachine::ActionStateMachine()
    {
        //-------------------------------------------------------------------------
        // Overlay Actions
        //-------------------------------------------------------------------------

        m_overlayActions.emplace_back( KRG::New<ShootOverlayAction>() );

        //-------------------------------------------------------------------------
        // Base Actions
        //-------------------------------------------------------------------------

        m_baseActions[Locomotion] = KRG::New<LocomotionAction>();
        m_baseActions[Falling] = KRG::New<FallingAction>();
        m_baseActions[Jump] = KRG::New<JumpAction>();

        //-------------------------------------------------------------------------
        // Transitions
        //-------------------------------------------------------------------------

        m_actionTransitions[Locomotion].emplace_back( Jump, Transition::Availability::Always );
        m_actionTransitions[Jump].emplace_back( Falling, Transition::Availability::OnlyOnCompleted );
        m_actionTransitions[Jump].emplace_back( Locomotion, Transition::Availability::OnlyOnCompleted );
        m_actionTransitions[Falling].emplace_back( Locomotion, Transition::Availability::OnlyOnCompleted );
    }

    ActionStateMachine::~ActionStateMachine()
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

        //-------------------------------------------------------------------------

        for ( auto pBaseAction : m_baseActions )
        {
            KRG_ASSERT( pBaseAction == nullptr );
        }

        KRG_ASSERT( m_overlayActions.empty() );
    }

    //-------------------------------------------------------------------------

    void ActionStateMachine::Update( ActionContext const& context )
    {
        KRG_ASSERT( context.IsValid() );

        //-------------------------------------------------------------------------

        auto EvaluateTransitions = [this, &context] ( Action::Status const activeStateStatus, TInlineVector<Transition, 6> const& transitions )
        {
            for ( auto const& transition : transitions )
            {
                if ( !transition.IsAvailable( activeStateStatus ) )
                {
                    continue;
                }

                if ( m_baseActions[transition.m_targetStateID]->TryStart( context ) )
                {
                    #if KRG_DEVELOPMENT_TOOLS
                    m_actionLog.emplace_back( context.m_pEntityUpdateContext->GetFrameID(), m_baseActions[m_activeBaseActionID]->GetName(), ( activeStateStatus == Action::Status::Completed ) ? LoggedStatus::ActionCompleted : LoggedStatus::ActionInterrupted );
                    m_actionLog.emplace_back( context.m_pEntityUpdateContext->GetFrameID(), m_baseActions[transition.m_targetStateID]->GetName(), LoggedStatus::ActionStarted );
                    #endif

                    // Stop the currently active state
                    Action::StopReason const stopReason = ( activeStateStatus == Action::Status::Completed ) ? Action::StopReason::ActionCompleted : Action::StopReason::TransitionFired;
                    m_baseActions[m_activeBaseActionID]->Stop( context, stopReason );
                    m_activeBaseActionID = InvalidAction;

                    //-------------------------------------------------------------------------

                    // Start the new state
                    m_activeBaseActionID = transition.m_targetStateID;
                    Action::Status const newActionStatus = m_baseActions[m_activeBaseActionID]->Update( context );
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
            Action::Status const status = m_baseActions[m_activeBaseActionID]->Update( context );

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
                m_baseActions[m_activeBaseActionID]->Stop( context, Action::StopReason::ActionCompleted );
                m_activeBaseActionID = InvalidAction;
            }
        }

        // Handle the case where the state completed and we had no valid transition and end up in an invalid state
        if ( m_activeBaseActionID == InvalidAction )
        {
            if ( !m_isFirstUpdate )
            {
                KRG_LOG_ERROR( "Player", "Ended up with no state, starting default state!" );
            }

            m_activeBaseActionID = DefaultAction;
            bool const tryStartResult = m_baseActions[m_activeBaseActionID]->TryStart( context );
            KRG_ASSERT( tryStartResult ); // The default state MUST always be able to start

            #if KRG_DEVELOPMENT_TOOLS
            m_actionLog.emplace_back( context.m_pEntityUpdateContext->GetFrameID(), m_baseActions[m_activeBaseActionID]->GetName(), LoggedStatus::ActionStarted );
            #endif
        }

        //-------------------------------------------------------------------------
        // Evaluate overlay actions
        //-------------------------------------------------------------------------

        for ( auto pOverlayAction : m_overlayActions )
        {
            // Update running actions
            if ( pOverlayAction->IsActive() )
            {
                if ( pOverlayAction->Update( context ) == Action::Status::Completed )
                {
                    pOverlayAction->Stop( context, Action::StopReason::ActionCompleted );

                    #if KRG_DEVELOPMENT_TOOLS
                    m_actionLog.emplace_back( context.m_pEntityUpdateContext->GetFrameID(), pOverlayAction->GetName(), LoggedStatus::ActionCompleted, false );
                    #endif
                }
            }
            // Try to start action
            else if ( pOverlayAction->TryStart( context ) )
            {
                Action::Status const newActionStatus = pOverlayAction->Update( context );
                KRG_ASSERT( newActionStatus == Action::Status::Running ); // Why did you instantly completed the action you just started, this is likely a mistake!

                #if KRG_DEVELOPMENT_TOOLS
                m_actionLog.emplace_back( context.m_pEntityUpdateContext->GetFrameID(), pOverlayAction->GetName(), LoggedStatus::ActionStarted, false );
                #endif
            }
        }

        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        if ( m_actionLog.size() > 500 )
        {
            m_actionLog.erase( m_actionLog.begin(), m_actionLog.end() - 100 );
        }
        #endif

        m_isFirstUpdate = false;
    }
}