#pragma once

#include "PlayerAction.h"
#include "System\Core\Time\Timers.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    class LocomotionAction final : public Action
    {
    public:

        KRG_PLAYER_ACTION_ID( LocomotionAction );

        virtual bool TryStartInternal( ActionContext const& ctx ) override;
        virtual Status UpdateInternal( ActionContext const& ctx ) override;
        virtual void StopInternal( ActionContext const& ctx, StopReason reason ) override;

    private:

        virtual void DrawDebugUI() override;

    private:

        EngineTimer                 m_timer;
        bool                        m_timerStarted = false;

        float                       m_debugSpeed = 0.0f;
    };
}