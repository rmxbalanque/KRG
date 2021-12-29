#pragma once

#include "Engine/Physics/_Module/API.h"
#include "Component_PhysicsShape.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class KRG_ENGINE_PHYSICS_API CapsuleComponent : public PhysicsShapeComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( CapsuleComponent );

        friend class PhysicsWorldSystem;

    public:

        CapsuleComponent();

        // The capsule radius
        KRG_FORCE_INLINE float GetRadius() const { return m_radius; }

        // The half-height of the cylinder portion of the capsule
        KRG_FORCE_INLINE float GetCylinderPortionHalfHeight() const { return m_cylinderPortionHalfHeight; }

        // Get the full height of the capsule
        KRG_FORCE_INLINE float GetCapsuleHeight() const { return ( m_cylinderPortionHalfHeight + m_radius ) * 2; }

        // Get the half-height of the capsule
        KRG_FORCE_INLINE float GetCapsuleHalfHeight() const { return ( m_cylinderPortionHalfHeight + m_radius ); }

    private:

        virtual bool HasValidPhysicsSetup() const override final;
        virtual TInlineVector<StringID, 4> GetPhysicsMaterialIDs() const override final { return { m_materialID }; }

    protected:

        KRG_EXPOSE StringID                                 m_materialID;
        KRG_EXPOSE float                                    m_radius = 0.5f;
        KRG_EXPOSE float                                    m_cylinderPortionHalfHeight = 1.0f;
    };
}