#include "AIBehavior_Wander.h"

//-------------------------------------------------------------------------

namespace KRG::AI
{
    void WanderBehavior::StartInternal( BehaviorContext const& ctx )
    {

    }

    Behavior::Status WanderBehavior::UpdateInternal( BehaviorContext const& ctx )
    {
        return Status::Running;
    }

    void WanderBehavior::StopInternal( BehaviorContext const& ctx, StopReason reason )
    {

    }
}