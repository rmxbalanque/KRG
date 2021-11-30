#pragma once

#include "PlayerGameplayState.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    class LocomotionState final : public GameplayState
    {
    public:

        KRG_PLAYER_GAMEPLAY_STATE_ID( LocomotionState );

        virtual bool TryStart( GameplayStateContext const& ctx ) { return true; }
        virtual void Start( GameplayStateContext const& ctx );
        virtual bool Update( GameplayStateContext const& ctx );
        virtual void Stop( GameplayStateContext const& ctx );
    };
}