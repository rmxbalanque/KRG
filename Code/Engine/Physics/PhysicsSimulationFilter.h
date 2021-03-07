#pragma once

#include "PhysX.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class SimulationFilter
    {
    public:

        static physx::PxFilterFlags Shader( physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0, physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1, physx::PxPairFlags& pairFlags, void const* constantBlock, uint32 constantBlockSize );
    };
}