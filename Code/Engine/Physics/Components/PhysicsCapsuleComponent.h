#pragma once

#include "../_Module/API.h"
#include "PhysicsShapeComponent.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class KRG_ENGINE_PHYSICS_API PhysicsCapsuleComponent : public PhysicsShapeComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT;

        friend class PhysicsWorldSystem;

    protected:

        EXPOSE F32                                  m_capsuleHalfHeight = 1.0f;
        EXPOSE F32                                  m_capsuleRadius = 0.5f;
    };
}