#pragma once

#include "Game/Core/Player/Actions/PlayerAction.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    class ActionStateMachine
    {
        friend class PlayerDebugView;

        #if KRG_DEVELOPMENT_TOOLS
        enum class LoggedStatus
        {
            None = 0,
            ActionStarted,
            ActionCompleted,
            ActionInterrupted,
        };

        struct LogEntry
        {
            LogEntry( uint64 frameID, char const* pActionName, LoggedStatus status, bool isBaseAction = true )
                : m_frameID( frameID )
                , m_pActionName( pActionName )
                , m_status( status )
                , m_isBaseAction( isBaseAction )
            {
                KRG_ASSERT( status != LoggedStatus::None && pActionName != nullptr );
            }

            uint64          m_frameID;
            char const*     m_pActionName;
            LoggedStatus    m_status;
            bool            m_isBaseAction;
        };
        #endif

    public:

        // List of all the state machine states
        //-------------------------------------------------------------------------

        enum ActionID : int8
        {
            InvalidAction = -1,
            Locomotion = 0,
            Falling,
            Jump,
            Dash,

            NumActions,
            DefaultAction = Locomotion,
        };

        // Transitions in the state machine
        //-------------------------------------------------------------------------

        struct Transition
        {
            enum class Availability : uint8
            {
                Always,
                OnlyOnCompleted
            };

            Transition( ActionID stateID, Availability availability = Availability::Always )
                : m_targetStateID( stateID )
                , m_availability( availability )
            {}

            inline bool IsAvailable( Action::Status stateUpdateResult ) const
            {
                return ( m_availability == Availability::Always ) || ( stateUpdateResult == Action::Status::Completed );
            }

            ActionID            m_targetStateID;
            Availability        m_availability;
        };

    public:

        ActionStateMachine( ActionContext const& context );
        ~ActionStateMachine();

        void Update();

    private:

        ActionContext const&                                    m_actionContext;
        ActionID                                                m_activeBaseActionID = InvalidAction;

        TArray<Action*, NumActions>                             m_baseActions;
        TArray<TInlineVector<Transition, 6>, NumActions>        m_actionTransitions;
        TInlineVector<Transition, 6>                            m_highPriorityGlobalTransitions;
        TInlineVector<Transition, 6>                            m_lowPriorityGlobalTransitions;

        TInlineVector<OverlayAction*, 5>                        m_overlayActions;
        bool                                                    m_isFirstUpdate = true;

        #if KRG_DEVELOPMENT_TOOLS
        TVector<LogEntry>                                       m_actionLog;
        #endif
    };
}