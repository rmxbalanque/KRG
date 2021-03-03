#include "PhysicsWorldSystem.h"
#include "PhysicsSystem.h"
#include "Components/PhysicsMeshComponent.h"
#include "Components/PhysicsCapsuleComponent.h"
#include "Components/PhysicsBoxComponent.h"
#include "System/Entity/Entity.h"

//-------------------------------------------------------------------------

using namespace physx;

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    UpdatePriorityList const PhysicsWorldSystem::PriorityList;

    //-------------------------------------------------------------------------

    PhysicsWorldSystem::PhysicsWorldSystem( PhysicsSystem& physicsSystem )
        : m_physicsSystem( physicsSystem )
        , m_pScene( physicsSystem.CreateScene() )
    {
        #if KRG_DEVELOPMENT_TOOLS
        m_pScene->SetDebugName( "Primary Scene" );
        #endif
    }

    PhysicsWorldSystem::~PhysicsWorldSystem()
    {
        m_physicsSystem.DestroyScene( m_pScene );
    }

    //-------------------------------------------------------------------------

    void PhysicsWorldSystem::RegisterComponent( Entity const* pEntity, EntityComponent* pComponent )
    {
        if ( auto pShapeComponent = ComponentCast<PhysicsShapeComponent>( pComponent ) )
        {
            RegisterShapeComponent( pEntity, pShapeComponent );
        }
    }

    void PhysicsWorldSystem::UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent )
    {
        if ( auto pShapeComponent = ComponentCast<PhysicsShapeComponent>( pComponent ) )
        {
            UnregisterShapeComponent( pEntity, pShapeComponent );
        }
    }

    //-------------------------------------------------------------------------

    void PhysicsWorldSystem::RegisterShapeComponent( Entity const* pEntity, PhysicsShapeComponent* pComponent )
    {
        KRG_ASSERT( pComponent != nullptr );

        // Add component to entity record
        //-------------------------------------------------------------------------

        EntityPhysicsRecord* pRecord = m_registeredEntities[pEntity->GetID()];
        if ( pRecord == nullptr )
        {
            pRecord = &m_registeredEntities.AddRecord( pEntity->GetID() );
        }

        pRecord->m_shapeComponents.emplace_back( pComponent );

        // Create PhysX actor
        //-------------------------------------------------------------------------

        if ( pComponent->m_pPhysicsMaterial == nullptr )
        {
            KRG_LOG_WARNING( "Physics", "No Physics Material set for component: %s (%s), no physics actors will be created!", pComponent->GetName().c_str(), pComponent->GetID().ToString().c_str() );
            return;
        }

        PxShape* pShape = CreateShape( pComponent );
        if ( pShape != nullptr )
        {
            Transform const& worldTransform = pComponent->GetWorldTransform();

            PxPhysics& physics = m_physicsSystem.GetPxPhysics();
            PxScene* pScene = m_pScene->GetPxScene();

            // Create physx actor
            PxTransform const bodyPose( ToPx( worldTransform.GetTranslation() ), ToPx( worldTransform.GetRotation() ) );
            PxRigidStatic* pStaticBody = physics.createRigidStatic( bodyPose );
            pStaticBody->attachShape( *pShape );
            pScene->addActor( *pStaticBody );
            pStaticBody->userData = pComponent;

            pComponent->m_pPhysicsActor = pStaticBody;

            // Release created temp resources
            pShape->release();
        }
    }

    void PhysicsWorldSystem::UnregisterShapeComponent( Entity const* pEntity, PhysicsShapeComponent* pComponent )
    {
        KRG_ASSERT( pComponent != nullptr );
        auto const pRecord = m_registeredEntities[pEntity->GetID()];
        KRG_ASSERT( pRecord != nullptr );

        // Destroy physics actor (if one exists)
        //-------------------------------------------------------------------------

        if ( pComponent->m_pPhysicsActor != nullptr )
        {
            pComponent->m_pPhysicsActor->release();
            pComponent->m_pPhysicsActor = nullptr;
        }

        // Remove component
        //-------------------------------------------------------------------------

        KRG_ASSERT( VectorContains( pRecord->m_shapeComponents, pComponent ) );
        pRecord->m_shapeComponents.erase_first( pComponent );

        // Remove record if empty
        //-------------------------------------------------------------------------

        if ( pRecord->IsEmpty() )
        {
            m_registeredEntities.RemoveRecord( pEntity->GetID() );
        }
    }

    physx::PxShape* PhysicsWorldSystem::CreateShape( PhysicsShapeComponent* pComponent )
    {
        KRG_ASSERT( pComponent != nullptr );
        Transform const& worldTransform = pComponent->GetWorldTransform();
        Float3 const& scale = worldTransform.GetScale();

        //-------------------------------------------------------------------------

        PxPhysics& physics = m_physicsSystem.GetPxPhysics();

        //-------------------------------------------------------------------------

        PxShape* pShape = nullptr;
        PxMaterial* pMaterial = pComponent->m_pPhysicsMaterial->GetMaterial();
        KRG_ASSERT( pMaterial != nullptr );

        if ( auto pMeshComponent = ComponentCast<PhysicsMeshComponent>( pComponent ) )
        {
            if ( pMeshComponent->m_pPhysicsMesh->GetTriangleMesh() != nullptr )
            {
                PxTriangleMeshGeometry const triMeshGeo( pMeshComponent->m_pPhysicsMesh->GetTriangleMesh(), ToPx( scale ) );
                pShape = physics.createShape( triMeshGeo, *pMaterial );
            }
            else
            {
                KRG_LOG_ERROR( "Physics", "Physics Mesh Component %s (%s) has no physics mesh set!", pComponent->GetName().c_str(), pComponent->GetID().ToString().c_str() );
            }
        }
        else if ( auto pBoxComponent = ComponentCast<PhysicsBoxComponent>( pComponent ) )
        {
            Vector const scaledBoxExtents = Vector( pBoxComponent->m_boxExtents ) * Vector( scale );
            PxBoxGeometry const boxGeo( ToPx( scaledBoxExtents ) );
            pShape = physics.createShape( boxGeo, *pMaterial );
        }
        else if ( auto pCapsuleComponent = ComponentCast<PhysicsCapsuleComponent>( pComponent ) )
        {
            // TODO: apply scale
            PxCapsuleGeometry const capsuleGeo( pCapsuleComponent->m_capsuleRadius, pCapsuleComponent->m_capsuleHalfHeight );
            pShape = physics.createShape( capsuleGeo, *pMaterial );
        }

        //-------------------------------------------------------------------------

        pShape->userData = pComponent;
        return pShape;
    }
}