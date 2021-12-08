#include "Component_PhysicsCharacter.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    void CharacterComponent::Initialize()
    {
        SpatialEntityComponent::Initialize();
        m_capsuleWorldTransform = CalculateCapsuleTransformFromWorldTransform( GetWorldTransform() );
    }

    bool CharacterComponent::HasValidPhysicsSetup() const
    {
        if ( m_radius <= 0 || m_cylinderPortionHalfHeight <= 0 )
        {
            KRG_LOG_ERROR( "Physics", "Invalid radius or half height on Physics Capsule Component: %s (%u). Negative or zero values are not allowed!", GetName().c_str(), GetID() );
            return false;
        }

        return true;
    }

    void CharacterComponent::OnWorldTransformUpdated()
    {
        m_capsuleWorldTransform = CalculateCapsuleTransformFromWorldTransform( GetWorldTransform() );
        m_linearVelocity = Vector::Zero;

        if ( m_pPhysicsActor != nullptr )
        {
            // Teleport kinematic body
            auto physicsScene = m_pPhysicsActor->getScene();
            physicsScene->lockWrite();
            auto pKinematicActor = m_pPhysicsActor->is<physx::PxRigidDynamic>();
            KRG_ASSERT( pKinematicActor->getRigidBodyFlags().isSet( physx::PxRigidBodyFlag::eKINEMATIC ) );
            pKinematicActor->setGlobalPose( ToPx( m_capsuleWorldTransform ) );
            physicsScene->unlockWrite();
        }
    }

    void CharacterComponent::MoveCharacter( Seconds const deltaTime, Transform const& newWorldTransform )
    {
        KRG_ASSERT( deltaTime > 0.0f );

        Vector const deltaTranslation = newWorldTransform.GetTranslation() - GetPosition();
        m_linearVelocity = deltaTranslation / deltaTime;

        //-------------------------------------------------------------------------

        KRG_ASSERT( m_pPhysicsActor != nullptr  );
        SetWorldTransformDirectly( newWorldTransform, false ); // Do not fire callback as we dont want to teleport the character
        m_capsuleWorldTransform = CalculateCapsuleTransformFromWorldTransform( GetWorldTransform() );

        // Request the kinematic body be moved by the physics simulation
        auto physicsScene = m_pPhysicsActor->getScene();
        physicsScene->lockWrite();
        auto pKinematicActor = m_pPhysicsActor->is<physx::PxRigidDynamic>();
        KRG_ASSERT( pKinematicActor->getRigidBodyFlags().isSet( physx::PxRigidBodyFlag::eKINEMATIC ) );
        pKinematicActor->setKinematicTarget( ToPx( m_capsuleWorldTransform ) );
        physicsScene->unlockWrite();
    }
}