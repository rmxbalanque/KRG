#pragma once

#include "Engine/Physics/_Module/API.h"
#include "Component_PhysicsShape.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class KRG_ENGINE_PHYSICS_API BoxComponent : public PhysicsShapeComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( BoxComponent );

        friend class PhysicsWorldSystem;

    public:

        BoxComponent();

        KRG_FORCE_INLINE Float3 const& GetExtents() const { return m_boxExtents; }

    private:

        virtual bool HasValidPhysicsSetup() const override final;
        virtual TInlineVector<StringID, 4> GetPhysicsMaterialIDs() const override final { return { m_materialID }; }

    protected:

        KRG_EXPOSE StringID                                 m_materialID;
        KRG_EXPOSE Float3                                   m_boxExtents = Float3( 1.0f );
    };
}