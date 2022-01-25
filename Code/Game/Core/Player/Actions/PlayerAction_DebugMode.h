#pragma once
#include "PlayerAction.h"
#include "System\Core\Time\Timers.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Player
{
    class DebugModeAction final : public Action
    {
    public:

        KRG_PLAYER_ACTION_ID( DebugModeAction );

        virtual bool TryStartInternal( ActionContext const& ctx ) override;
        virtual Status UpdateInternal( ActionContext const& ctx ) override;
        virtual void StopInternal( ActionContext const& ctx, StopReason reason ) override;

    private:

        ManualTimer m_HackTimer;
        ManualCountdownTimer m_CooldownTimer;
        float speed = 10.0f;
    };
}
#endif