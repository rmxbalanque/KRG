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

        // Get the current linear velocity (m/s)
        KRG_FORCE_INLINE Vector const GetCharacterVelocity() const { return m_linearVelocity; }

        // Clear character velocity
        KRG_FORCE_INLINE void ClearCharacterVelocity() { m_linearVelocity = Vector::Zero; }

        // This will set the component's world transform and request that the kinematic actor be moved to the desired location, correctly interacting with other actors on it's path.
        // This function takes a delta time so that it can update the tracked velocities of the character
        // Note: the actual physics actor will only be moved during the next physics simulation step
        // Note: if you call the SetWorldTransform function on the component, it will teleport the character!!
        void MoveCharacter( Seconds deltaTime, Transform const& newWorldTransform );

        // This will set the component's world transform and teleport the physics actor to the desired location.
        // Note: This will reset the character velocity
        // Note: This is the same as directly moving the component
        void TeleportCharacter( Transform const& newWorldTransform ) { SetWorldTransform( newWorldTransform ); }

        // Capsule Info
        //-------------------------------------------------------------------------

        // The character capsule radius
        KRG_FORCE_INLINE float GetCapsuleRadius() const { return m_radius; }

        // The half-height of the cylinder portion of the character capsule
        KRG_FORCE_INLINE float GetCapsuleCylinderPortionHalfHeight() const { return m_cylinderPortionHalfHeight; }

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

    protected:

        virtual void Initialize() override;

    private:

        // Override this function to change access specifier
        // Use the MoveCharacter or TeleportCharacter functions instead!
        KRG_FORCE_INLINE void SetWorldTransform( Transform const& newTransform )
        {
            SpatialEntityComponent::SetWorldTransform( newTransform );
        }

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
        Vector                                              m_linearVelocity = Vector::Zero;

        #if KRG_DEVELOPMENT_TOOLS
        String                                              m_debugName; // Keep a debug name here since the physx SDK doesnt store the name data
        #endif
    };
}