#include "PhysicsSystem.h"
#include "PhysicsMaterialDatabase.h"
#include "PhysicsSimulationFilter.h"
#include "System/Entity/Entity.h"
#include "System/Core/Profiling/Profiling.h"

//-------------------------------------------------------------------------

using namespace physx;

//-------------------------------------------------------------------------
// Shared Mesh Constructors
//-------------------------------------------------------------------------

namespace KRG::Physics
{
    // Creates a unit cylinder mesh with the specified number of side
    // Half-height is along the X axis, Radius is in the Y/Z plane
    static PxConvexMesh* CreateSharedCylinderMesh( PxPhysics* pPhysics, PxCooking* pCooking, uint32 numSides )
    {
        KRG_ASSERT( pPhysics != nullptr && pCooking != nullptr );

        //-------------------------------------------------------------------------

        Radians const anglePerSide( Math::TwoPi / numSides );
        PxVec3 const rotationAxis( 1, 0, 0 );
        PxVec3 const vertVector( 0, 0, -0.5f );
        PxVec3 const cylinderTopOffset( -0.5f, 0, 0 );
        PxVec3 const cylinderBottomOffset( 0.5f, 0, 0 );

        //-------------------------------------------------------------------------

        TVector<PxVec3> convexVerts;
        convexVerts.resize( ( numSides + 1 ) * 2 );

        convexVerts[0] = vertVector + cylinderTopOffset;
        convexVerts[numSides] = vertVector + cylinderBottomOffset;

        Radians currentAngle = anglePerSide;
        for ( uint32 i = 1; i < numSides; i++ )
        {
            PxQuat const rotation( currentAngle, rotationAxis );
            PxVec3 const rotatedVertVector = rotation.rotate( vertVector );

            convexVerts[i] = rotatedVertVector + cylinderTopOffset;
            convexVerts[numSides + i] = rotatedVertVector + cylinderBottomOffset;

            currentAngle += anglePerSide;
        }

        //-------------------------------------------------------------------------

        PxConvexMeshDesc convexDesc;
        convexDesc.points.count = (uint32) convexVerts.size();
        convexDesc.points.stride = sizeof( PxVec3 );
        convexDesc.points.data = convexVerts.data();
        convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

        PxDefaultMemoryOutputStream buffer;
        PxConvexMeshCookingResult::Enum result;
        if ( !pCooking->cookConvexMesh( convexDesc, buffer, &result ) )
        {
            KRG_UNREACHABLE_CODE();
        }

        PxDefaultMemoryInputData input( buffer.getData(), buffer.getSize() );
        PxConvexMesh* pCylinderMesh = pPhysics->createConvexMesh( input );
        KRG_ASSERT( pCylinderMesh != nullptr );
        return pCylinderMesh;
    }
}

//-------------------------------------------------------------------------
// System
//-------------------------------------------------------------------------

namespace KRG::Physics
{
    void PhysicsSystem::Initialize()
    {
        KRG_ASSERT( m_pFoundation == nullptr && m_pPhysics == nullptr && m_pDispatcher == nullptr );

        m_pAllocatorCallback = KRG::New<PhysXAllocator>();
        m_pErrorCallback = KRG::New<PhysXUserErrorCallback>();

        m_pFoundation = PxCreateFoundation( PX_PHYSICS_VERSION, *m_pAllocatorCallback, *m_pErrorCallback );
        m_pDispatcher = KRG::New<PhysXTaskDispatcher>();

        #if KRG_DEVELOPMENT_TOOLS
        m_pPVD = PxCreatePvd( *m_pFoundation );
        m_pPVDTransport = PxDefaultPvdSocketTransportCreate( "127.0.0.1", 5425, 10 );
        #endif

        PxTolerancesScale tolerancesScale;
        tolerancesScale.length = Constants::s_lengthScale;
        tolerancesScale.speed = Constants::s_speedScale;
        m_pPhysics = PxCreatePhysics( PX_PHYSICS_VERSION, *m_pFoundation, tolerancesScale, true, m_pPVD );
        m_pCooking = PxCreateCooking( PX_PHYSICS_VERSION, *m_pFoundation, PxCookingParams( tolerancesScale ) );

        // Create PhysX scene
        //-------------------------------------------------------------------------

        KRG_ASSERT( m_pScene == nullptr );
        PxSceneDesc sceneDesc( tolerancesScale );
        sceneDesc.gravity = ToPx( Constants::s_gravity );
        sceneDesc.cpuDispatcher = m_pDispatcher;
        sceneDesc.filterShader = SimulationFilter::Shader;
        m_pScene = m_pPhysics->createScene( sceneDesc );

        // Create shared resources
        //-------------------------------------------------------------------------

        CreateSharedMeshes();

        // Debug
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        auto pPvdClient = m_pScene->getScenePvdClient();
        pPvdClient->setScenePvdFlag( PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true );
        pPvdClient->setScenePvdFlag( PxPvdSceneFlag::eTRANSMIT_CONTACTS, true );
        pPvdClient->setScenePvdFlag( PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true );
        SetDebugFlags( 1 << PxVisualizationParameter::eCOLLISION_SHAPES );
        #endif
    }

