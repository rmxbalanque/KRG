#pragma once

#include "Game/Core/AI/Actions/AIAction.h"

//-------------------------------------------------------------------------

namespace KRG::AI
{
    class ActionController
    {
        friend class AIDebugView;

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

        // List of all the AI actions
        //-------------------------------------------------------------------------

        enum ActionID : int8
        {
            InvalidAction = -1,
            Idle = 0,
            MoveTo,

            NumActions,
            DefaultAction = Idle,
        };

    public:

        ActionController();
        ~ActionController();

        void Update( ActionContext const& context );

    private:

        ActionID                                                m_activeBaseActionID = InvalidAction;
        TArray<Action*, NumActions>                             m_baseActions;
        TInlineVector<OverlayAction*, 5>                        m_overlayActions;

        #if KRG_DEVELOPMENT_TOOLS
        TVector<LogEntry>                                       m_actionLog;
        #endif
    };
}