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

        ManualCountdownTimer        m_sprintTimer;
        ManualCountdownTimer        m_crouchTimer;
        ManualTimer                 m_slideTimer;
        bool                        m_crouchToggled = false;
    };
}