    void PhysicsSystem::Shutdown()
    {
        KRG_ASSERT( m_pFoundation != nullptr && m_pPhysics != nullptr && m_pDispatcher != nullptr );

        #if KRG_DEVELOPMENT_TOOLS
        if ( m_pPVD->isConnected() )
        {
            m_pPVD->disconnect();
        }
        #endif

        //-------------------------------------------------------------------------

        DestroySharedMeshes();

        //-------------------------------------------------------------------------

        if ( m_pScene != nullptr )
        {
            m_pScene->release();
            m_pScene = nullptr;
        }

        //-------------------------------------------------------------------------

        m_pCooking->release();
        m_pPhysics->release();

        #if KRG_DEVELOPMENT_TOOLS
        m_pPVDTransport->release();
        m_pPVD->release();
        #endif

        KRG::Delete( m_pDispatcher );
        m_pFoundation->release();

        KRG::Delete( m_pErrorCallback );
        KRG::Delete( m_pAllocatorCallback );
    }

    //-------------------------------------------------------------------------

    void PhysicsSystem::CreateSharedMeshes()
    {
        KRG_ASSERT( m_pCooking != nullptr );
        Threading::ScopeLock lock( m_systemLock );

        KRG_ASSERT( SharedMeshes::s_pUnitCylinderMesh == nullptr );
        SharedMeshes::s_pUnitCylinderMesh = CreateSharedCylinderMesh( m_pPhysics, m_pCooking, 30 );
    }

    void PhysicsSystem::DestroySharedMeshes()
    {
        Threading::ScopeLock lock( m_systemLock );
        SharedMeshes::s_pUnitCylinderMesh->release();
        SharedMeshes::s_pUnitCylinderMesh = nullptr;
    }

    //-------------------------------------------------------------------------

    void PhysicsSystem::Update( UpdateContext& ctx )
    {
        {
            KRG_PROFILE_SCOPE_PHYSICS( "Simulate" );
            m_pScene->simulate( ctx.GetDeltaTime() );
        }

        {
            KRG_PROFILE_SCOPE_PHYSICS( "Fetch Results" );
            m_pScene->fetchResults( true );
        }

        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        UpdatePVD( ctx.GetDeltaTime() );
        #endif
    }
}

//-------------------------------------------------------------------------
// Physical Materials
//-------------------------------------------------------------------------

namespace KRG::Physics
{
    void PhysicsSystem::FillMaterialDatabase( TVector<PhysicsMaterialSettings> const& materials )
    {
        ScopeLock const lock( this, ScopeLock::Type::Write );

        // Create default physics material
        StringID const defaultMaterialID( PhysicsMaterial::DefaultID );
        auto pxMaterial = m_pPhysics->createMaterial( PhysicsMaterial::DefaultStaticFriction, PhysicsMaterial::DefaultDynamicFriction, PhysicsMaterial::DefaultRestitution );
        m_materials.insert( TPair<StringID, PhysicsMaterial>( defaultMaterialID, PhysicsMaterial( defaultMaterialID, pxMaterial ) ) );

        // Create physX material
        for ( auto const& materialSettings : materials )
        {
            pxMaterial = m_pPhysics->createMaterial( materialSettings.m_staticFriction, materialSettings.m_dynamicFriction, materialSettings.m_restitution );
            pxMaterial->setFrictionCombineMode( (PxCombineMode::Enum) materialSettings.m_frictionCombineMode );
            pxMaterial->setRestitutionCombineMode( (PxCombineMode::Enum) materialSettings.m_restitutionCombineMode );

            m_materials.insert( TPair<StringID, PhysicsMaterial>( materialSettings.m_ID, PhysicsMaterial( materialSettings.m_ID, pxMaterial ) ) );
        }
    }

