#include "PhysicsScene.h"
#include "System/Core/Profiling/Profiling.h"

//-------------------------------------------------------------------------

using namespace physx;

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    namespace
    {
        constexpr static S32 const g_hitBufferSize = 256;
    }

    //-------------------------------------------------------------------------

    physx::PxConvexMesh* PhysicsScene::s_pUnitCylinderMesh = nullptr;

    void PhysicsScene::CreateUnitCylinderMesh( physx::PxPhysics& physics, physx::PxCooking* pCooking )
    {
        KRG_ASSERT( pCooking != nullptr );

        //-------------------------------------------------------------------------

        static const PxVec3 convexVerts[] = { PxVec3( 0,1,0 ),PxVec3( 1,0,0 ),PxVec3( -1,0,0 ),PxVec3( 0,0,1 ), PxVec3( 0,0,-1 ) };

        //-------------------------------------------------------------------------

        PxConvexMeshDesc convexDesc;
        convexDesc.points.count = 5;
        convexDesc.points.stride = sizeof( PxVec3 );
        convexDesc.points.data = convexVerts;
        convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

        PxDefaultMemoryOutputStream buf;
        PxConvexMeshCookingResult::Enum result;
        if ( !pCooking->cookConvexMesh( convexDesc, buf, &result ) )
        {
            KRG_UNREACHABLE_CODE();
        }

        //-------------------------------------------------------------------------

        PxDefaultMemoryInputData input( buf.getData(), buf.getSize() );
        s_pUnitCylinderMesh = physics.createConvexMesh( input );
        KRG_ASSERT( s_pUnitCylinderMesh != nullptr );
    }

    void PhysicsScene::DestroyUnitCylinderMesh()
    {
        s_pUnitCylinderMesh->release();
        s_pUnitCylinderMesh = nullptr;
    }

    //-------------------------------------------------------------------------

    PhysicsScene::PhysicsScene( physx::PxPhysics& physics, physx::PxCpuDispatcher* pDispatcher, physx::PxSceneQueryFilterCallback* pOptionalQueryFilter )
        : m_pQueryFilter( pOptionalQueryFilter )
    {
        KRG_ASSERT( pDispatcher != nullptr );

        // Create PhysX scene
        //-------------------------------------------------------------------------

        auto const& tolerancesScale = physics.getTolerancesScale();

        KRG_ASSERT( m_pScene == nullptr );
        PxSceneDesc sceneDesc( tolerancesScale );
        sceneDesc.gravity = ToPx( Constants::Gravity );
        sceneDesc.cpuDispatcher = pDispatcher;
        sceneDesc.filterShader = PxDefaultSimulationFilterShader;
        m_pScene = physics.createScene( sceneDesc );

        // PVD
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        auto pPvdClient = m_pScene->getScenePvdClient();
        pPvdClient->setScenePvdFlag( PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true );
        pPvdClient->setScenePvdFlag( PxPvdSceneFlag::eTRANSMIT_CONTACTS, true );
        pPvdClient->setScenePvdFlag( PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true );
        #endif

        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        SetDebugFlags( 1 << PxVisualizationParameter::eCOLLISION_SHAPES );
        #endif
    }

    PhysicsScene::~PhysicsScene()
    {
        if ( m_pScene != nullptr )
        {
            m_pScene->release();
            m_pScene = nullptr;
        }
    }

    void PhysicsScene::Simulate()
    {
        {
            KRG_PROFILE_SCOPE_PHYSICS( "Simulate" );
            m_pScene->simulate( 1.0f / 60.0f );
        }

        {
            KRG_PROFILE_SCOPE_PHYSICS( "Fetch Results" );
            m_pScene->fetchResults( true );
        }
    }
}

// Queries
//-------------------------------------------------------------------------

namespace KRG::Physics
{
    bool PhysicsScene::RayCast( Vector const& start, Vector const& unitDirection, F32 distance, QueryRules const& rules )
    {
        KRG_ASSERT( unitDirection.IsNormalized3() && distance > 0 );

        PxRaycastHit hits[256];
        PxRaycastBuffer hitBuffer( hits, 256 );
        bool const result = m_pScene->raycast( ToPx( start ), ToPx( unitDirection ), distance, hitBuffer, PxHitFlag::eDEFAULT, rules, m_pQueryFilter );

        // TODO: return hit results

        return result;
    }

    bool PhysicsScene::RayCast( Vector const& start, Vector const& end, QueryRules const& rules )
    {
        Vector dir, length;
        ( end - start ).ToDirectionAndLength3( dir, length );
        KRG_ASSERT( !dir.IsNearZero3() );

        return RayCast( start, dir, length.x, rules );
    }

    //-------------------------------------------------------------------------

