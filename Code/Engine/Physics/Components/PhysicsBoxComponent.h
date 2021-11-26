#pragma once

#include "../_Module/API.h"
#include "PhysicsComponent.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class KRG_ENGINE_PHYSICS_API BoxComponent : public PhysicsComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( BoxComponent );

        friend class PhysicsWorldSystem;

    public:

        BoxComponent();

    private:

        virtual bool HasValidPhysicsSetup() const override final;
        virtual TInlineVector<StringID, 4> GetPhysicsMaterialIDs() const override final { return { m_physicsMaterialID }; }

    protected:

        KRG_EXPOSE StringID                                 m_physicsMaterialID;
        KRG_EXPOSE Float3                                   m_boxExtents = Float3( 1.0f );
    };
}