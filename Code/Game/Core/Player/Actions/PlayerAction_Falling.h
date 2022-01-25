#pragma once

#include "PlayerAction.h"
#include "System\Core\Time\Timers.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    class FallingAction final : public Action
    {
    public:

        KRG_PLAYER_ACTION_ID( FallingAction );

        virtual bool TryStartInternal( ActionContext const& ctx ) override;
        virtual Status UpdateInternal( ActionContext const& ctx ) override;
        virtual void StopInternal( ActionContext const& ctx, StopReason reason ) override;
    };
}