#include "AIBehavior_Wander.h"
#include "Engine/Navmesh/Systems/WorldSystem_Navmesh.h"
#include "System/Core/Math/Random.h"

//-------------------------------------------------------------------------

namespace KRG::AI
{
    void WanderBehavior::StartInternal( BehaviorContext const& ctx )
    {

    }

    Behavior::Status WanderBehavior::UpdateInternal( BehaviorContext const& ctx )
    {
        // Remain in idle if there is no valid navmesh
        AABB navmeshBounds = ctx.m_pNavmeshSystem->GetNavmeshBounds( 0 );
        if ( !navmeshBounds.IsValid() )
        {
            return Status::Running;
        }

        navmeshBounds.Shrink( Vector( 0.1, 0.1, 0.0f ) );

        //-------------------------------------------------------------------------

        if ( m_moveToAction.IsRunning() )
        {
            m_moveToAction.Update( ctx );
        }

        // If we are not moving, find a new wander target and issue a move order
        if ( !m_moveToAction.IsRunning() )
        {
            Vector const boundsMin = navmeshBounds.GetMin();
            Vector const boundsMax = navmeshBounds.GetMax();
            Vector const moveGoalPosition( Math::GetRandomFloat( boundsMin.m_x, boundsMax.m_x ), Math::GetRandomFloat( boundsMin.m_y, boundsMax.m_y ), navmeshBounds.GetCenter().m_z );
            m_moveToAction.Start( ctx, moveGoalPosition );
        }

        //-------------------------------------------------------------------------

        return Status::Running;
    }

    void WanderBehavior::StopInternal( BehaviorContext const& ctx, StopReason reason )
    {

    }
}