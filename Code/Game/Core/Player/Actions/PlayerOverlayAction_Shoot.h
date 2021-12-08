#pragma once
#include "PlayerAction.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    class ShootOverlayAction final : public OverlayAction
    {
    public:

        KRG_PLAYER_ACTION_ID( ShootOverlayAction );

        virtual bool TryStartInternal( ActionContext const& ctx ) override;
        virtual Status UpdateInternal( ActionContext const& ctx ) override;
        virtual void StopInternal( ActionContext const& ctx, StopReason reason ) override;
    };
}