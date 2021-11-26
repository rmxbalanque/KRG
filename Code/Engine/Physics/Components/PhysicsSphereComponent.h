#pragma once

#include "../_Module/API.h"
#include "PhysicsComponent.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class KRG_ENGINE_PHYSICS_API SphereComponent : public PhysicsComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( SphereComponent );

        friend class PhysicsWorldSystem;

    public:

        SphereComponent();

    private:

        virtual bool HasValidPhysicsSetup() const override final;
        virtual TInlineVector<StringID, 4> GetPhysicsMaterialIDs() const override final { return { m_physicsMaterialID }; }

    protected:

        KRG_EXPOSE StringID                                 m_physicsMaterialID;
        KRG_EXPOSE float                                    m_radius = 0.5f;
    };
}