    void PhysicsSystem::ClearMaterialDatabase()
    {
        ScopeLock const lock( this, ScopeLock::Type::Write );

        for ( auto& materialPair : m_materials )
        {
            materialPair.second.m_pMaterial->release();
            materialPair.second.m_pMaterial = nullptr;
        }

        m_materials.clear();
    }

    PxMaterial* PhysicsSystem::GetMaterial( StringID materialID ) const
    {
        KRG_ASSERT( materialID.IsValid() );

        auto foundMaterialPairIter = m_materials.find( materialID );
        if ( foundMaterialPairIter != m_materials.end() )
        {
            return foundMaterialPairIter->second.m_pMaterial;
        }

        KRG_LOG_WARNING( "Physics", "Failed to find physic material with ID: %s", materialID.c_str() );
        return nullptr;
    }
}

//-------------------------------------------------------------------------
// Queries
//-------------------------------------------------------------------------

namespace KRG::Physics
{
    bool PhysicsSystem::RayCast( Vector const& start, Vector const& unitDirection, float distance, QueryRules const& rules, PxRaycastCallback& outResults )
    {
        KRG_ASSERT( m_readLockAcquired );
        KRG_ASSERT( unitDirection.IsNormalized3() && distance > 0 );

        bool const result = m_pScene->raycast( ToPx( start ), ToPx( unitDirection ), distance, outResults, PxHitFlag::eDEFAULT, rules, &m_queryFilter );
        return result;
    }

    bool PhysicsSystem::RayCast( Vector const& start, Vector const& end, QueryRules const& rules, PxRaycastCallback& outResults )
    {
        Vector dir, length;
        ( end - start ).ToDirectionAndLength3( dir, length );
        KRG_ASSERT( !dir.IsNearZero3() );

        return RayCast( start, dir, length.m_x, rules, outResults );
    }

    //-------------------------------------------------------------------------

    bool PhysicsSystem::SphereCast( float radius, Vector const& start, Vector const& unitDirection, float distance, QueryRules const& rules, PxSweepCallback& outResults )
    {
        KRG_ASSERT( m_readLockAcquired );
        KRG_ASSERT( unitDirection.IsNormalized3() && distance > 0 );

        PxSphereGeometry const sphereGeo( radius );
        bool const result = m_pScene->sweep( sphereGeo, PxTransform( ToPx( start ) ), ToPx( unitDirection ), distance, outResults, PxHitFlag::eDEFAULT, rules, &m_queryFilter );
        return result;
    }

    bool PhysicsSystem::SphereCast( float radius, Vector const& start, Vector const& end, QueryRules const& rules, PxSweepCallback& outResults )
    {
        Vector dir, length;
        ( end - start ).ToDirectionAndLength3( dir, length );
        KRG_ASSERT( !dir.IsNearZero3() );

        return SphereCast( radius, start, dir, length.m_x, rules, outResults );
    }

    bool PhysicsSystem::SphereOverlap( float radius, Vector const& position, QueryRules const& rules, PxOverlapCallback& outResults )
    {
        KRG_ASSERT( m_readLockAcquired );
        PxSphereGeometry const sphereGeo( radius );
        QueryRules overlapRules = rules;
        overlapRules.flags |= PxQueryFlag::eNO_BLOCK;
        bool const result = m_pScene->overlap( sphereGeo, PxTransform( ToPx( position ) ), outResults, overlapRules, &m_queryFilter );
        return result;
    }

    //-------------------------------------------------------------------------

