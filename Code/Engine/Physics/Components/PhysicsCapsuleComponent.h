#pragma once

#include "../_Module/API.h"
#include "PhysicsComponent.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class KRG_ENGINE_PHYSICS_API PhysicsCapsuleComponent : public PhysicsComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( PhysicsCapsuleComponent );

        friend class PhysicsWorldSystem;

    public:

        PhysicsCapsuleComponent();

    private:

        virtual bool HasValidPhysicsSetup() const override final;
        virtual TInlineVector<StringID, 4> GetPhysicsMaterialIDs() const override final { return { m_physicsMaterialID }; }

    protected:

        KRG_EXPOSE StringID                                 m_physicsMaterialID;
        KRG_EXPOSE float                                    m_capsuleHalfHeight = 1.0f;
        KRG_EXPOSE float                                    m_capsuleRadius = 0.5f;
    };
}