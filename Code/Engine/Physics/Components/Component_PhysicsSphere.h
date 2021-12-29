#pragma once

#include "Engine/Physics/_Module/API.h"
#include "Component_PhysicsShape.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class KRG_ENGINE_PHYSICS_API SphereComponent : public PhysicsShapeComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( SphereComponent );

        friend class PhysicsWorldSystem;

    public:

        SphereComponent();

        KRG_FORCE_INLINE float GetRadius() const { return m_radius; }

    private:

        virtual bool HasValidPhysicsSetup() const override final;
        virtual TInlineVector<StringID, 4> GetPhysicsMaterialIDs() const override final { return { m_materialID }; }

    protected:

        KRG_EXPOSE StringID                                 m_materialID;
        KRG_EXPOSE float                                    m_radius = 0.5f;
    };
}