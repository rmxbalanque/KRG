#pragma once

#include "_Module/API.h"
#include "PhysicsQueryTypes.h"
#include "PhysicsQueryFilter.h"
#include "Engine/Core/Entity/EntityWorldSystem.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/Core/Systems/ISystem.h"
#include "PhysX.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class PhysicsSystem;
    class PhysicsComponent;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_PHYSICS_API PhysicsWorldSystem final : public IWorldEntitySystem
    {
        friend class PhysicsDebugView;
        friend class PhysicsRenderer;

        struct EntityPhysicsRecord : public EntityRegistryRecord
        {
            inline bool IsEmpty() const { return m_components.empty(); }

            TVector<PhysicsComponent*>                  m_components;
        };

    public:

        KRG_REGISTER_TYPE( PhysicsWorldSystem );
        KRG_ENTITY_WORLD_SYSTEM( PhysicsWorldSystem, RequiresUpdate( UpdateStage::PostPhysics ) );

    public:

        PhysicsWorldSystem() = default;
        PhysicsWorldSystem( PhysicsSystem& physicsSystem );

        // Locks
        //-------------------------------------------------------------------------

        inline void AcquireReadLock()
        {
            m_pScene->lockRead();
            KRG_DEVELOPMENT_TOOLS_ONLY( m_readLockAcquired = true );
        }

        inline void ReleaseReadLock()
        {
            m_pScene->unlockRead();
            KRG_DEVELOPMENT_TOOLS_ONLY( m_readLockAcquired = false );
        }

        inline void AcquireWriteLock()
        {
            m_pScene->lockWrite();
            KRG_DEVELOPMENT_TOOLS_ONLY( m_writeLockAcquired = true );
        }

        inline void ReleaseWriteLock()
        {
            m_pScene->unlockWrite();
            KRG_DEVELOPMENT_TOOLS_ONLY( m_writeLockAcquired = false );
        }

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
        inline uint32 GetDebugFlags() const { return m_sceneDebugFlags; }
        void SetDebugFlags( uint32 debugFlags );

        inline bool IsDebugDrawingEnabled() const;
        inline float GetDebugDrawDistance() const { return m_debugDrawDistance; }
        inline void SetDebugDrawDistance( float drawDistance ) { m_debugDrawDistance = Math::Max( drawDistance, 0.0f ); }
        #endif

    private:

        virtual void InitializeSystem( SystemRegistry const& systemRegistry ) override;
        virtual void ShutdownSystem() override final;
        virtual void RegisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;
        virtual void UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;
        virtual void UpdateSystem( EntityUpdateContext const& ctx ) override final;

        physx::PxRigidActor* CreateActor( PhysicsComponent* pComponent ) const;
        physx::PxShape* CreateShape( PhysicsComponent* pComponent, physx::PxRigidActor* pActor ) const;

    private:

        PhysicsSystem*                                  m_pPhysicsSystem = nullptr;
        physx::PxScene*                                 m_pScene = nullptr;
        QueryFilter                                     m_queryFilter;

        EntityRegistry<EntityPhysicsRecord>             m_registeredEntities;
        THashMap<UUID, PhysicsComponent*>               m_dynamicComponents; // TODO: profile and see if we need to use a dynamic pool

        #if KRG_DEVELOPMENT_TOOLS
        std::atomic<bool>                               m_readLockAcquired = false;     // Assertion helper
        std::atomic<bool>                               m_writeLockAcquired = false;    // Assertion helper

        bool                                            m_drawDynamicActorBounds = false;
        bool                                            m_drawKinematicActorBounds = false;

        uint32                                          m_sceneDebugFlags = 0;
        float                                           m_debugDrawDistance = 10.0f;
        #endif
    };
}