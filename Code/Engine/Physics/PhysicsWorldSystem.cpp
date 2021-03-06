#include "PhysicsWorldSystem.h"
#include "PhysicsSystem.h"
#include "Components/PhysicsMeshComponent.h"
#include "Components/PhysicsCapsuleComponent.h"
#include "Components/PhysicsSphereComponent.h"
#include "Components/PhysicsBoxComponent.h"
#include "System/Entity/Entity.h"

//-------------------------------------------------------------------------

using namespace physx;

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    UpdatePriorityList const PhysicsWorldSystem::PriorityList = UpdatePriorityList().EnableUpdateStage( UpdateStage::PostPhysics );

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
        if ( auto pPhysicsComponent = ComponentCast<PhysicsComponent>( pComponent ) )
        {
            // Add component to entity record
            //-------------------------------------------------------------------------

            EntityPhysicsRecord* pRecord = m_registeredEntities[pEntity->GetID()];
            if ( pRecord == nullptr )
            {
                pRecord = &m_registeredEntities.AddRecord( pEntity->GetID() );
            }

            pRecord->m_components.emplace_back( pPhysicsComponent );

            //-------------------------------------------------------------------------

            if ( !pPhysicsComponent->HasValidPhysicsSetup() )
            {
                KRG_LOG_ERROR( "Physics", "No Physics Material set for component: %s (%s), no physics actors will be created!", pPhysicsComponent->GetName().c_str(), pPhysicsComponent->GetID().ToString().c_str() );
                return;
            }

            #if KRG_DEVELOPMENT_TOOLS
            pPhysicsComponent->m_debugName = pPhysicsComponent->GetName().c_str();
            #endif

            // Create PhysX actor and shapes
            //-------------------------------------------------------------------------
            {
                SystemScopeLock const systemLock( &m_physicsSystem );

                PxRigidActor*pPhysicsActor = CreateActor( pPhysicsComponent );
                if ( pPhysicsActor == nullptr )
                {
                    KRG_LOG_ERROR( "Physics", "Failed to create physics actor for component %s (%s)!", pPhysicsComponent->GetName().c_str(), pPhysicsComponent->GetID().ToString().c_str() );
                    return;
                }

                PxShape* pShape = CreateShape( pPhysicsComponent, pPhysicsActor );
                if ( pShape == nullptr )
                {
                    KRG_LOG_ERROR( "Physics", "Failed to create physics shape for component %s (%s)!", pPhysicsComponent->GetName().c_str(), pPhysicsComponent->GetID().ToString().c_str() );
                    return;
                }

                // Add actor to scene
                if ( pPhysicsActor != nullptr && pShape != nullptr )
                {
                    SceneScopeLock const sceneLock( m_pScene, SceneScopeLock::Type::Write );
                    m_pScene->GetPxScene()->addActor( *pPhysicsActor );
                }
            }

            // Add to dynamic component list if needed
            //-------------------------------------------------------------------------

            if ( pPhysicsComponent->m_actorType == ActorType::Dynamic )
            {
                KRG_ASSERT( m_dynamicComponents.find( pPhysicsComponent->GetID() ) == m_dynamicComponents.end() );
                m_dynamicComponents.insert( TPair<UUID, PhysicsComponent*>( pPhysicsComponent->GetID(), pPhysicsComponent ) );
            }
        }
    }

    void PhysicsWorldSystem::UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent )
    {
        if ( auto pPhysicsComponent = ComponentCast<PhysicsComponent>( pComponent ) )
        {
            auto const pRecord = m_registeredEntities[pEntity->GetID()];
            KRG_ASSERT( pRecord != nullptr );

            // Remove from dynamic components list
            //-------------------------------------------------------------------------

            if ( pPhysicsComponent->m_pPhysicsActor != nullptr && pPhysicsComponent->m_actorType == ActorType::Dynamic )
            {
                m_dynamicComponents.erase( pPhysicsComponent->GetID() );
            }

            // Destroy physics actor (if one exists)
            //-------------------------------------------------------------------------

            if ( pPhysicsComponent->m_pPhysicsActor != nullptr )
            {
                {
                    SceneScopeLock const sceneLock( m_pScene, SceneScopeLock::Type::Write );
                    m_pScene->GetPxScene()->removeActor( *pPhysicsComponent->m_pPhysicsActor );
                }

                //-------------------------------------------------------------------------

                {
                    SystemScopeLock const systemLock( &m_physicsSystem );
                    pPhysicsComponent->m_pPhysicsActor->release();
                }
            }

            // Clear component data
            //-------------------------------------------------------------------------

            pPhysicsComponent->m_pPhysicsShape = nullptr;
            pPhysicsComponent->m_pPhysicsActor = nullptr;
            pPhysicsComponent->SetLocalBounds( OBB() );

            #if KRG_DEVELOPMENT_TOOLS
            pPhysicsComponent->m_debugName.clear();
            #endif

            // Remove component
            //-------------------------------------------------------------------------

            KRG_ASSERT( VectorContains( pRecord->m_components, pPhysicsComponent ) );
            pRecord->m_components.erase_first( pPhysicsComponent );

            // Remove record if empty
            //-------------------------------------------------------------------------

            if ( pRecord->IsEmpty() )
            {
                m_registeredEntities.RemoveRecord( pEntity->GetID() );
            }
        }
    }

    //-------------------------------------------------------------------------

    physx::PxRigidActor* PhysicsWorldSystem::CreateActor( PhysicsComponent* pComponent ) const
    {
        KRG_ASSERT( pComponent != nullptr );

        PxPhysics& physics = m_physicsSystem.GetPxPhysics();

        // Create and setup actor
        //-------------------------------------------------------------------------

        Transform const& worldTransform = pComponent->GetWorldTransform();
        PxTransform const bodyPose( ToPx( worldTransform.GetTranslation() ), ToPx( worldTransform.GetRotation() ) );

        physx::PxRigidActor* pPhysicsActor = nullptr;

        switch ( pComponent->m_actorType )
        {
            case ActorType::Static:
            {
                pPhysicsActor = physics.createRigidStatic( bodyPose );
            }
            break;

            case ActorType::Dynamic:
            {
                pPhysicsActor = physics.createRigidDynamic( bodyPose );
            }
            break;

            case ActorType::Kinematic:
            {
                PxRigidDynamic* pRigidDynamicActor = physics.createRigidDynamic( bodyPose );
                pRigidDynamicActor->setRigidBodyFlag( PxRigidBodyFlag::eKINEMATIC, true );
                pPhysicsActor = pRigidDynamicActor;
            }
            break;
        }

        // Set up component <-> physics links
        //-------------------------------------------------------------------------

        pPhysicsActor->userData = pComponent;
        pComponent->m_pPhysicsActor = pPhysicsActor;

        #if KRG_DEVELOPMENT_TOOLS
        pPhysicsActor->setName( pComponent->m_debugName.c_str() );
        #endif

        //-------------------------------------------------------------------------

        return pPhysicsActor;
    }

    physx::PxShape* PhysicsWorldSystem::CreateShape( PhysicsComponent* pComponent, physx::PxRigidActor* pPhysicsActor ) const
    {
        KRG_ASSERT( pComponent != nullptr && pPhysicsActor != nullptr );

        physx::PxShape* pPhysicsShape = nullptr;

        // Calculate shape flags
        //-------------------------------------------------------------------------

        PxShapeFlags shapeFlags( PxShapeFlag::eVISUALIZATION );

        switch ( pComponent->m_shapeType )
        {
            case ShapeType::SimulationAndQuery:
            {
                shapeFlags |= PxShapeFlag::eSCENE_QUERY_SHAPE;
                shapeFlags |= PxShapeFlag::eSIMULATION_SHAPE;
            }
            break;

            case ShapeType::SimulationOnly:
            {
                shapeFlags |= PxShapeFlag::eSIMULATION_SHAPE;
            }
            break;

            case ShapeType::QueryOnly:
            {
                shapeFlags |= PxShapeFlag::eSCENE_QUERY_SHAPE;
            }
            break;
        }

        // Create PhysX shape
        //-------------------------------------------------------------------------
        // We also calculate and set the component local bounds

        PxPhysics& physics = m_physicsSystem.GetPxPhysics();
        OBB localBounds;
        if ( auto pMeshComponent = ComponentCast<PhysicsMeshComponent>( pComponent ) )
        {
            KRG_ASSERT( pMeshComponent->m_pPhysicsMesh->IsValid() );
            Transform const& worldTransform = pComponent->GetWorldTransform();
            Vector const& scale = worldTransform.GetScale();

            TInlineVector<PxMaterial*, 4> const physicalMaterials = pMeshComponent->GetPhysicalMaterials();

            // Calculate bounds
            if ( pMeshComponent->m_pPhysicsMesh->IsTriangleMesh() )
            {
                PxTriangleMesh const* pTriMesh = pMeshComponent->m_pPhysicsMesh->GetTriangleMesh();
                PxTriangleMeshGeometry const meshGeo( const_cast<PxTriangleMesh*>( pTriMesh ), ToPx( scale ) );
                pPhysicsShape = physics.createShape( meshGeo, physicalMaterials.data(), (uint16) physicalMaterials.size(), true, shapeFlags );

                auto const meshLocalBounds = pTriMesh->getLocalBounds();
                localBounds = OBB( FromPx( meshLocalBounds.getCenter() ), FromPx( meshLocalBounds.getExtents() ) * scale.GetAbs() );
            }
            else // Convex Mesh
            {
                PxConvexMesh const* pConvexMesh = pMeshComponent->m_pPhysicsMesh->GetConvexMesh();
                PxConvexMeshGeometry const meshGeo( const_cast<PxConvexMesh*>( pConvexMesh ), ToPx( scale ) );
                pPhysicsShape = physics.createShape( meshGeo, physicalMaterials.data(), (uint16) physicalMaterials.size(), true, shapeFlags );

                auto const meshLocalBounds = pConvexMesh->getLocalBounds();
                localBounds = OBB( FromPx( meshLocalBounds.getCenter() ), FromPx( meshLocalBounds.getExtents() ) * scale.GetAbs() );
            }
        }
        else if ( auto pBoxComponent = ComponentCast<PhysicsBoxComponent>( pComponent ) )
        {
            PxMaterial* pPhysicsMaterial = pBoxComponent->m_pPhysicsMaterial->GetMaterial();
            KRG_ASSERT( pPhysicsMaterial != nullptr );

            PxBoxGeometry const boxGeo( ToPx( pBoxComponent->m_boxExtents ) );
            pPhysicsShape = physics.createShape( boxGeo, *pPhysicsMaterial, true, shapeFlags );
            localBounds = OBB( Vector::Origin, pBoxComponent->m_boxExtents );
        }
        else if ( auto pSphereComponent = ComponentCast<PhysicsSphereComponent>( pComponent ) )
        {
            PxMaterial* pPhysicsMaterial = pSphereComponent->m_pPhysicsMaterial->GetMaterial();
            KRG_ASSERT( pPhysicsMaterial != nullptr );

            PxSphereGeometry const sphereGeo( pSphereComponent->m_radius );
            pPhysicsShape = physics.createShape( sphereGeo, *pPhysicsMaterial, true, shapeFlags );
            localBounds = OBB( Vector::Origin, Vector( pSphereComponent->m_radius ) );
        }
        else if ( auto pCapsuleComponent = ComponentCast<PhysicsCapsuleComponent>( pComponent ) )
        {
            PxMaterial* pPhysicsMaterial = pCapsuleComponent->m_pPhysicsMaterial->GetMaterial();
            KRG_ASSERT( pPhysicsMaterial != nullptr );

            PxCapsuleGeometry const capsuleGeo( pCapsuleComponent->m_capsuleRadius, pCapsuleComponent->m_capsuleHalfHeight );
            pPhysicsShape = physics.createShape( capsuleGeo, *pPhysicsMaterial, true, shapeFlags );
            localBounds = OBB( Vector::Origin, Vector( pCapsuleComponent->m_capsuleHalfHeight + pCapsuleComponent->m_capsuleRadius, pCapsuleComponent->m_capsuleRadius, pCapsuleComponent->m_capsuleRadius ) );
        }

        pComponent->SetLocalBounds( localBounds );

        // Set component <-> physics shape links
        //-------------------------------------------------------------------------

        pPhysicsShape->userData = const_cast<PhysicsComponent*>( pComponent );
        pComponent->m_pPhysicsShape = pPhysicsShape;

        #if KRG_DEVELOPMENT_TOOLS
        pPhysicsShape->setName( pComponent->m_debugName.c_str() );
        #endif

        // Attach the shape to the new actor and release the temporary reference
        //-------------------------------------------------------------------------

        pPhysicsActor->attachShape( *pPhysicsShape );
        pPhysicsShape->release();

        //-------------------------------------------------------------------------

        return pPhysicsShape;
    }

    //-------------------------------------------------------------------------
    
    void PhysicsWorldSystem::UpdateEntitySystem( UpdateContext const& ctx )
    {
        SceneScopeLock const sceneLock( m_pScene, SceneScopeLock::Type::Read );

        for ( auto const& registeredDynamicComponent : m_dynamicComponents )
        {
            PhysicsComponent* pComponent = registeredDynamicComponent.second;

            // Transfer physics pose back to component
            if ( pComponent->m_pPhysicsActor != nullptr && pComponent->m_actorType == ActorType::Dynamic )
            {
                auto physicsPose = pComponent->m_pPhysicsActor->getGlobalPose();
                pComponent->SetWorldTransform( FromPx( physicsPose ) );
            }
        }
    }
}