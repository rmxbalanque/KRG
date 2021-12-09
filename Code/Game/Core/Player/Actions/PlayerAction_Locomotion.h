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

        EngineTimer                 m_timer;
        bool                        m_timerStarted = false;

        Radians                     m_maxAngularSpeed = Radians( Degrees( 90 ) );   // radians
        float                       m_maxSprintLinearSpeed = 6.5f;                 // meter/seconds
        float                       m_maxWalkLinearSpeed = 5.0f;                   // meters/seconds
        Seconds                     m_sprintThreshold = 1.0f;                       // seconds
        float                       m_sprintStickAmplitude = 0.8f;                  // [0,1]
        float                       m_sprintSpeedThreshold = 10.0f;                 // meters/seconds
    };
}