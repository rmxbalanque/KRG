#include "PhysicsWorldSystem.h"
#include "PhysicsSystem.h"
#include "Components/PhysicsMeshComponent.h"
#include "Components/PhysicsCapsuleComponent.h"
#include "Components/PhysicsSphereComponent.h"
#include "Components/PhysicsBoxComponent.h"
#include "Engine/Core/Entity/Entity.h"
#include "Engine/Core/Entity/EntityUpdateContext.h"

//-------------------------------------------------------------------------

using namespace physx;

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    PhysicsWorldSystem::PhysicsWorldSystem( PhysicsSystem& physicsSystem )
        : m_pPhysicsSystem( &physicsSystem )
    {}

    //-------------------------------------------------------------------------

    void PhysicsWorldSystem::InitializeSystem( SystemRegistry const& systemRegistry )
    {
        m_pPhysicsSystem = systemRegistry.GetSystem<PhysicsSystem>();
        KRG_ASSERT( m_pPhysicsSystem != nullptr );

        m_pScene = m_pPhysicsSystem->CreateScene();
        KRG_ASSERT( m_pScene != nullptr );

        #if KRG_DEVELOPMENT_TOOLS
        SetDebugFlags( 1 << PxVisualizationParameter::eCOLLISION_SHAPES );
        #endif
    }

    void PhysicsWorldSystem::ShutdownSystem()
    {
        m_pPhysicsSystem->DestroyScene( m_pScene );
        m_pPhysicsSystem = nullptr;
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
                PxRigidActor* pPhysicsActor = CreateActor( pPhysicsComponent );
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
                    m_pScene->lockWrite();
                    m_pScene->addActor( *pPhysicsActor );
                    m_pScene->unlockWrite();
                }
            }

            // Add to dynamic component list if needed
            //-------------------------------------------------------------------------

            if ( pPhysicsComponent->m_actorType != ActorType::Static )
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

            if ( pPhysicsComponent->m_pPhysicsActor != nullptr && pPhysicsComponent->m_actorType != ActorType::Static )
            {
                m_dynamicComponents.erase( pPhysicsComponent->GetID() );
            }

            // Destroy physics actor (if one exists)
            //-------------------------------------------------------------------------

            if ( pPhysicsComponent->m_pPhysicsActor != nullptr )
            {
                m_pScene->lockWrite();
                m_pScene->removeActor( *pPhysicsComponent->m_pPhysicsActor );
                m_pScene->unlockWrite();

                pPhysicsComponent->m_pPhysicsActor->release();
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

        PxPhysics& physics = m_pPhysicsSystem->GetPxPhysics();

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

        // Get physics materials
        //-------------------------------------------------------------------------

        // Get physic materials
        TInlineVector<StringID, 4> const physicsMaterialIDs = pComponent->GetPhysicsMaterialIDs();
        TInlineVector<PxMaterial*, 4> physicsMaterials;

        for ( auto const& materialID : physicsMaterialIDs )
        {
            PxMaterial* pPhysicsMaterial = m_pPhysicsSystem->GetMaterial( materialID );
            if ( pPhysicsMaterial == nullptr )
            {
                return false;
            }

            physicsMaterials.emplace_back( pPhysicsMaterial );
        }

        if ( physicsMaterials.empty() )
        {
            KRG_LOG_ERROR( "Physics", "No physics materials set for component %s (%s). No shapes will be created!", pComponent->GetName().c_str(), pComponent->GetID().ToString().c_str() );
            return false;
        }

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

        PxPhysics& physics = m_pPhysicsSystem->GetPxPhysics();
        OBB localBounds;
        if ( auto pMeshComponent = ComponentCast<PhysicsMeshComponent>( pComponent ) )
        {
            KRG_ASSERT( pMeshComponent->m_pPhysicsMesh->IsValid() );
            Transform const& worldTransform = pComponent->GetWorldTransform();
            Vector const& scale = worldTransform.GetScale();

            // Calculate bounds
            if ( pMeshComponent->m_pPhysicsMesh->IsTriangleMesh() )
            {
                PxTriangleMesh const* pTriMesh = pMeshComponent->m_pPhysicsMesh->GetTriangleMesh();
                PxTriangleMeshGeometry const meshGeo( const_cast<PxTriangleMesh*>( pTriMesh ), ToPx( scale ) );
                pPhysicsShape = physics.createShape( meshGeo, physicsMaterials.data(), (uint16) physicsMaterials.size(), true, shapeFlags );

                auto const meshLocalBounds = pTriMesh->getLocalBounds();
                localBounds = OBB( FromPx( meshLocalBounds.getCenter() ), FromPx( meshLocalBounds.getExtents() ) * scale.GetAbs() );
            }
            else // Convex Mesh
            {
                PxConvexMesh const* pConvexMesh = pMeshComponent->m_pPhysicsMesh->GetConvexMesh();
                PxConvexMeshGeometry const meshGeo( const_cast<PxConvexMesh*>( pConvexMesh ), ToPx( scale ) );
                pPhysicsShape = physics.createShape( meshGeo, physicsMaterials.data(), (uint16) physicsMaterials.size(), true, shapeFlags );

                auto const meshLocalBounds = pConvexMesh->getLocalBounds();
                localBounds = OBB( FromPx( meshLocalBounds.getCenter() ), FromPx( meshLocalBounds.getExtents() ) * scale.GetAbs() );
            }
        }
        else if ( auto pBoxComponent = ComponentCast<PhysicsBoxComponent>( pComponent ) )
        {
            PxBoxGeometry const boxGeo( ToPx( pBoxComponent->m_boxExtents ) );
            pPhysicsShape = physics.createShape( boxGeo, physicsMaterials.data(), (uint16) physicsMaterials.size(), true, shapeFlags );
            localBounds = OBB( Vector::Origin, pBoxComponent->m_boxExtents );
        }
        else if ( auto pSphereComponent = ComponentCast<PhysicsSphereComponent>( pComponent ) )
        {
            PxSphereGeometry const sphereGeo( pSphereComponent->m_radius );
            pPhysicsShape = physics.createShape( sphereGeo, physicsMaterials.data(), (uint16) physicsMaterials.size(), true, shapeFlags );
            localBounds = OBB( Vector::Origin, Vector( pSphereComponent->m_radius ) );
        }
        else if ( auto pCapsuleComponent = ComponentCast<PhysicsCapsuleComponent>( pComponent ) )
        {
            PxCapsuleGeometry const capsuleGeo( pCapsuleComponent->m_capsuleRadius, pCapsuleComponent->m_capsuleHalfHeight );
            pPhysicsShape = physics.createShape( capsuleGeo, physicsMaterials.data(), (uint16) physicsMaterials.size(), true, shapeFlags );
            localBounds = OBB( Vector::Origin, Vector( pCapsuleComponent->m_capsuleHalfHeight + pCapsuleComponent->m_capsuleRadius, pCapsuleComponent->m_capsuleRadius, pCapsuleComponent->m_capsuleRadius ) );
        }

        pComponent->SetLocalBounds( localBounds );

        // Set component <-> physics shape links
        //-------------------------------------------------------------------------

        pPhysicsShape->setSimulationFilterData( PxFilterData( pComponent->m_layers.Get(), 0, 0, 0 ) );
        pPhysicsShape->setQueryFilterData( PxFilterData( pComponent->m_layers.Get(), 0, 0, 0 ) );

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
    
    void PhysicsWorldSystem::UpdateSystem( EntityUpdateContext const& ctx )
    {
        #if KRG_DEVELOPMENT_TOOLS
        auto drawingContext = ctx.GetDrawingContext();
        #endif

        for ( auto const& registeredDynamicComponent : m_dynamicComponents )
        {
            PhysicsComponent* pComponent = registeredDynamicComponent.second;

            // Transfer physics pose back to component
            if ( pComponent->m_pPhysicsActor != nullptr && pComponent->m_actorType == ActorType::Dynamic )
            {
                auto physicsPose = pComponent->m_pPhysicsActor->getGlobalPose();
                pComponent->SetWorldTransform( FromPx( physicsPose ) );
            }

            // Debug
            //-------------------------------------------------------------------------

            #if KRG_DEVELOPMENT_TOOLS
            if ( m_drawDynamicActorBounds && pComponent->m_actorType == ActorType::Dynamic )
            {
                drawingContext.DrawBox( pComponent->GetWorldBounds(), Colors::Orange.GetAlphaVersion( 0.5f ) );
                drawingContext.DrawWireBox( pComponent->GetWorldBounds(), Colors::Orange );
            }

            if ( m_drawKinematicActorBounds && pComponent->m_actorType == ActorType::Kinematic )
            {
                drawingContext.DrawBox( pComponent->GetWorldBounds(), Colors::HotPink.GetAlphaVersion( 0.5f ) );
                drawingContext.DrawWireBox( pComponent->GetWorldBounds(), Colors::HotPink );
            }
            #endif
        }
    }

    //-------------------------------------------------------------------------

    bool PhysicsWorldSystem::RayCast( Vector const& start, Vector const& unitDirection, float distance, QueryRules const& rules, PxRaycastCallback& outResults )
    {
        KRG_ASSERT( m_readLockAcquired );
        KRG_ASSERT( unitDirection.IsNormalized3() && distance > 0 );

        bool const result = m_pScene->raycast( ToPx( start ), ToPx( unitDirection ), distance, outResults, PxHitFlag::eDEFAULT, rules, &m_queryFilter );
        return result;
    }

    bool PhysicsWorldSystem::RayCast( Vector const& start, Vector const& end, QueryRules const& rules, PxRaycastCallback& outResults )
    {
        Vector dir, length;
        ( end - start ).ToDirectionAndLength3( dir, length );
        KRG_ASSERT( !dir.IsNearZero3() );

        return RayCast( start, dir, length.m_x, rules, outResults );
    }

    //-------------------------------------------------------------------------

    bool PhysicsWorldSystem::SphereCast( float radius, Vector const& start, Vector const& unitDirection, float distance, QueryRules const& rules, PxSweepCallback& outResults )
    {
        KRG_ASSERT( m_readLockAcquired );
        KRG_ASSERT( unitDirection.IsNormalized3() && distance > 0 );

        PxSphereGeometry const sphereGeo( radius );
        bool const result = m_pScene->sweep( sphereGeo, PxTransform( ToPx( start ) ), ToPx( unitDirection ), distance, outResults, PxHitFlag::eDEFAULT, rules, &m_queryFilter );
        return result;
    }

    bool PhysicsWorldSystem::SphereCast( float radius, Vector const& start, Vector const& end, QueryRules const& rules, PxSweepCallback& outResults )
    {
        Vector dir, length;
        ( end - start ).ToDirectionAndLength3( dir, length );
        KRG_ASSERT( !dir.IsNearZero3() );

        return SphereCast( radius, start, dir, length.m_x, rules, outResults );
    }

    bool PhysicsWorldSystem::SphereOverlap( float radius, Vector const& position, QueryRules const& rules, PxOverlapCallback& outResults )
    {
        KRG_ASSERT( m_readLockAcquired );
        PxSphereGeometry const sphereGeo( radius );
        QueryRules overlapRules = rules;
        overlapRules.flags |= PxQueryFlag::eNO_BLOCK;
        bool const result = m_pScene->overlap( sphereGeo, PxTransform( ToPx( position ) ), outResults, overlapRules, &m_queryFilter );
        return result;
    }

    //-------------------------------------------------------------------------

    bool PhysicsWorldSystem::CapsuleCast( float halfHeight, float radius, Quaternion const& orientation, Vector const& start, Vector const& unitDirection, float distance, QueryRules const& rules, PxSweepCallback& outResults )
    {
        KRG_ASSERT( m_readLockAcquired );
        KRG_ASSERT( unitDirection.IsNormalized3() && distance > 0 );

        PxCapsuleGeometry const capsuleGeo( radius, halfHeight );
        bool const result = m_pScene->sweep( capsuleGeo, PxTransform( ToPx( start ), ToPx( orientation ) ), ToPx( unitDirection ), distance, outResults, PxHitFlag::eDEFAULT, rules, &m_queryFilter );
        return result;
    }

    bool PhysicsWorldSystem::CapsuleCast( float halfHeight, float radius, Quaternion const& orientation, Vector const& start, Vector const& end, QueryRules const& rules, PxSweepCallback& outResults )
    {
        Vector dir, length;
        ( end - start ).ToDirectionAndLength3( dir, length );
        KRG_ASSERT( !dir.IsNearZero3() );

        return CapsuleCast( halfHeight, radius, orientation, start, dir, length.m_x, rules, outResults );
    }

    bool PhysicsWorldSystem::CapsuleOverlap( float halfHeight, float radius, Quaternion const& orientation, Vector const& position, QueryRules const& rules, PxOverlapCallback& outResults )
    {
        KRG_ASSERT( m_readLockAcquired );
        PxConvexMeshGeometry const cylinderGeo( SharedMeshes::s_pUnitCylinderMesh, PxMeshScale( PxVec3( radius, radius, halfHeight ) ) );
        bool result = m_pScene->overlap( cylinderGeo, PxTransform( ToPx( position ), ToPx( orientation ) ), outResults, rules, &m_queryFilter );
        return result;
    }

    //-------------------------------------------------------------------------

    bool PhysicsWorldSystem::CylinderCast( float halfHeight, float radius, Quaternion const& orientation, Vector const& start, Vector const& unitDirection, float distance, QueryRules const& rules, PxSweepCallback& outResults )
    {
        KRG_ASSERT( m_readLockAcquired );
        KRG_ASSERT( unitDirection.IsNormalized3() && distance > 0 );
        KRG_ASSERT( SharedMeshes::s_pUnitCylinderMesh != nullptr );

        PxConvexMeshGeometry const cylinderGeo( SharedMeshes::s_pUnitCylinderMesh, PxMeshScale( PxVec3( radius, radius, halfHeight ) ) );
        bool const result = m_pScene->sweep( cylinderGeo, PxTransform( ToPx( start ), ToPx( orientation ) ), ToPx( unitDirection ), distance, outResults, PxHitFlag::eDEFAULT, rules, &m_queryFilter );
        return result;
    }

    bool PhysicsWorldSystem::CylinderCast( float halfHeight, float radius, Quaternion const& orientation, Vector const& start, Vector const& end, QueryRules const& rules, PxSweepCallback& outResults )
    {
        Vector dir, length;
        ( end - start ).ToDirectionAndLength3( dir, length );
        KRG_ASSERT( !dir.IsNearZero3() );

        return CylinderCast( halfHeight, radius, orientation, start, dir, length.m_x, rules, outResults );
    }

    bool PhysicsWorldSystem::CylinderOverlap( float halfHeight, float radius, Quaternion const& orientation, Vector const& position, QueryRules const& rules, PxOverlapCallback& outResults )
    {
        KRG_ASSERT( m_readLockAcquired );
        PxCapsuleGeometry const capsuleGeo( radius, halfHeight );
        bool const result = m_pScene->overlap( capsuleGeo, PxTransform( ToPx( position ), ToPx( orientation ) ), outResults, rules, &m_queryFilter );
        return result;
    }

    //-------------------------------------------------------------------------

    bool PhysicsWorldSystem::BoxCast( Vector halfExtents, Vector const& position, Quaternion const& orientation, Vector const& start, Vector const& unitDirection, float distance, QueryRules const& rules, PxSweepCallback& outResults )
    {
        KRG_ASSERT( m_readLockAcquired );
        KRG_ASSERT( unitDirection.IsNormalized3() && distance > 0 );

        PxBoxGeometry const boxGeo( ToPx( halfExtents ) );
        bool const result = m_pScene->sweep( boxGeo, PxTransform( ToPx( start ), ToPx( orientation ) ), ToPx( unitDirection ), distance, outResults, PxHitFlag::eDEFAULT, rules, &m_queryFilter );
        return result;
    }

    bool PhysicsWorldSystem::BoxCast( Vector halfExtents, Vector const& position, Quaternion const& orientation, Vector const& start, Vector const& end, QueryRules const& rules, PxSweepCallback& outResults )
    {
        Vector dir, length;
        ( end - start ).ToDirectionAndLength3( dir, length );
        KRG_ASSERT( !dir.IsNearZero3() );

        return BoxCast( halfExtents, position, orientation, start, dir, length.m_x, rules, outResults );
    }

    bool PhysicsWorldSystem::BoxOverlap( Vector halfExtents, Vector const& position, Quaternion const& orientation, QueryRules const& rules, PxOverlapCallback& outResults )
    {
        KRG_ASSERT( m_readLockAcquired );
        PxBoxGeometry const boxGeo( ToPx( halfExtents ) );
        bool const result = m_pScene->overlap( boxGeo, PxTransform( ToPx( position ), ToPx( orientation ) ), outResults, rules, &m_queryFilter );
        return result;
    }

    //-------------------------------------------------------------------------

    bool PhysicsWorldSystem::ShapeCast( physx::PxShape* pShape, Transform const& startTransform, Vector const& unitDirection, float distance, QueryRules const& rules, PxSweepCallback& outResults )
    {
        KRG_ASSERT( unitDirection.IsNormalized3() && distance > 0 );
        bool const result = m_pScene->sweep( pShape->getGeometry().any(), ToPx( startTransform ), ToPx( unitDirection ), distance, outResults, PxHitFlag::eDEFAULT, rules, &m_queryFilter );
        return result;
    }

    bool PhysicsWorldSystem::ShapeCast( physx::PxShape* pShape, Transform const& startTransform, Vector const& desiredEndPosition, QueryRules const& rules, PxSweepCallback& outResults )
    {
        KRG_ASSERT( m_readLockAcquired );
        Vector dir, length;
        ( desiredEndPosition - startTransform.GetTranslation() ).ToDirectionAndLength3( dir, length );
        KRG_ASSERT( !dir.IsNearZero3() );

        return ShapeCast( pShape, startTransform, dir, length.m_x, rules, outResults );
    }

    bool PhysicsWorldSystem::ShapeOverlap( physx::PxShape* pShape, Transform const& transform, QueryRules const& rules, PxOverlapCallback& outResults )
    {
        KRG_ASSERT( m_readLockAcquired );
        bool const result = m_pScene->overlap( pShape->getGeometry().any(), ToPx( transform ), outResults, rules, &m_queryFilter );
        return result;
    }

    //------------------------------------------------------------------------- 
    // Debug
    //-------------------------------------------------------------------------

    #if KRG_DEVELOPMENT_TOOLS
    bool PhysicsWorldSystem::IsDebugDrawingEnabled() const
    {
        return m_sceneDebugFlags && ( 1 << physx::PxVisualizationParameter::eSCALE ) != 0;
    }

    void PhysicsWorldSystem::SetDebugFlags( uint32 debugFlags )
    {
        KRG_ASSERT( m_pScene != nullptr );
        m_sceneDebugFlags = debugFlags;

        //-------------------------------------------------------------------------

        auto SetVisualizationParameter = [this] ( PxVisualizationParameter::Enum flag, float onValue, float offValue )
        {
            bool const isFlagSet = ( m_sceneDebugFlags & ( 1 << flag ) ) != 0;
            m_pScene->setVisualizationParameter( flag, isFlagSet ? onValue : offValue );
        };

        AcquireWriteLock();
        SetVisualizationParameter( PxVisualizationParameter::eSCALE, 1.0f, 0.0f );
        SetVisualizationParameter( PxVisualizationParameter::eCOLLISION_AABBS, 1.0f, 0.0f );
        SetVisualizationParameter( PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f, 0.0f );
        SetVisualizationParameter( PxVisualizationParameter::eCOLLISION_AXES, 0.25f, 0.0f );
        SetVisualizationParameter( PxVisualizationParameter::eCOLLISION_FNORMALS, 0.15f, 0.0f );
        SetVisualizationParameter( PxVisualizationParameter::eCOLLISION_EDGES, 1.0f, 0.0f );
        SetVisualizationParameter( PxVisualizationParameter::eCONTACT_POINT, 1.0f, 0.0f );
        SetVisualizationParameter( PxVisualizationParameter::eCONTACT_NORMAL, 0.25f, 0.0f );
        SetVisualizationParameter( PxVisualizationParameter::eCONTACT_FORCE, 0.25f, 0.0f );
        SetVisualizationParameter( PxVisualizationParameter::eACTOR_AXES, 0.25f, 0.0f );
        SetVisualizationParameter( PxVisualizationParameter::eBODY_AXES, 0.25f, 0.0f );
        SetVisualizationParameter( PxVisualizationParameter::eBODY_LIN_VELOCITY, 1.0f, 0.0f );
        SetVisualizationParameter( PxVisualizationParameter::eBODY_ANG_VELOCITY, 1.0f, 0.0f );
        SetVisualizationParameter( PxVisualizationParameter::eBODY_MASS_AXES, 1.0f, 0.0f );
        SetVisualizationParameter( PxVisualizationParameter::eJOINT_LIMITS, 1.0f, 0.0f );
        SetVisualizationParameter( PxVisualizationParameter::eJOINT_LOCAL_FRAMES, 1.0f, 0.0f );
        ReleaseWriteLock();
    }
    #endif
}