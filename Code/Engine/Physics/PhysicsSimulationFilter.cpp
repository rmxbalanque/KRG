#include "PhysicsSimulationFilter.h"

//-------------------------------------------------------------------------

using namespace physx;

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    PxFilterFlags SimulationFilter::Shader( PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1, PxFilterData filterData1, PxPairFlags& pairFlags, void const* constantBlock, uint32 constantBlockSize )
    {
        // let triggers through
        if ( PxFilterObjectIsTrigger( attributes0 ) || PxFilterObjectIsTrigger( attributes1 ) )
        {
            pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
            return PxFilterFlag::eDEFAULT;
        }

        pairFlags = PxPairFlag::eCONTACT_DEFAULT;
        return PxFilterFlag::eDEFAULT;
    }
}