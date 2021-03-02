#pragma once

#include "../_Module/API.h"
#include "System/Entity/EntitySpatialComponent.h"
#include "Engine/Physics/PhysX.h"

//-------------------------------------------------------------------------

namespace physx
{
    class PxActor;
}

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class KRG_ENGINE_PHYSICS_API PhysicsShapeComponent : public SpatialEntityComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT;

        friend class PhysicsWorldSystem;

    private:

        physx::PxActor*                             m_pPhysicsActor = nullptr;
    };
}