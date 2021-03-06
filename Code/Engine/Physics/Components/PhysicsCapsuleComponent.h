#pragma once

#include "../_Module/API.h"
#include "PhysicsComponent.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class KRG_ENGINE_PHYSICS_API PhysicsCapsuleComponent : public PhysicsComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT;

        friend class PhysicsWorldSystem;

    private:

        virtual bool HasValidPhysicsSetup() const override final;

    protected:

        EXPOSE float                                    m_capsuleHalfHeight = 1.0f;
        EXPOSE float                                    m_capsuleRadius = 0.5f;
    };
}