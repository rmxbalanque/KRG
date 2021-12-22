#pragma once
#include "AIBehavior.h"
#include "Actions/AIAction_MoveTo.h"
#include "Actions/AIAction_Idle.h"

//-------------------------------------------------------------------------

namespace KRG::AI
{
    class WanderBehavior : public Behavior
    {
    public:

        KRG_AI_BEHAVIOR_ID( WanderBehavior );

    private:

        virtual void StartInternal( BehaviorContext const& ctx ) override;
        virtual Status UpdateInternal( BehaviorContext const& ctx ) override;
        virtual void StopInternal( BehaviorContext const& ctx, StopReason reason ) override;

    private:

        MoveToAction                m_moveToAction;
        IdleAction                  m_idleAction;
        EngineCooldownTimer         m_waitTimer;
    };
}