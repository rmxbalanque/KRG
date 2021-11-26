#pragma once

#include "_Module/API.h"
#include "PhysicsQuery.h"
#include "Engine/Core/Entity/EntityWorldSystem.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/Core/Systems/ISystem.h"
#include "System/Core/Types/IDVector.h"
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

        struct EntityPhysicsRecord
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
        // The Custom versions of the queries allow you to provide your own result container, generally only useful if you hit the 32 hit limit the default results provides
        // WARNING!!! None of these function acquire read locks, the user is expected to manually lock/unlock the scene, since they are often doing more than one query

        bool RayCastCustom( Vector const& start, Vector const& end, QueryFilter& filter, physx::PxRaycastCallback& outResults );
        bool RayCastCustom( Vector const& start, Vector const& unitDirection, float distance, QueryFilter& filter, physx::PxRaycastCallback& outResults );
        KRG_FORCE_INLINE bool RayCast( Vector const& start, Vector const& end, QueryFilter& filter, RayCastResults& outResults ) { return RayCastCustom( start, end, filter, outResults ); }
        KRG_FORCE_INLINE bool RayCast( Vector const& start, Vector const& unitDirection, float distance, QueryFilter& filter, RayCastResults& outResults ) { return RayCastCustom( start, unitDirection, distance, filter, outResults ); }

        // Spheres
        bool SphereCastCustom( float radius, Vector const& start, Vector const& end, QueryFilter& filter, physx::PxSweepCallback& outResults );
        bool SphereCastCustom( float radius, Vector const& start, Vector const& unitDirection, float distance, QueryFilter& filter, physx::PxSweepCallback& outResults );
        bool SphereOverlapCustom( float radius, Vector const& position, QueryFilter& filter, physx::PxOverlapCallback& outResults );

        KRG_FORCE_INLINE bool SphereCast( float radius, Vector const& start, Vector const& end, QueryFilter& filter, ShapeCastResults& outResults ) { return SphereCastCustom( radius, start, end, filter, outResults ); }
        KRG_FORCE_INLINE bool SphereCast( float radius, Vector const& start, Vector const& unitDirection, float distance, QueryFilter& filter, ShapeCastResults& outResults ) { return SphereCastCustom( radius, start, unitDirection, distance, filter, outResults ); }
        KRG_FORCE_INLINE bool SphereOverlap( float radius, Vector const& position, QueryFilter& filter, OverlapResults& outResults ) { return SphereOverlapCustom( radius, position, filter, outResults ); }

        // Capsules - Half-height is always along Z axis
        bool CapsuleCastCustom( float halfHeight, float radius, Quaternion const& orientation, Vector const& start, Vector const& end, QueryFilter& filter, physx::PxSweepCallback& outResults );
        bool CapsuleCastCustom( float halfHeight, float radius, Quaternion const& orientation, Vector const& start, Vector const& unitDirection, float distance, QueryFilter& filter, physx::PxSweepCallback& outResults );
        bool CapsuleOverlapCustom( float halfHeight, float radius, Quaternion const& orientation, Vector const& position, QueryFilter& filter, physx::PxOverlapCallback& outResults );

        KRG_FORCE_INLINE bool CapsuleCast( float halfHeight, float radius, Quaternion const& orientation, Vector const& start, Vector const& end, QueryFilter& filter, ShapeCastResults& outResults ) { return CapsuleCastCustom( halfHeight, radius, orientation, start, end, filter, outResults ); }
        KRG_FORCE_INLINE bool CapsuleCast( float halfHeight, float radius, Quaternion const& orientation, Vector const& start, Vector const& unitDirection, float distance, QueryFilter& filter, ShapeCastResults& outResults ) { return CapsuleCastCustom( halfHeight, radius, orientation, start, unitDirection, distance, filter, outResults ); }
        KRG_FORCE_INLINE bool CapsuleOverlap( float halfHeight, float radius, Quaternion const& orientation, Vector const& position, QueryFilter& filter, OverlapResults& outResults ) { return CapsuleOverlapCustom( halfHeight, radius, orientation, position, filter, outResults ); }

        // Cylinders - Half-height is always along Z axis
        bool CylinderCastCustom( float halfHeight, float radius, Quaternion const& orientation, Vector const& start, Vector const& end, QueryFilter& filter, physx::PxSweepCallback& outResults );
        bool CylinderCastCustom( float halfHeight, float radius, Quaternion const& orientation, Vector const& start, Vector const& unitDirection, float distance, QueryFilter& filter, physx::PxSweepCallback& outResults );
        bool CylinderOverlapCustom( float halfHeight, float radius, Quaternion const& orientation, Vector const& position, QueryFilter& filter, physx::PxOverlapCallback& outResults );

        KRG_FORCE_INLINE bool CylinderCast( float halfHeight, float radius, Quaternion const& orientation, Vector const& start, Vector const& end, QueryFilter& filter, ShapeCastResults& outResults ) { return CylinderCastCustom( halfHeight, radius, orientation, start, end, filter, outResults ); }
        KRG_FORCE_INLINE bool CylinderCast( float halfHeight, float radius, Quaternion const& orientation, Vector const& start, Vector const& unitDirection, float distance, QueryFilter& filter, ShapeCastResults& outResults ) { return CylinderCastCustom( halfHeight, radius, orientation, start, unitDirection, distance, filter, outResults ); }
        KRG_FORCE_INLINE bool CylinderOverlap( float halfHeight, float radius, Quaternion const& orientation, Vector const& position, QueryFilter& filter, OverlapResults& outResults ) { return CylinderOverlapCustom( halfHeight, radius, orientation, position, filter, outResults ); }

        // Boxes
        bool BoxCastCustom( Vector halfExtents, Vector const& position, Quaternion const& orientation, Vector const& start, Vector const& end, QueryFilter& filter, physx::PxSweepCallback& outResults );
        bool BoxCastCustom( Vector halfExtents, Vector const& position, Quaternion const& orientation, Vector const& start, Vector const& unitDirection, float distance, QueryFilter& filter, physx::PxSweepCallback& outResults );
        bool BoxOverlapCustom( Vector halfExtents, Vector const& position, Quaternion const& orientation, QueryFilter& filter, physx::PxOverlapCallback& outResults );

        KRG_FORCE_INLINE bool BoxCast( Vector halfExtents, Vector const& position, Quaternion const& orientation, Vector const& start, Vector const& end, QueryFilter& filter, ShapeCastResults& outResults ) { return BoxCastCustom( halfExtents, position, orientation, start, end, filter, outResults ); }
        KRG_FORCE_INLINE bool BoxCast( Vector halfExtents, Vector const& position, Quaternion const& orientation, Vector const& start, Vector const& unitDirection, float distance, QueryFilter& filter, ShapeCastResults& outResults ) { return BoxCastCustom( halfExtents, position, orientation, start, unitDirection, distance, filter, outResults ); }
        KRG_FORCE_INLINE bool BoxOverlap( Vector halfExtents, Vector const& position, Quaternion const& orientation, QueryFilter& filter, OverlapResults& outResults ) { return BoxOverlapCustom( halfExtents, position, orientation, filter, outResults ); }

        // Shapes
        bool ShapeCastCustom( physx::PxShape* pShape, Transform const& startTransform, Vector const& desiredEndPosition, QueryFilter& filter, physx::PxSweepCallback& outResults );
        bool ShapeCastCustom( physx::PxShape* pShape, Transform const& startTransform, Vector const& unitDirection, float distance, QueryFilter& filter, physx::PxSweepCallback& outResults );
        bool ShapeOverlapCustom( physx::PxShape* pShape, Transform const& transform, QueryFilter& filter, physx::PxOverlapCallback& outResults );

        KRG_FORCE_INLINE bool ShapeCast( physx::PxShape* pShape, Transform const& startTransform, Vector const& desiredEndPosition, QueryFilter& filter, ShapeCastResults& outResults ) { return ShapeCastCustom( pShape, startTransform, desiredEndPosition, filter, outResults ); }
        KRG_FORCE_INLINE bool ShapeCast( physx::PxShape* pShape, Transform const& startTransform, Vector const& unitDirection, float distance, QueryFilter& filter, ShapeCastResults& outResults ) { return ShapeCastCustom( pShape, startTransform, unitDirection, distance, filter, outResults ); }
        KRG_FORCE_INLINE bool ShapeOverlap( physx::PxShape* pShape, Transform const& transform, QueryFilter& filter, OverlapResults& outResults ) { return ShapeOverlapCustom( pShape, transform, filter, outResults ); }

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

        TIDVector<UUID, PhysicsComponent*>              m_registeredPhysicsComponents;
        TIDVector<UUID, PhysicsComponent*>              m_dynamicComponents; // TODO: profile and see if we need to use a dynamic pool

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