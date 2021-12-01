#pragma once

#include "PlayerAction.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    class LocomotionAction final : public Action
    {
    public:

        KRG_PLAYER_GAMEPLAY_ACTION_ID( LocomotionAction );

        virtual bool TryStartInternal( ActionContext const& ctx ) override { return true; }
        virtual Status UpdateInternal( ActionContext const& ctx ) override;
        virtual void StopInternal( ActionContext const& ctx, StopReason reason ) override;
    };
}