    bool PhysicsScene::SphereCast( F32 radius, Vector const& start, Vector const& unitDirection, F32 distance, QueryRules const& rules )
    {
        KRG_ASSERT( unitDirection.IsNormalized3() && distance > 0 );

        PxSweepHit hits[g_hitBufferSize];
        PxSweepBuffer sweepBuffer( hits, g_hitBufferSize );

        PxSphereGeometry const sphereGeo( radius );
        bool const result = m_pScene->sweep( sphereGeo, PxTransform( ToPx( start ) ), ToPx( unitDirection ), distance, sweepBuffer, PxHitFlag::eDEFAULT, rules, m_pQueryFilter );

        // TODO: return hit results

        return result;
    }

    bool PhysicsScene::SphereCast( F32 radius, Vector const& start, Vector const& end, QueryRules const& rules )
    {
        Vector dir, length;
        ( end - start ).ToDirectionAndLength3( dir, length );
        KRG_ASSERT( !dir.IsNearZero3() );

        return SphereCast( radius, start, dir, length.x, rules );
    }

    bool PhysicsScene::SphereOverlap( F32 radius, Vector const& position, QueryRules const& rules )
    {
        PxOverlapHit hits[g_hitBufferSize];
        PxOverlapBuffer sweepBuffer( hits, g_hitBufferSize );

        PxSphereGeometry const sphereGeo( radius );
        bool const result = m_pScene->overlap( sphereGeo, PxTransform( ToPx( position ) ), sweepBuffer, rules, m_pQueryFilter );

        // TODO: return hit results

        return result;
    }

    //-------------------------------------------------------------------------

    bool PhysicsScene::CapsuleCast( F32 halfHeight, F32 radius, Quaternion const& orientation, Vector const& start, Vector const& unitDirection, F32 distance, QueryRules const& rules )
    {
        KRG_ASSERT( unitDirection.IsNormalized3() && distance > 0 );

        PxSweepHit hits[g_hitBufferSize];
        PxSweepBuffer sweepBuffer( hits, g_hitBufferSize );

        PxCapsuleGeometry const capsuleGeo( radius, halfHeight );
        bool const result = m_pScene->sweep( capsuleGeo, PxTransform( ToPx( start ), ToPx( orientation ) ), ToPx( unitDirection ), distance, sweepBuffer, PxHitFlag::eDEFAULT, rules, m_pQueryFilter );

        // TODO: return hit results

        return result;
    }

    bool PhysicsScene::CapsuleCast( F32 halfHeight, F32 radius, Quaternion const& orientation, Vector const& start, Vector const& end, QueryRules const& rules )
    {
        Vector dir, length;
        ( end - start ).ToDirectionAndLength3( dir, length );
        KRG_ASSERT( !dir.IsNearZero3() );

        return CapsuleCast( halfHeight, radius, orientation, start, dir, length.x, rules );
    }

    bool PhysicsScene::CapsuleOverlap( F32 halfHeight, F32 radius, Quaternion const& orientation, Vector const& position, QueryRules const& rules )
    {
        PxOverlapHit hits[g_hitBufferSize];
        PxOverlapBuffer sweepBuffer( hits, g_hitBufferSize );

        PxConvexMeshGeometry const cylinderGeo( s_pUnitCylinderMesh, PxMeshScale( PxVec3( radius, radius, halfHeight ) ) );
        bool result = m_pScene->overlap( cylinderGeo, PxTransform( ToPx( position ), ToPx( orientation ) ), sweepBuffer, rules, m_pQueryFilter );

        // TODO: return hit results

        return result;
    }

    //-------------------------------------------------------------------------

    bool PhysicsScene::CylinderCast( F32 halfHeight, F32 radius, Quaternion const& orientation, Vector const& start, Vector const& unitDirection, F32 distance, QueryRules const& rules )
    {
        KRG_ASSERT( unitDirection.IsNormalized3() && distance > 0 );
        KRG_ASSERT( s_pUnitCylinderMesh != nullptr );

        PxSweepHit hits[g_hitBufferSize];
        PxSweepBuffer sweepBuffer( hits, g_hitBufferSize );

        PxConvexMeshGeometry const cylinderGeo( s_pUnitCylinderMesh, PxMeshScale( PxVec3( radius, radius, halfHeight ) ) );
        bool const result = m_pScene->sweep( cylinderGeo, PxTransform( ToPx( start ), ToPx( orientation ) ), ToPx( unitDirection ), distance, sweepBuffer, PxHitFlag::eDEFAULT, rules, m_pQueryFilter );

        // TODO: return hit results

        return result;
    }

    bool PhysicsScene::CylinderCast( F32 halfHeight, F32 radius, Quaternion const& orientation, Vector const& start, Vector const& end, QueryRules const& rules )
    {
        Vector dir, length;
        ( end - start ).ToDirectionAndLength3( dir, length );
        KRG_ASSERT( !dir.IsNearZero3() );

        return CylinderCast( halfHeight, radius, orientation, start, dir, length.x, rules );
    }