    bool PhysicsSystem::CapsuleCast( float halfHeight, float radius, Quaternion const& orientation, Vector const& start, Vector const& unitDirection, float distance, QueryRules const& rules, PxSweepCallback& outResults )
    {
        KRG_ASSERT( m_readLockAcquired );
        KRG_ASSERT( unitDirection.IsNormalized3() && distance > 0 );

        PxCapsuleGeometry const capsuleGeo( radius, halfHeight );
        bool const result = m_pScene->sweep( capsuleGeo, PxTransform( ToPx( start ), ToPx( orientation ) ), ToPx( unitDirection ), distance, outResults, PxHitFlag::eDEFAULT, rules, &m_queryFilter );
        return result;
    }

    bool PhysicsSystem::CapsuleCast( float halfHeight, float radius, Quaternion const& orientation, Vector const& start, Vector const& end, QueryRules const& rules, PxSweepCallback& outResults )
    {
        Vector dir, length;
        ( end - start ).ToDirectionAndLength3( dir, length );
        KRG_ASSERT( !dir.IsNearZero3() );

        return CapsuleCast( halfHeight, radius, orientation, start, dir, length.m_x, rules, outResults );
    }

    bool PhysicsSystem::CapsuleOverlap( float halfHeight, float radius, Quaternion const& orientation, Vector const& position, QueryRules const& rules, PxOverlapCallback& outResults )
    {
        KRG_ASSERT( m_readLockAcquired );
        PxConvexMeshGeometry const cylinderGeo( SharedMeshes::s_pUnitCylinderMesh, PxMeshScale( PxVec3( radius, radius, halfHeight ) ) );
        bool result = m_pScene->overlap( cylinderGeo, PxTransform( ToPx( position ), ToPx( orientation ) ), outResults, rules, &m_queryFilter );
        return result;
    }

    //-------------------------------------------------------------------------

    bool PhysicsSystem::CylinderCast( float halfHeight, float radius, Quaternion const& orientation, Vector const& start, Vector const& unitDirection, float distance, QueryRules const& rules, PxSweepCallback& outResults )
    {
        KRG_ASSERT( m_readLockAcquired );
        KRG_ASSERT( unitDirection.IsNormalized3() && distance > 0 );
        KRG_ASSERT( SharedMeshes::s_pUnitCylinderMesh != nullptr );

        PxConvexMeshGeometry const cylinderGeo( SharedMeshes::s_pUnitCylinderMesh, PxMeshScale( PxVec3( radius, radius, halfHeight ) ) );
        bool const result = m_pScene->sweep( cylinderGeo, PxTransform( ToPx( start ), ToPx( orientation ) ), ToPx( unitDirection ), distance, outResults, PxHitFlag::eDEFAULT, rules, &m_queryFilter );
        return result;
    }

    bool PhysicsSystem::CylinderCast( float halfHeight, float radius, Quaternion const& orientation, Vector const& start, Vector const& end, QueryRules const& rules, PxSweepCallback& outResults )
    {
        Vector dir, length;
        ( end - start ).ToDirectionAndLength3( dir, length );
        KRG_ASSERT( !dir.IsNearZero3() );

        return CylinderCast( halfHeight, radius, orientation, start, dir, length.m_x, rules, outResults );
    }

    bool PhysicsSystem::CylinderOverlap( float halfHeight, float radius, Quaternion const& orientation, Vector const& position, QueryRules const& rules, PxOverlapCallback& outResults )
    {
        KRG_ASSERT( m_readLockAcquired );
        PxCapsuleGeometry const capsuleGeo( radius, halfHeight );
        bool const result = m_pScene->overlap( capsuleGeo, PxTransform( ToPx( position ), ToPx( orientation ) ), outResults, rules, &m_queryFilter );
        return result;
    }

    //-------------------------------------------------------------------------

    bool PhysicsSystem::BoxCast( Vector halfExtents, Vector const& position, Quaternion const& orientation, Vector const& start, Vector const& unitDirection, float distance, QueryRules const& rules, PxSweepCallback& outResults )
    {
        KRG_ASSERT( m_readLockAcquired );
        KRG_ASSERT( unitDirection.IsNormalized3() && distance > 0 );

        PxBoxGeometry const boxGeo( ToPx( halfExtents ) );
        bool const result = m_pScene->sweep( boxGeo, PxTransform( ToPx( start ), ToPx( orientation ) ), ToPx( unitDirection ), distance, outResults, PxHitFlag::eDEFAULT, rules, &m_queryFilter );
        return result;
    }

