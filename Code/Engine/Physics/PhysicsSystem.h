#pragma once

#include "_Module/API.h"
#include "PhysX.h"
#include "PhysicsQueryFilter.h"
#include "PhysicsQueryTypes.h"
#include "PhysicsMaterial.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/Core/Systems/ISystem.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class PhysicsMaterialDatabase;

    //-------------------------------------------------------------------------
    // Physics System
    //-------------------------------------------------------------------------
    // This is effectively a singleton since PhysX only allows a single instance of the foundation class

    class KRG_ENGINE_PHYSICS_API PhysicsSystem : public ISystem
    {
        friend class ScopeLock;
        friend class PhysicsDebugViewController;

    public:

        KRG_SYSTEM_ID( PhysicsSystem );

    public:

        PhysicsSystem() = default;

        void Initialize();
        void Shutdown();
        void Update( UpdateContext& ctx );

        // PhysX accessors
        //-------------------------------------------------------------------------

        inline physx::PxPhysics& GetPxPhysics() const { return *m_pPhysics; }
        inline physx::PxScene* GetPxScene() const { return m_pScene; }

        // Locks
        //-------------------------------------------------------------------------

        inline void AcquireReadLock() { m_pScene->lockRead(); m_readLockAcquired = true; }
        inline void ReleaseReadLock() { m_pScene->unlockRead(); m_readLockAcquired = false; }

        inline void AcquireWriteLock() { m_pScene->lockWrite(); m_systemLock.lock(); m_writeLockAcquired = true; }
        inline void ReleaseWriteLock() { m_pScene->unlockWrite(); m_systemLock.unlock(); m_writeLockAcquired = false; }

        // Physic Materials
        //-------------------------------------------------------------------------

        void FillMaterialDatabase( TVector<PhysicsMaterialSettings> const& materials );
        void ClearMaterialDatabase();
        physx::PxMaterial* GetMaterial( StringID materialID ) const;

        // Queries
        //-------------------------------------------------------------------------
        // None of these function acquire read locks, the user is expected to manually lock/unlock the scene, since they are often doing more than one query

        // Rays
        bool RayCast( Vector const& start, Vector const& end, QueryRules const& rules, physx::PxRaycastCallback& outResults );
        bool RayCast( Vector const& start, Vector const& unitDirection, float distance, QueryRules const& rules, physx::PxRaycastCallback& outResults );

        // Spheres
        bool SphereCast( float radius, Vector const& start, Vector const& end, QueryRules const& rules, physx::PxSweepCallback& outResults );
        bool SphereCast( float radius, Vector const& start, Vector const& unitDirection, float distance, QueryRules const& rules, physx::PxSweepCallback& outResults );
        bool SphereOverlap( float radius, Vector const& position, QueryRules const& rules, physx::PxOverlapCallback& outResults );

        // Capsules - Half-height is always along Z axis
        bool CapsuleCast( float halfHeight, float radius, Quaternion const& orientation, Vector const& start, Vector const& end, QueryRules const& rules, physx::PxSweepCallback& outResults );
        bool CapsuleCast( float halfHeight, float radius, Quaternion const& orientation, Vector const& start, Vector const& unitDirection, float distance, QueryRules const& rules, physx::PxSweepCallback& outResults );
        bool CapsuleOverlap( float halfHeight, float radius, Quaternion const& orientation, Vector const& position, QueryRules const& rules, physx::PxOverlapCallback& outResults );

        // Cylinders - Half-height is always along Z axis
        bool CylinderCast( float halfHeight, float radius, Quaternion const& orientation, Vector const& start, Vector const& end, QueryRules const& rules, physx::PxSweepCallback& outResults );
        bool CylinderCast( float halfHeight, float radius, Quaternion const& orientation, Vector const& start, Vector const& unitDirection, float distance, QueryRules const& rules, physx::PxSweepCallback& outResults );
        bool CylinderOverlap( float halfHeight, float radius, Quaternion const& orientation, Vector const& position, QueryRules const& rules, physx::PxOverlapCallback& outResults );

        // Boxes
        bool BoxCast( Vector halfExtents, Vector const& position, Quaternion const& orientation, Vector const& start, Vector const& end, QueryRules const& rules, physx::PxSweepCallback& outResults );
        bool BoxCast( Vector halfExtents, Vector const& position, Quaternion const& orientation, Vector const& start, Vector const& unitDirection, float distance, QueryRules const& rules, physx::PxSweepCallback& outResults );
        bool BoxOverlap( Vector halfExtents, Vector const& position, Quaternion const& orientation, QueryRules const& rules, physx::PxOverlapCallback& outResults );

        // Shapes
        bool ShapeCast( physx::PxShape* pShape, Transform const& startTransform, Vector const& desiredEndPosition, QueryRules const& rules, physx::PxSweepCallback& outResults );
        bool ShapeCast( physx::PxShape* pShape, Transform const& startTransform, Vector const& unitDirection, float distance, QueryRules const& rules, physx::PxSweepCallback& outResults );
        bool ShapeOverlap( physx::PxShape* pShape, Transform const& transform, QueryRules const& rules, physx::PxOverlapCallback& outResults );

        // Debug
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        bool IsConnectedToPVD();
        void ConnectToPVD( Seconds timeToRecord = -1.0f );
        void DisconnectFromPVD();
        void UpdatePVD( Seconds TimeDelta );

        inline uint32 GetDebugFlags() const { return m_sceneDebugFlags; }
        void SetDebugFlags( uint32 debugFlags );

        inline bool IsDebugDrawingEnabled() const;
        inline float GetDebugDrawDistance() const { return m_debugDrawDistance; }
        inline void SetDebugDrawDistance( float drawDistance ) { m_debugDrawDistance = Math::Max( drawDistance, 0.0f ); }
        #endif

    private:

        void CreateSharedMeshes();
        void DestroySharedMeshes();

        // System Lock
        //-------------------------------------------------------------------------
        // This is used to ensure thread safety for any system operations like creating resources, etc...

        inline Threading::Mutex& GetSystemMutex() { return m_systemLock; }

    private:

        physx::PxFoundation*                            m_pFoundation = nullptr;
        physx::PxPhysics*                               m_pPhysics = nullptr;
        physx::PxCooking*                               m_pCooking = nullptr;
        physx::PxCpuDispatcher*                         m_pDispatcher = nullptr;
        physx::PxAllocatorCallback*                     m_pAllocatorCallback = nullptr;
        physx::PxErrorCallback*                         m_pErrorCallback = nullptr;

        physx::PxScene*                                 m_pScene = nullptr;
        physx::PxSimulationEventCallback*               m_pEventCallbackHandler = nullptr;
        QueryFilter                                     m_queryFilter;

        THashMap<StringID, PhysicsMaterial>             m_materials;

        Threading::Mutex                                m_systemLock;                   // Additional system lock for any scene writes or actor/shape creation

        #if KRG_DEVELOPMENT_TOOLS
        physx::PxPvd*                                   m_pPVD = nullptr;
        physx::PxPvdTransport*                          m_pPVDTransport = nullptr;
        Seconds                                         m_recordingTimeLeft = -1.0f;

        uint32                                          m_sceneDebugFlags = 0;
        float                                           m_debugDrawDistance = 10.0f;
        std::atomic<bool>                               m_readLockAcquired = false;     // Assertion helper
        std::atomic<bool>                               m_writeLockAcquired = false;    // Assertion helper
        #endif
    };

    //-------------------------------------------------------------------------
    // Physics System Lock
    //-------------------------------------------------------------------------
    // Take a scoped read/write lock on the physics system

    class [[nodiscard]] ScopeLock
    {
    public:

        enum class Type { Read, Write };

        inline ScopeLock( PhysicsSystem& system, Type type )
            : m_system( system )
            , m_type( type )
        {
            if ( m_type == Type::Read )
            {
                m_system.AcquireReadLock();
            }
            else
            {
                m_system.AcquireWriteLock();
            }
        }

        inline ScopeLock( PhysicsSystem* pSystem, Type type )
            : ScopeLock( *pSystem, type )
        {}

        inline ~ScopeLock()
        {
            if ( m_type == Type::Read )
            {
                m_system.ReleaseReadLock();
            }
            else
            {

                m_system.ReleaseWriteLock();
            }
        }

        ScopeLock( ScopeLock const& ) = delete;
        ScopeLock( ScopeLock&& ) = delete;
        ScopeLock& operator=( ScopeLock const& ) = delete;
        ScopeLock& operator=( ScopeLock&& ) = delete;

    private:

        PhysicsSystem&      m_system;
        Type                m_type;
    };
}