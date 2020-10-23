#include "PhysicsSystem.h"
#include "System/Physics/PhysicsWorld.h"
#include "System/Physics/PhysX.h"
#include "Engine/Physics/Components/PhysicsGeometryComponent.h"
#include "System/Entity/Entity.h"

//-------------------------------------------------------------------------

using namespace physx;

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Physics
    {
        UpdatePriorityList const PhysicsSystem::PriorityList = UpdatePriorityList();

        //-------------------------------------------------------------------------

        void PhysicsSystem::RegisterComponent( Entity const* pEntity, EntityComponent* pComponent )
        {
            auto pGeometryComponent = ComponentCast<PhysicsGeometryComponent>( pComponent );
            if ( pGeometryComponent != nullptr )
            {
                auto& registeredComponent = m_geometryComponents.AddRecord( pEntity->GetID() );
                registeredComponent.m_pComponent = pGeometryComponent;

                // Create physics actor
                if ( pGeometryComponent->m_pPhysicsGeometry->GetTriangleMesh() != nullptr )
                {
                    auto pPhysics = PhysicsWorld::GetPhysics();
                    auto pScene = PhysicsWorld::GetScene();
                    auto pMaterial = pPhysics->createMaterial( 0.5f, 0.5f, 0.6f );

                    Transform const& worldTransform = pGeometryComponent->GetWorldTransform();

                    // Create shape
                    PxTriangleMeshGeometry triMeshGeo( pGeometryComponent->m_pPhysicsGeometry->GetTriangleMesh() );
                    triMeshGeo.scale = ToPx( worldTransform.GetScale() );
                    PxShape* pShape = pPhysics->createShape( triMeshGeo, *pMaterial );
                    if ( pShape != nullptr )
                    {
                        // Create physx actor
                        PxTransform const bodyPose( ToPx( worldTransform.GetTranslation() ), ToPx( worldTransform.GetRotation() ) );
                        PxRigidStatic* pStaticBody = pPhysics->createRigidStatic( bodyPose );
                        pStaticBody->attachShape( *pShape );
                        pScene->addActor( *pStaticBody );
                        pGeometryComponent->m_pPhysicsActor = pStaticBody;

                        // Release created temp resources
                        pShape->release();
                        pMaterial->release();
                    }
                }
            }
        }

        void PhysicsSystem::UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent )
        {
            auto const pRecord = m_geometryComponents[pEntity->GetID()];
            if ( pRecord != nullptr )
            {
                // Destroy physics actor
                auto pGeometryComponent = pRecord->m_pComponent;
                KRG_ASSERT( pGeometryComponent != nullptr );

                if ( pGeometryComponent->m_pPhysicsActor != nullptr )
                {
                    pGeometryComponent->m_pPhysicsActor->release();
                    pGeometryComponent->m_pPhysicsActor = nullptr;
                }

                // Remove record
                m_geometryComponents.RemoveRecord( pEntity->GetID() );
            }
        }
    }
}