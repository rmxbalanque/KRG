#pragma once

#include "PlayerAction.h"
#include "System\Core\Time\Timers.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    class DashAction final : public Action
    {
    public:

        KRG_PLAYER_ACTION_ID( DashAction );

        virtual bool TryStartInternal( ActionContext const& ctx ) override;
        virtual Status UpdateInternal( ActionContext const& ctx ) override;
        virtual void StopInternal( ActionContext const& ctx, StopReason reason ) override;

    private:

        void DrawDebugUI() override;

    private:

        EngineTimer                 m_timer;
        EngineTimer                 m_settleTimer;
        Vector                      m_dashDirection = Vector::Zero;
        Vector                      m_initialVelocity = Vector::Zero;
        bool                        m_isFirstUse = true;
        bool                        m_isInSettle = false;

        Vector                      m_debugStartPosition = Vector::Zero;
    };
}