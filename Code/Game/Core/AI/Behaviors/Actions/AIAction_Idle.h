#pragma once
#include "System/Core/Time/Timers.h"
#include "System/Core/Math/Vector.h"

//-------------------------------------------------------------------------

namespace KRG::AI
{
    struct BehaviorContext;

    //-------------------------------------------------------------------------

    class IdleAction
    {
    public:

        void Start( BehaviorContext const& ctx );
        void Update( BehaviorContext const& ctx );

    private:

        void ResetIdleBreakerTimer();

    private:

        EngineCooldownTimer        m_idleBreakerCooldown;
    };
}