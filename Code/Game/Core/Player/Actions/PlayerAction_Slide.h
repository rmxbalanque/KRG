#pragma once

#include "PlayerAction.h"
#include "System\Core\Time\Timers.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    class SlideAction final : public Action
    {
    public:

        KRG_PLAYER_ACTION_ID( SlideAction );

        virtual bool TryStartInternal( ActionContext const& ctx ) override;
        virtual Status UpdateInternal( ActionContext const& ctx ) override;
        virtual void StopInternal( ActionContext const& ctx, StopReason reason ) override;

    private:

        ManualTimer                 m_stickPressedTimer;
        ManualTimer                 m_slideDurationTimer;
        ManualTimer                 m_hackSettleTimer;
        Vector                      m_slideDirection = Vector::Zero;
        bool                        m_isInSettle = false;

        #if KRG_DEVELOPMENT_TOOLS
        Vector                      m_debugStartPosition = Vector::Zero;
        #endif
    };
}