#pragma once

#include "../_Module/API.h"
#include "PhysicsComponent.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class KRG_ENGINE_PHYSICS_API PhysicsBoxComponent : public PhysicsComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( PhysicsBoxComponent );

        friend class PhysicsWorldSystem;

    public:

        PhysicsBoxComponent();

    private:

        virtual bool HasValidPhysicsSetup() const override final;
        virtual TInlineVector<StringID, 4> GetPhysicsMaterialIDs() const override final { return { m_physicsMaterialID }; }

    protected:

        KRG_EXPOSE StringID                                 m_physicsMaterialID;
        KRG_EXPOSE Float3                                   m_boxExtents = Float3( 1.0f );
    };
}