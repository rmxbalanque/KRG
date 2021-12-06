#pragma once

#include "Engine/Physics/_Module/API.h"
#include "Engine/Physics/PhysX.h"
#include "Engine/Physics/PhysicsLayers.h"
#include "Engine/Core/Entity/EntitySpatialComponent.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class KRG_ENGINE_PHYSICS_API CharacterComponent : public SpatialEntityComponent
    {
        KRG_REGISTER_SINGLETON_ENTITY_COMPONENT( CharacterComponent );

        friend class PhysicsWorldSystem;

    public:

        // Character
        //-------------------------------------------------------------------------

        // The character radius
        KRG_FORCE_INLINE float GetCharacterRadius() const { return m_radius; }

        // The character total height
        KRG_FORCE_INLINE float GetCharacterHeight() const { return ( m_radius + m_cylinderPortionHalfHeight ) * 2; }

        // Capsule Info
        //-------------------------------------------------------------------------

        // The character capsule radius
        KRG_FORCE_INLINE float GetCapsuleRadius() const { return m_radius; }

        // The half-height of the cylinder portion of the character capsule
        KRG_FORCE_INLINE float GetCylinderPortionHalfHeight() const { return m_cylinderPortionHalfHeight; }

        // Get the full height of the character capsule
        KRG_FORCE_INLINE float GetCapsuleHeight() const { return ( m_cylinderPortionHalfHeight + m_radius ) * 2; }

        // Get the half-height of the character capsule
        KRG_FORCE_INLINE float GetCapsuleHalfHeight() const { return ( m_cylinderPortionHalfHeight + m_radius ); }

        // In physX the capsule's height is along the x-axis, this component apply a corrective rotation to the capsule transform to ensure that the height is along the Z axis
        KRG_FORCE_INLINE Transform const& GetCapsuleWorldTransform() const { return m_capsuleWorldTransform; }

        // Get the physics capsule world space position
        KRG_FORCE_INLINE Vector const& GetCapsulePosition() const { return m_capsuleWorldTransform.GetTranslation(); }

        // Get the physics capsule world space orientation
        KRG_FORCE_INLINE Quaternion const& GetCapsuleOrientation() const { return m_capsuleWorldTransform.GetRotation(); }

        // Get physics capsule world space forward vector
        KRG_FORCE_INLINE Vector GetCapsuleForwardVector() const { return m_capsuleWorldTransform.GetForwardVector(); }

        // Get physics capsule world space up vector
        KRG_FORCE_INLINE Vector GetCapsuleUpVector() const { return m_capsuleWorldTransform.GetUpVector(); }

        // Get physics capsule world space right vector
        KRG_FORCE_INLINE Vector GetCapsuleRightVector() const { return m_capsuleWorldTransform.GetRightVector(); }

        // Calculate the Z=height capsule transform from a given world transform
        KRG_FORCE_INLINE Transform CalculateCapsuleTransformFromWorldTransform( Transform const& worldTransform ) const
        {
            static const Transform correctiveTransform( Quaternion( EulerAngles( 0, 90, 0 ) ) );
            return correctiveTransform * worldTransform;
        }

        // Calculate the world transform from a given capsule transform (assuming capsule height = Z )
        KRG_FORCE_INLINE Transform CalculateWorldTransformFromCapsuleTransform( Transform const& physicsWorldTransform ) const
        {
            static const Transform correctiveTransform( Quaternion( EulerAngles( 0, -90, 0 ) ) );
            return correctiveTransform * physicsWorldTransform;
        }

        // Physics
        //-------------------------------------------------------------------------

        bool HasValidPhysicsSetup() const;

        // This will set the component's world transform and teleport the physics actor to the desired location
        void TeleportTo( Transform const& newWorldTransform );

        // This will set the component's world transform and request that the kinematic actor be moved to the desired location, correctly interacting with other actors on it's path.
        // Note: the actual physics actor will only be moved during the next physics simulation step
        void MoveTo( Transform const& newWorldTransform );

    protected:

        virtual void Initialize() override;

    private:

        // Update physics world position for this shape
        virtual void OnWorldTransformUpdated() override final;

    protected:

        KRG_EXPOSE StringID                                 m_physicsMaterialID;
        KRG_EXPOSE float                                    m_radius = 0.5f;
        KRG_EXPOSE float                                    m_cylinderPortionHalfHeight = 1.0f;

        // What layers does this shape belong to?
        KRG_EXPOSE TBitFlags<Layers>                        m_layers = Layers::Environment;

    private:

        physx::PxRigidActor*                                m_pPhysicsActor = nullptr;
        physx::PxShape*                                     m_pCapsuleShape = nullptr;
        Transform                                           m_capsuleWorldTransform;

        #if KRG_DEVELOPMENT_TOOLS
        String                                              m_debugName; // Keep a debug name here since the physx SDK doesnt store the name data
        #endif
    };
}