#pragma once

#include "../_Module/API.h"
#include "Engine/Physics/PhysX.h"
#include "Engine/Physics/PhysicsMaterial.h"
#include "System/Entity/EntitySpatialComponent.h"
#include "System/Resource/ResourcePtr.h"

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

    protected:

        EXPOSE TResourcePtr<PhysicsMaterial>        m_pPhysicsMaterial = nullptr;

    private:

        physx::PxActor*                             m_pPhysicsActor = nullptr;
    };
}