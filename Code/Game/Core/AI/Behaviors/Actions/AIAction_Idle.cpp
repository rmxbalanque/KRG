#include "AIAction_Idle.h"
#include "Game/Core/AI/Behaviors/AIBehavior.h"
#include "Game/Core/AI/GraphControllers/AIGraphController_Locomotion.h"
#include "System/Core/Math/Random.h"

//-------------------------------------------------------------------------

namespace KRG::AI
{
    void IdleAction::Start( BehaviorContext const& ctx )
    {
        auto pLocomotionController = ctx.m_pAnimationController->GetSubGraphController<LocomotionGraphController>();
        pLocomotionController->SetIdle();

        ResetIdleBreakerTimer();
    }

    void IdleAction::ResetIdleBreakerTimer()
    {
        m_idleBreakerCooldown.Start( Math::GetRandomFloat( 15.0f, 25.0f ) );
    }

    void IdleAction::Update( BehaviorContext const& ctx )
    {
        if ( m_idleBreakerCooldown.IsRunning() )
        {
            if ( m_idleBreakerCooldown.Update( ctx.GetDeltaTime() ) )
            {
                // TODO: run idle breaker
            }
        }
    }
}
