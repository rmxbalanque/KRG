#pragma once

#include "../_Module/API.h"
#include "PhysicsComponent.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class KRG_ENGINE_PHYSICS_API CapsuleComponent : public PhysicsComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( CapsuleComponent );

        friend class PhysicsWorldSystem;

    public:

        CapsuleComponent();

        // The capsule radius
        inline float GetRadius() const { return m_radius; }

        // The half-height of the cylinder portion of the capsule
        inline float GetCylinderPortionHalfHeight() const { return m_cylinderPortionHalfHeight; }

        // Get the full height of the capsule
        inline float GetCapsuleHeight() const { return ( m_cylinderPortionHalfHeight + m_radius ) * 2; }

        // Get the half-height of the capsule
        inline float GetCapsuleHalfHeight() const { return ( m_cylinderPortionHalfHeight + m_radius ); }

    private:

        virtual bool HasValidPhysicsSetup() const override final;
        virtual TInlineVector<StringID, 4> GetPhysicsMaterialIDs() const override final { return { m_physicsMaterialID }; }

    protected:

        KRG_EXPOSE StringID                                 m_physicsMaterialID;

        KRG_EXPOSE float                                    m_radius = 0.5f;

        KRG_EXPOSE float                                    m_cylinderPortionHalfHeight = 1.0f;
    };
}