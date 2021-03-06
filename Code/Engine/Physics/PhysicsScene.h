#pragma once

#include "PhysX.h"
#include "PxScene.h"
#include "System/Core/Time/Time.h"

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

    public:

        PhysicsScene( physx::PxPhysics& physics, physx::PxCpuDispatcher* pDispatcher, physx::PxSceneQueryFilterCallback* pOptionalQueryFilter = nullptr );
        ~PhysicsScene();

        inline physx::PxPhysics& GetPxPhysics() { return m_pScene->getPhysics(); }
        inline physx::PxScene* GetPxScene() { return m_pScene; }

        void Simulate( Seconds deltaTime );

        // Locks
        //-------------------------------------------------------------------------

        inline void AcquireReadLock() { m_pScene->lockRead(); m_readLockAcquired = true; }
        inline void ReleaseReadLock() { m_pScene->unlockRead(); m_readLockAcquired = false; }

        inline void AcquireWriteLock() { m_pScene->lockWrite(); m_writeLockAcquired = true;}
        inline void ReleaseWriteLock() { m_pScene->unlockWrite(); m_writeLockAcquired = false; }

        // Queries
        //-------------------------------------------------------------------------
        // None of these function acquire read locks, the user is expected to manually lock/unlock the scene, since they are often doing more than one query

        // Rays
        bool RayCast( Vector const& start, Vector const& end, QueryRules const& rules );
        bool RayCast( Vector const& start, Vector const& unitDirection, float distance, QueryRules const& rules );
        
        // Spheres
        bool SphereCast( float radius, Vector const& start, Vector const& end, QueryRules const& rules );
        bool SphereCast( float radius, Vector const& start, Vector const& unitDirection, float distance, QueryRules const& rules );
        bool SphereOverlap( float radius, Vector const& position, QueryRules const& rules );
        
        // Capsules - Half-height is always along Z axis
        bool CapsuleCast( float halfHeight, float radius, Quaternion const& orientation, Vector const& start, Vector const& end, QueryRules const& rules );
        bool CapsuleCast( float halfHeight, float radius, Quaternion const& orientation, Vector const& start, Vector const& unitDirection, float distance, QueryRules const& rules );
        bool CapsuleOverlap( float halfHeight, float radius, Quaternion const& orientation, Vector const& position, QueryRules const& rules );

        // Cylinders - Half-height is always along Z axis
        bool CylinderCast( float halfHeight, float radius, Quaternion const& orientation, Vector const& start, Vector const& end, QueryRules const& rules );
        bool CylinderCast( float halfHeight, float radius, Quaternion const& orientation, Vector const& start, Vector const& unitDirection, float distance, QueryRules const& rules );
        bool CylinderOverlap( float halfHeight, float radius, Quaternion const& orientation, Vector const& position, QueryRules const& rules );

        // Boxes
        bool BoxCast( Vector halfExtents, Vector const& position, Quaternion const& orientation, Vector const& start, Vector const& end, QueryRules const& rules );
        bool BoxCast( Vector halfExtents, Vector const& position, Quaternion const& orientation, Vector const& start, Vector const& unitDirection, float distance, QueryRules const& rules );
        bool BoxOverlap( Vector halfExtents, Vector const& position, Quaternion const& orientation, QueryRules const& rules );
        
        // Shapes
        bool ShapeCast( physx::PxShape* pShape, Transform const& transform, Vector const& desiredEndPosition, QueryRules const& rules );
        bool ShapeCast( physx::PxShape* pShape, Transform const& transform, Vector const& unitDirection, float distance, QueryRules const& rules );
        bool ShapeOverlap( physx::PxShape* pShape, Transform const& transform, QueryRules const& rules );

        // Debug
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        inline void SetDebugName( char const* pName ) { m_debugName = pName; }
        inline String const& GetDebugName() const { return m_debugName; }
        inline uint32 GetDebugFlags() const { return m_sceneDebugFlags; }
        void SetDebugFlags( uint32 debugFlags );

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
        uint32                                          m_sceneDebugFlags = 0;
        float                                           m_debugDrawDistance = 10.0f;
        std::atomic<bool>                               m_readLockAcquired = false;     // Assertion helper
        std::atomic<bool>                               m_writeLockAcquired = false;    // Assertion helper
        #endif
    };

    //-------------------------------------------------------------------------
    // Physics Scene Lock
    //-------------------------------------------------------------------------
    // Take a scoped read/write lock on a specific physics scene

    class SceneScopeLock
    {
    public:

        enum class Type { Read, Write };

        inline SceneScopeLock( PhysicsScene& scene, Type type )
            : m_scene( scene )
            , m_type( type )
        {
            if ( m_type == Type::Read )
            {
                m_scene.AcquireReadLock();
            }
            else
            {
                m_scene.AcquireWriteLock();
            }
        }

        inline SceneScopeLock( PhysicsScene* pScene, Type type )
            : SceneScopeLock( *pScene, type )
        {}

        inline ~SceneScopeLock()
        {
            if ( m_type == Type::Read )
            {
                m_scene.ReleaseReadLock();
            }
            else
            {

                m_scene.ReleaseWriteLock();
            }
        }

        SceneScopeLock( SceneScopeLock const& ) = delete;
        SceneScopeLock( SceneScopeLock&& ) = delete;
        SceneScopeLock& operator=( SceneScopeLock const& ) = delete;
        SceneScopeLock& operator=( SceneScopeLock&& ) = delete;

    private:

        PhysicsScene&   m_scene;
        Type            m_type;
    };
}