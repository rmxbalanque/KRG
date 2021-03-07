#include "PhysicsQueryFilter.h"

//-------------------------------------------------------------------------

using namespace physx;

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    PxQueryHitType::Enum QueryFilter::preFilter( PxFilterData const& filterData, PxShape const* shape, PxRigidActor const* actor, PxHitFlags& queryFlags )
    {
        return PxQueryHitType::eBLOCK;
    }

    PxQueryHitType::Enum QueryFilter::postFilter( PxFilterData const& filterData, PxQueryHit const& hit )
    {
        KRG_UNREACHABLE_CODE(); // Not currently used
        return PxQueryHitType::eBLOCK;
    }
}