    bool PhysicsScene::CylinderOverlap( F32 halfHeight, F32 radius, Quaternion const& orientation, Vector const& position, QueryRules const& rules )
    {
        PxOverlapHit hits[g_hitBufferSize];
        PxOverlapBuffer sweepBuffer( hits, g_hitBufferSize );

        PxCapsuleGeometry const capsuleGeo( radius, halfHeight );
        bool const result = m_pScene->overlap( capsuleGeo, PxTransform( ToPx( position ), ToPx( orientation ) ), sweepBuffer, rules, m_pQueryFilter );

        // TODO: return hit results

        return result;
    }

    //-------------------------------------------------------------------------

    bool PhysicsScene::BoxCast( Vector halfExtents, Vector const& position, Quaternion const& orientation, Vector const& start, Vector const& unitDirection, F32 distance, QueryRules const& rules )
    {
        KRG_ASSERT( unitDirection.IsNormalized3() && distance > 0 );

        PxSweepHit hits[g_hitBufferSize];
        PxSweepBuffer sweepBuffer( hits, g_hitBufferSize );

        PxBoxGeometry const boxGeo( ToPx( halfExtents ) );
        bool const result = m_pScene->sweep( boxGeo, PxTransform( ToPx( start ), ToPx( orientation ) ), ToPx( unitDirection ), distance, sweepBuffer, PxHitFlag::eDEFAULT, rules, m_pQueryFilter );

        // TODO: return hit results

        return result;
    }

    bool PhysicsScene::BoxCast( Vector halfExtents, Vector const& position, Quaternion const& orientation, Vector const& start, Vector const& end, QueryRules const& rules )
    {
        Vector dir, length;
        ( end - start ).ToDirectionAndLength3( dir, length );
        KRG_ASSERT( !dir.IsNearZero3() );

        return BoxCast( halfExtents, position, orientation, start, dir, length.x, rules );
    }

    bool PhysicsScene::BoxOverlap( Vector halfExtents, Vector const& position, Quaternion const& orientation, QueryRules const& rules )
    {
        PxOverlapHit hits[g_hitBufferSize];
        PxOverlapBuffer sweepBuffer( hits, g_hitBufferSize );

        PxBoxGeometry const boxGeo( ToPx( halfExtents ) );
        bool const result = m_pScene->overlap( boxGeo, PxTransform( ToPx( position ), ToPx( orientation ) ), sweepBuffer, rules, m_pQueryFilter );

        // TODO: return hit results

        return result;
    }

    //-------------------------------------------------------------------------

    bool PhysicsScene::ShapeCast( physx::PxShape* pShape, Transform const& transform, Vector const& desiredEndPosition, QueryRules const& rules )
    {
        Vector dir, length;
        ( desiredEndPosition - transform.GetTranslation() ).ToDirectionAndLength3( dir, length );
        KRG_ASSERT( !dir.IsNearZero3() );

        return ShapeCast( pShape, transform, dir, length.x, rules );
    }

    bool PhysicsScene::ShapeCast( physx::PxShape* pShape, Transform const& transform, Vector const& unitDirection, F32 distance, QueryRules const& rules )
    {
        KRG_ASSERT( unitDirection.IsNormalized3() && distance > 0 );

        PxSweepHit hits[g_hitBufferSize];
        PxSweepBuffer sweepBuffer( hits, g_hitBufferSize );
        bool const result = m_pScene->sweep( pShape->getGeometry().any(), ToPx( transform ), ToPx( unitDirection ), distance, sweepBuffer, PxHitFlag::eDEFAULT, rules, m_pQueryFilter );

        return result;
    }

    bool PhysicsScene::ShapeOverlap( physx::PxShape* pShape, Transform const& transform, QueryRules const& rules )
    {
        PxOverlapHit hits[g_hitBufferSize];
        PxOverlapBuffer sweepBuffer( hits, g_hitBufferSize );

        bool const result = m_pScene->overlap( pShape->getGeometry().any(), ToPx( transform ), sweepBuffer, rules, m_pQueryFilter );

        // TODO: return hit results

        return result;
    }
}

// Debug
//-------------------------------------------------------------------------

namespace KRG::Physics
{
    #if KRG_DEVELOPMENT_TOOLS
    bool PhysicsScene::IsDebugDrawingEnabled() const
    {
        return m_sceneDebugFlags && ( 1 << physx::PxVisualizationParameter::eSCALE ) != 0;
    }

    void PhysicsScene::SetDebugFlags( U32 debugFlags )
    {
        KRG_ASSERT( m_pScene != nullptr );
        m_sceneDebugFlags = debugFlags;

        //-------------------------------------------------------------------------

        auto SetVisualizationParameter = [this] ( PxVisualizationParameter::Enum flag, F32 onValue, F32 offValue )
        {
            bool const isFlagSet = ( m_sceneDebugFlags & ( 1 << flag ) ) != 0;
            m_pScene->setVisualizationParameter( flag, isFlagSet ? onValue : offValue );
        };

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
    }
    #endif
}