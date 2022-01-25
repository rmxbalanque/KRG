#pragma once
#include "AIBehavior.h"
#include "Actions/AIAction_MoveTo.h"
#include "Actions/AIAction_Idle.h"

//-------------------------------------------------------------------------

namespace KRG::AI
{
    class CombatPositionBehavior : public Behavior
    {
    public:

        KRG_AI_BEHAVIOR_ID( CombatPositionBehavior );

    private:

        virtual void StartInternal( BehaviorContext const& ctx ) override;
        virtual Status UpdateInternal( BehaviorContext const& ctx ) override;
        virtual void StopInternal( BehaviorContext const& ctx, StopReason reason ) override;

    private:

        MoveToAction                m_moveToAction;
        IdleAction                  m_idleAction;
        ManualCountdownTimer        m_waitTimer;
    };
}