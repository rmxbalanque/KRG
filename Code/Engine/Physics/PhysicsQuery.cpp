#include "PhysicsQuery.h"

//-------------------------------------------------------------------------

using namespace physx;

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    PxQueryHitType::Enum QueryFilter::preFilter( PxFilterData const& filterData, PxShape const* pShape, PxRigidActor const* pActor, PxHitFlags& queryFlags )
    {
        for ( auto const& pIgnoredActor : m_ignoredActors )
        {
            if ( pIgnoredActor == pActor )
            {
                return PxQueryHitType::eNONE;
            }
        }

        //-------------------------------------------------------------------------

        return PxQueryHitType::eBLOCK;
    }

    PxQueryHitType::Enum QueryFilter::postFilter( PxFilterData const& filterData, PxQueryHit const& hit )
    {
        KRG_UNREACHABLE_CODE(); // Not currently used
        return PxQueryHitType::eBLOCK;
    }
}