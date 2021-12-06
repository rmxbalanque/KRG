#include "Component_PhysicsShape.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    void PhysicsShapeComponent::OnWorldTransformUpdated()
    {
        if ( m_pPhysicsActor != nullptr && IsKinematic() )
        {
            // Request the kinematic body be moved by the physics simulation
            auto physicsScene = m_pPhysicsActor->getScene();
            physicsScene->lockWrite();
            auto pKinematicActor = m_pPhysicsActor->is<physx::PxRigidDynamic>();
            KRG_ASSERT( pKinematicActor->getRigidBodyFlags().isSet( physx::PxRigidBodyFlag::eKINEMATIC ) );
            pKinematicActor->setKinematicTarget( ToPx( GetWorldTransform() ) );
            physicsScene->unlockWrite();
        }
    }

    void PhysicsShapeComponent::TeleportTo( Transform const& newWorldTransform )
    {
        KRG_ASSERT( m_pPhysicsActor != nullptr && IsKinematic() );
        SetWorldTransformDirectly( newWorldTransform, false ); // Do not fire callback as we dont want to lock the scene twice

        // Teleport kinematic body
        auto physicsScene = m_pPhysicsActor->getScene();
        physicsScene->lockWrite();
        auto pKinematicActor = m_pPhysicsActor->is<physx::PxRigidDynamic>();
        KRG_ASSERT( pKinematicActor->getRigidBodyFlags().isSet( physx::PxRigidBodyFlag::eKINEMATIC ) );
        pKinematicActor->setGlobalPose( ToPx( GetWorldTransform() ) );
        physicsScene->unlockWrite();
    }

    void PhysicsShapeComponent::MoveTo( Transform const& newWorldTransform )
    {
        KRG_ASSERT( m_pPhysicsActor != nullptr && IsKinematic() );
        SetWorldTransform( newWorldTransform ); // The callback will update the kinematic target
    }

    void PhysicsShapeComponent::SetVelocity( Float3 newVelocity )
    {
        KRG_ASSERT( m_pPhysicsActor != nullptr && IsDynamic() );
        KRG_UNIMPLEMENTED_FUNCTION();
    }
}