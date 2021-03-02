#pragma once

#include "../_Module/API.h"
#include "PhysicsShapeComponent.h"
#include "Engine/Physics/PhysicsMesh.h"
#include "System/Resource/ResourcePtr.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class PhysicsMesh;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_PHYSICS_API PhysicsMeshComponent : public PhysicsShapeComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT;

        friend class PhysicsWorldSystem;

    protected:

        EXPOSE TResourcePtr<PhysicsMesh>        m_pPhysicsMesh;
    };
}