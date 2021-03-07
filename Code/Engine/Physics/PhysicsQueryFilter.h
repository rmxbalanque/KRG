#pragma once

#include "PhysX.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class QueryFilter final : public physx::PxQueryFilterCallback
    {
    private:

        virtual physx::PxQueryHitType::Enum preFilter( physx::PxFilterData const& filterData, physx::PxShape const* shape, physx::PxRigidActor const* actor, physx::PxHitFlags& queryFlags ) override;
        virtual physx::PxQueryHitType::Enum postFilter( physx::PxFilterData const& filterData, physx::PxQueryHit const& hit ) override;
    };
}