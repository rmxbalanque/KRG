#pragma once

#include "PlayerGameplayState.h"
#include "System/Core/Time/Timers.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    class JumpGameplayState final : public GameplayState
    {
    public:

        KRG_PLAYER_GAMEPLAY_STATE_ID( JumpGameplayState );

        virtual bool TryStart( GameplayStateContext const& ctx ) override;
        virtual Status Update( GameplayStateContext const& ctx ) override;
        virtual void Stop( GameplayStateContext const& ctx, StopReason reason ) override;

    private:

        EngineTimer m_timer;
    };
}