    bool PhysicsSystem::BoxCast( Vector halfExtents, Vector const& position, Quaternion const& orientation, Vector const& start, Vector const& end, QueryRules const& rules, PxSweepCallback& outResults )
    {
        Vector dir, length;
        ( end - start ).ToDirectionAndLength3( dir, length );
        KRG_ASSERT( !dir.IsNearZero3() );

        return BoxCast( halfExtents, position, orientation, start, dir, length.m_x, rules, outResults );
    }

    bool PhysicsSystem::BoxOverlap( Vector halfExtents, Vector const& position, Quaternion const& orientation, QueryRules const& rules, PxOverlapCallback& outResults )
    {
        KRG_ASSERT( m_readLockAcquired );
        PxBoxGeometry const boxGeo( ToPx( halfExtents ) );
        bool const result = m_pScene->overlap( boxGeo, PxTransform( ToPx( position ), ToPx( orientation ) ), outResults, rules, &m_queryFilter );
        return result;
    }

    //-------------------------------------------------------------------------

    bool PhysicsSystem::ShapeCast( physx::PxShape* pShape, Transform const& startTransform, Vector const& unitDirection, float distance, QueryRules const& rules, PxSweepCallback& outResults )
    {
        KRG_ASSERT( unitDirection.IsNormalized3() && distance > 0 );
        bool const result = m_pScene->sweep( pShape->getGeometry().any(), ToPx( startTransform ), ToPx( unitDirection ), distance, outResults, PxHitFlag::eDEFAULT, rules, &m_queryFilter );
        return result;
    }

    bool PhysicsSystem::ShapeCast( physx::PxShape* pShape, Transform const& startTransform, Vector const& desiredEndPosition, QueryRules const& rules, PxSweepCallback& outResults )
    {
        KRG_ASSERT( m_readLockAcquired );
        Vector dir, length;
        ( desiredEndPosition - startTransform.GetTranslation() ).ToDirectionAndLength3( dir, length );
        KRG_ASSERT( !dir.IsNearZero3() );

        return ShapeCast( pShape, startTransform, dir, length.m_x, rules, outResults );
    }

    bool PhysicsSystem::ShapeOverlap( physx::PxShape* pShape, Transform const& transform, QueryRules const& rules, PxOverlapCallback& outResults )
    {
        KRG_ASSERT( m_readLockAcquired );
        bool const result = m_pScene->overlap( pShape->getGeometry().any(), ToPx( transform ), outResults, rules, &m_queryFilter );
        return result;
    }
}

//------------------------------------------------------------------------- 
// Debug
//-------------------------------------------------------------------------

namespace KRG::Physics
{
    #if KRG_DEVELOPMENT_TOOLS
    bool PhysicsSystem::IsConnectedToPVD()
    {
        KRG_ASSERT( m_pPVD != nullptr );
        return m_pPVD->isConnected();
    }

    void PhysicsSystem::ConnectToPVD( Seconds timeToRecord )
    {
        KRG_ASSERT( m_pPVD != nullptr );
        if ( !m_pPVD->isConnected() )
        {
            m_pPVD->connect( *m_pPVDTransport, PxPvdInstrumentationFlag::eALL );
            m_recordingTimeLeft = timeToRecord;
        }
    }

    void PhysicsSystem::DisconnectFromPVD()
    {
        KRG_ASSERT( m_pPVD != nullptr );
        if ( m_pPVD->isConnected() )
        {
            m_pPVD->disconnect();
        }
    }

    void PhysicsSystem::UpdatePVD( Seconds TimeDelta )
    {
        if ( m_recordingTimeLeft >= 0 )
        {
            if ( m_pPVD->isConnected() )
            {
                m_recordingTimeLeft -= TimeDelta;
                if ( m_recordingTimeLeft < 0 )
                {
                    m_pPVD->disconnect();
                    m_recordingTimeLeft = -1.0f;
                }
            }
            else
            {
                m_recordingTimeLeft = -1.0f;
            }
        }
    }

    bool PhysicsSystem::IsDebugDrawingEnabled() const
    {
        return m_sceneDebugFlags && ( 1 << physx::PxVisualizationParameter::eSCALE ) != 0;
    }

    void PhysicsSystem::SetDebugFlags( uint32 debugFlags )
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