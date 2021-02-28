#pragma once

#include "PhysX.h"
#include "PxScene.h"

//-------------------------------------------------------------------------

namespace physx
{
    class PxPhysics;
    class PxSimulationEventCallback;
}

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    struct QueryRules : public physx::PxQueryFilterData
    {
    };

    //-------------------------------------------------------------------------

    struct QueryResults
    {
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_PHYSICS_API PhysicsScene
    {
        friend class PhysicsSystem;
        static physx::PxConvexMesh* s_pUnitCylinderMesh;

    private:

        static void CreateUnitCylinderMesh( physx::PxPhysics& physics, physx::PxCooking* pCooking );
        static void DestroyUnitCylinderMesh();

    public:

        PhysicsScene( physx::PxPhysics& physics, physx::PxCpuDispatcher* pDispatcher, physx::PxSceneQueryFilterCallback* pOptionalQueryFilter = nullptr );
        ~PhysicsScene();

        inline physx::PxPhysics& GetPxPhysics() { return m_pScene->getPhysics(); }
        inline physx::PxScene* GetPxScene() { return m_pScene; }

        void Simulate();

        // Locks
        //-------------------------------------------------------------------------

        inline void AcquireReadLock() { m_pScene->lockRead(); }
        inline void ReleaseReadLock() { m_pScene->unlockRead(); }

        inline void AcquireWriteLock() { m_pScene->lockWrite(); }
        inline void ReleaseWriteLock() { m_pScene->unlockWrite(); }

        // Queries
        //-------------------------------------------------------------------------
        // None of these function acquire read locks, the user is expected to manually lock/unlock the scene, since they are often doing more than one query

        // Rays
        bool RayCast( Vector const& start, Vector const& end, QueryRules const& rules );
        bool RayCast( Vector const& start, Vector const& unitDirection, F32 distance, QueryRules const& rules );
        
        // Spheres
        bool SphereCast( F32 radius, Vector const& start, Vector const& end, QueryRules const& rules );
        bool SphereCast( F32 radius, Vector const& start, Vector const& unitDirection, F32 distance, QueryRules const& rules );
        bool SphereOverlap( F32 radius, Vector const& position, QueryRules const& rules );
        
        // Capsules - Half-height is always along Z axis
        bool CapsuleCast( F32 halfHeight, F32 radius, Quaternion const& orientation, Vector const& start, Vector const& end, QueryRules const& rules );
        bool CapsuleCast( F32 halfHeight, F32 radius, Quaternion const& orientation, Vector const& start, Vector const& unitDirection, F32 distance, QueryRules const& rules );
        bool CapsuleOverlap( F32 halfHeight, F32 radius, Quaternion const& orientation, Vector const& position, QueryRules const& rules );

        // Cylinders - Half-height is always along Z axis
        bool CylinderCast( F32 halfHeight, F32 radius, Quaternion const& orientation, Vector const& start, Vector const& end, QueryRules const& rules );
        bool CylinderCast( F32 halfHeight, F32 radius, Quaternion const& orientation, Vector const& start, Vector const& unitDirection, F32 distance, QueryRules const& rules );
        bool CylinderOverlap( F32 halfHeight, F32 radius, Quaternion const& orientation, Vector const& position, QueryRules const& rules );

        // Boxes
        bool BoxCast( Vector halfExtents, Vector const& position, Quaternion const& orientation, Vector const& start, Vector const& end, QueryRules const& rules );
        bool BoxCast( Vector halfExtents, Vector const& position, Quaternion const& orientation, Vector const& start, Vector const& unitDirection, F32 distance, QueryRules const& rules );
        bool BoxOverlap( Vector halfExtents, Vector const& position, Quaternion const& orientation, QueryRules const& rules );
        
        // Shapes
        bool ShapeCast( physx::PxShape* pShape, Transform const& transform, Vector const& desiredEndPosition, QueryRules const& rules );
        bool ShapeCast( physx::PxShape* pShape, Transform const& transform, Vector const& unitDirection, F32 distance, QueryRules const& rules );
        bool ShapeOverlap( physx::PxShape* pShape, Transform const& transform, QueryRules const& rules );

        // Debug
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        inline void SetDebugName( char const* pName ) { m_debugName = pName; }
        inline String const& GetDebugName() const { return m_debugName; }
        inline U32 GetDebugFlags() const { return m_sceneDebugFlags; }
        void SetDebugFlags( U32 debugFlags );

        inline bool IsDebugDrawingEnabled() const;
        inline float GetDebugDrawDistance() const { return m_debugDrawDistance; }
        inline void SetDebugDrawDistance( float drawDistance ) { m_debugDrawDistance = Math::Max( drawDistance, 0.0f ); }
        #endif

    private:

        physx::PxScene*                                 m_pScene = nullptr;
        physx::PxSimulationEventCallback*               m_pEventCallbackHandler = nullptr;
        physx::PxSceneQueryFilterCallback*              m_pQueryFilter = nullptr;

        #if KRG_DEVELOPMENT_TOOLS
        String                                          m_debugName;
        U32                                             m_sceneDebugFlags = 0;
        F32                                             m_debugDrawDistance = 10.0f;
        #endif
    };

    //-------------------------------------------------------------------------

    // Take a scoped read/write lock on a specific physics scene
    class SceneScopeLock
    {
    public:

        enum class Type { Read, Write };

        inline SceneScopeLock( PhysicsScene* pScene, Type type )
            : m_pScene( pScene )
            , m_type( type )
        {
            KRG_ASSERT( pScene != nullptr );

            if ( m_type == Type::Read )
            {
                m_pScene->AcquireReadLock();
            }
            else
            {
                m_pScene->AcquireWriteLock();
            }
        }

        inline ~SceneScopeLock()
        {
            if ( m_type == Type::Read )
            {
                m_pScene->ReleaseReadLock();
            }
            else
            {
                m_pScene->ReleaseWriteLock();
            }
        }

        SceneScopeLock( SceneScopeLock const& ) = delete;
        SceneScopeLock( SceneScopeLock&& ) = delete;
        SceneScopeLock& operator=( SceneScopeLock const& ) = delete;
        SceneScopeLock& operator=( SceneScopeLock&& ) = delete;

    private:

        PhysicsScene*   m_pScene = nullptr;
        Type            m_type;
    };
}