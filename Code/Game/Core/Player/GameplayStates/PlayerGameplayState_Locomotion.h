#pragma once

#include "PlayerGameplayState.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    class LocomotionGameplayState final : public GameplayState
    {
    public:

        KRG_PLAYER_GAMEPLAY_STATE_ID( LocomotionGameplayState );

        virtual bool TryStart( GameplayStateContext const& ctx ) override { return true; }
        virtual Status Update( GameplayStateContext const& ctx ) override;
        virtual void Stop( GameplayStateContext const& ctx, StopReason reason ) override;
    };
}