#pragma once

#include "Engine/Physics/_Module/API.h"
#include "Engine/Physics/PhysicsQuery.h"
#include "Engine/Physics/PhysX.h"
#include "Engine/Core/Entity/EntityWorldSystem.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/Core/Systems/ISystem.h"
#include "System/Core/Types/IDVector.h"
#include "System/Core/Types/ScopedValue.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class PhysicsSystem;
    class PhysicsShapeComponent;
    class CharacterComponent;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_PHYSICS_API PhysicsWorldSystem final : public IWorldEntitySystem
    {
        friend class PhysicsDebugView;
        friend class PhysicsRenderer;

        // The distance that the shape is pushed away from a detected collision after a sweep - currently set to 5mm as that is a relatively standard value
        static constexpr float const s_sweepSeperationDistance = 0.005f;

        struct EntityPhysicsRecord
        {
            inline bool IsEmpty() const { return m_components.empty(); }

            TVector<PhysicsShapeComponent*>                  m_components;
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
        // The  versions of the queries allow you to provide your own result container, generally only useful if you hit the 32 hit limit the default results provides
        // NOTE!!! Make sure to always use the Physics world transform for the shape and not the component transforms directly!!!!
        // WARNING!!! None of these function acquire read locks, the user is expected to manually lock/unlock the scene, since they are often doing more than one query

        template<int N>
        bool RayCast( Vector const& start, Vector const& end, QueryFilter& filter, RayCastResultBuffer<N>& outResults )
        {
            KRG_ASSERT( m_readLockAcquired );

            outResults.m_start = start;
            outResults.m_end = end;

            Vector unitDirection, distance;
            ( end - start ).ToDirectionAndLength3( unitDirection, distance );
            KRG_ASSERT( !unitDirection.IsNearZero3() );
            return RayCast( start, unitDirection, distance.m_x, filter, outResults );
        }

        template<int N>
        bool RayCast( Vector const& start, Vector const& unitDirection, float distance, QueryFilter& filter, RayCastResultBuffer<N>& outResults )
        {
            KRG_ASSERT( m_readLockAcquired );
            KRG_ASSERT( unitDirection.IsNormalized3() && distance > 0 );

            outResults.m_start = start;
            outResults.m_end = Vector::MultiplyAdd( unitDirection, Vector( distance ), start );

            bool const result = m_pScene->raycast( ToPx( start ), ToPx( unitDirection ), distance, outResults, filter.m_hitFlags, filter.m_filterData, &filter );
            return result;
        }

        //-------------------------------------------------------------------------

        // Spheres
        template<int N>
        bool SphereSweep( float radius, Vector const& start, Vector const& end, QueryFilter& filter, SweepResultBuffer<N>& outResults )
        {
            KRG_ASSERT( m_readLockAcquired );

            outResults.m_sweepStart = start;
            outResults.m_sweepEnd = end;

            Vector unitDirection, distance;
            ( end - start ).ToDirectionAndLength3( unitDirection, distance );
            KRG_ASSERT( !unitDirection.IsNearZero3() );

            physx::PxSphereGeometry const sphereGeo( radius );
            bool const result = m_pScene->sweep( sphereGeo, physx::PxTransform( ToPx( start ) ), ToPx( unitDirection ), distance.m_x, outResults, filter.m_hitFlags, filter.m_filterData, &filter );
            outResults.CalculateFinalShapePosition( s_sweepSeperationDistance );
            return result;
        }

        template<int N>
        bool SphereSweep( float radius, Vector const& start, Vector const& unitDirection, float distance, QueryFilter& filter, SweepResultBuffer<N>& outResults )
        {
            KRG_ASSERT( m_readLockAcquired );
            KRG_ASSERT( unitDirection.IsNormalized3() && distance > 0 );

            outResults.m_sweepStart = start;
            outResults.m_sweepEnd = Vector::MultiplyAdd( unitDirection, Vector( distance ), start );

            physx::PxSphereGeometry const sphereGeo( radius );
            bool const result = m_pScene->sweep( sphereGeo, physx::PxTransform( ToPx( start ) ), ToPx( unitDirection ), distance, outResults, filter.m_hitFlags, filter.m_filterData, &filter );
            outResults.CalculateFinalShapePosition( s_sweepSeperationDistance );
            return result;
        }

        // Perform an overlap query. NOTE: This overlap may not agree with the results of a sweep query with regards to the initially overlapping state
        // Note: Overlap results will never have the block hit set!
        template<int N>
        bool SphereOverlap( float radius, Vector const& position, QueryFilter& filter, OverlapResultBuffer<N>& outResults )
        {
            KRG_ASSERT( m_readLockAcquired );

            outResults.m_position = position;

            // Set the no block value for overlaps
            TScopedGuardValue guard( filter.m_filterData.flags, filter.m_filterData.flags | physx::PxQueryFlag::eNO_BLOCK );
            physx::PxSphereGeometry const sphereGeo( radius );
            bool const result = m_pScene->overlap( sphereGeo, physx::PxTransform( ToPx( position ) ), outResults, filter.m_filterData, &filter );
            return result;
        }

        //-------------------------------------------------------------------------

        // Sweep a capsule
        // Note: the capsule half-height is along the X-axis
        template<int N>
        bool CapsuleSweep( float cylinderPortionHalfHeight, float radius, Quaternion const& orientation, Vector const& start, Vector const& end, QueryFilter& filter, SweepResultBuffer<N>& outResults )
        {
            KRG_ASSERT( m_readLockAcquired );

            outResults.m_sweepStart = start;
            outResults.m_sweepEnd = end;
            outResults.m_orientation = orientation;

            Vector unitDirection;
            float distance;
            ( end - start ).ToDirectionAndLength3( unitDirection, distance );
            KRG_ASSERT( !unitDirection.IsNearZero3() );

            physx::PxCapsuleGeometry const capsuleGeo( radius, cylinderPortionHalfHeight );
            physx::PxTransform Test( ToPx( start ), ToPx( orientation ) );
            KRG_ASSERT( Test.isValid() );
            bool const result = m_pScene->sweep( capsuleGeo, physx::PxTransform( ToPx( start ), ToPx( orientation ) ), ToPx( unitDirection ), distance, outResults, filter.m_hitFlags, filter.m_filterData, &filter );
            outResults.CalculateFinalShapePosition( s_sweepSeperationDistance );
            return result;
        }

        // Sweep a capsule
        // Note: the capsule half-height is along the X-axis
        template<int N>
        bool CapsuleSweep( float cylinderPortionHalfHeight, float radius, Quaternion const& orientation, Vector const& start, Vector const& unitDirection, float distance, QueryFilter& filter, SweepResultBuffer<N>& outResults )
        {
            KRG_ASSERT( m_readLockAcquired );
            KRG_ASSERT( unitDirection.IsNormalized3() && distance > 0 );

            outResults.m_sweepStart = start;
            outResults.m_sweepEnd = Vector::MultiplyAdd( unitDirection, Vector( distance ), start );
            outResults.m_orientation = orientation;

            physx::PxCapsuleGeometry const capsuleGeo( radius, cylinderPortionHalfHeight );
            physx::PxTransform Test( ToPx( start ), ToPx( orientation ) );
            KRG_ASSERT( Test.isValid() );
            bool const result = m_pScene->sweep( capsuleGeo, physx::PxTransform( ToPx( start ), ToPx( orientation ) ), ToPx( unitDirection ), distance, outResults, filter.m_hitFlags, filter.m_filterData, &filter );
            outResults.CalculateFinalShapePosition( s_sweepSeperationDistance );
            return result;
        }

        // Perform an overlap query. NOTE: This overlap may not agree with the results of a sweep query with regards to the initially overlapping state
        // Note: Overlap results will never have the block hit set!
        // Note: the capsule half-height is along the X-axis
        template<int N>
        bool CapsuleOverlap( float cylinderPortionHalfHeight, float radius, Quaternion const& orientation, Vector const& position, QueryFilter& filter, OverlapResultBuffer<N>& outResults )
        {
            KRG_ASSERT( m_readLockAcquired );

            outResults.m_position = position;
            outResults.m_orientation = orientation;

            // Set the no block value for overlaps
            TScopedGuardValue guard( filter.m_filterData.flags, filter.m_filterData.flags | physx::PxQueryFlag::eNO_BLOCK );
            physx::PxCapsuleGeometry const capsuleGeo( radius, cylinderPortionHalfHeight );
            bool result = m_pScene->overlap( capsuleGeo, physx::PxTransform( ToPx( position ), ToPx( orientation ) ), outResults, filter.m_filterData, &filter );
            return result;
        }

        //-------------------------------------------------------------------------

        template<int N>
        bool CylinderSweep( float halfHeight, float radius, Quaternion const& orientation, Vector const& start, Vector const& end, QueryFilter& filter, SweepResultBuffer<N>& outResults )
        {
            KRG_ASSERT( m_readLockAcquired );
            outResults.m_sweepStart = start;
            outResults.m_sweepEnd = end;
            outResults.m_orientation = orientation;

            Vector unitDirection, distance;
            ( end - start ).ToDirectionAndLength3( unitDirection, distance );
            KRG_ASSERT( !unitDirection.IsNearZero3() );

            physx::PxConvexMeshGeometry const cylinderGeo( SharedMeshes::s_pUnitCylinderMesh, physx::PxMeshScale( physx::PxVec3( radius, radius, halfHeight ) ) );
            bool const result = m_pScene->sweep( cylinderGeo, physx::PxTransform( ToPx( start ), ToPx( orientation ) ), ToPx( unitDirection ), distance.m_x, outResults, filter.m_hitFlags, filter.m_filterData, &filter );
            outResults.CalculateFinalShapePosition( s_sweepSeperationDistance );
            return result;
        }

        template<int N>
        bool CylinderSweep( float halfHeight, float radius, Quaternion const& orientation, Vector const& start, Vector const& unitDirection, float distance, QueryFilter& filter, SweepResultBuffer<N>& outResults )
        {
            KRG_ASSERT( m_readLockAcquired );
            KRG_ASSERT( unitDirection.IsNormalized3() && distance > 0 );
            KRG_ASSERT( SharedMeshes::s_pUnitCylinderMesh != nullptr );

            outResults.m_sweepStart = start;
            outResults.m_sweepEnd = Vector::MultiplyAdd( unitDirection, Vector( distance ), start );
            outResults.m_orientation = orientation;

            physx::PxConvexMeshGeometry const cylinderGeo( SharedMeshes::s_pUnitCylinderMesh, physx::PxMeshScale( physx::PxVec3( radius, radius, halfHeight ) ) );
            bool const result = m_pScene->sweep( cylinderGeo, physx::PxTransform( ToPx( start ), ToPx( orientation ) ), ToPx( unitDirection ), distance, outResults, filter.m_hitFlags, filter.m_filterData, &filter );
            outResults.CalculateFinalShapePosition( s_sweepSeperationDistance );
            return result;
        }

        // Perform an overlap query. NOTE: This overlap may not agree with the results of a sweep query with regards to the initially overlapping state
        // Note: Overlap results will never have the block hit set!
        template<int N>
        bool CylinderOverlap( float halfHeight, float radius, Quaternion const& orientation, Vector const& position, QueryFilter& filter, OverlapResultBuffer<N>& outResults )
        {
            KRG_ASSERT( m_readLockAcquired );

            outResults.m_position = position;
            outResults.m_orientation = orientation;

            // Set the no block value for overlaps
            TScopedGuardValue guard( filter.m_filterData.flags, filter.m_filterData.flags | physx::PxQueryFlag::eNO_BLOCK );
            physx::PxConvexMeshGeometry const cylinderGeo( SharedMeshes::s_pUnitCylinderMesh, physx::PxMeshScale( physx::PxVec3( radius, radius, halfHeight ) ) );
            bool const result = m_pScene->overlap( cylinderGeo, physx::PxTransform( ToPx( position ), ToPx( orientation ) ), outResults, filter.m_filterData, &filter );
            return result;
        }

        //-------------------------------------------------------------------------
        
        template<int N>
        bool BoxSweep( Vector halfExtents, Quaternion const& orientation, Vector const& start, Vector const& unitDirection, float distance, QueryFilter& filter, SweepResultBuffer<N>& outResults )
        {
            KRG_ASSERT( m_readLockAcquired );
            KRG_ASSERT( unitDirection.IsNormalized3() && distance > 0 );

            outResults.m_sweepStart = start;
            outResults.m_sweepEnd = Vector::MultiplyAdd( unitDirection, Vector( distance ), start );
            outResults.m_orientation = orientation;

            physx::PxBoxGeometry const boxGeo( ToPx( halfExtents ) );
            bool const result = m_pScene->sweep( boxGeo, physx::PxTransform( ToPx( start ), ToPx( orientation ) ), ToPx( unitDirection ), distance, outResults, filter.m_hitFlags, filter.m_filterData, &filter );
            outResults.CalculateFinalShapePosition( s_sweepSeperationDistance );
            return result;
        }

        template<int N>
        bool BoxSweep( Vector halfExtents, Quaternion const& orientation, Vector const& start, Vector const& end, QueryFilter& filter, SweepResultBuffer<N>& outResults )
        {
            KRG_ASSERT( m_readLockAcquired );

            Vector unitDirection, distance;
            ( end - start ).ToDirectionAndLength3( unitDirection, distance );
            KRG_ASSERT( !unitDirection.IsNearZero3() );

            outResults.m_sweepStart = start;
            outResults.m_sweepEnd = end;
            outResults.m_orientation = orientation;

            physx::PxBoxGeometry const boxGeo( ToPx( halfExtents ) );
            bool const result = m_pScene->sweep( boxGeo, physx::PxTransform( ToPx( start ), ToPx( orientation ) ), ToPx( unitDirection ), distance.m_x, outResults, filter.m_hitFlags, filter.m_filterData, &filter );
            outResults.CalculateFinalShapePosition( s_sweepSeperationDistance );
            return result;
        }

        // Perform an overlap query. NOTE: This overlap may not agree with the results of a sweep query with regards to the initially overlapping state
        // Note: Overlap results will never have the block hit set!
        template<int N>
        bool BoxOverlap( Vector halfExtents, Quaternion const& orientation, Vector const& position, QueryFilter& filter, OverlapResultBuffer<N>& outResults )
        {
            KRG_ASSERT( m_readLockAcquired );

            outResults.m_position = position;
            outResults.m_orientation = orientation;

            // Set the no block value for overlaps
            TScopedGuardValue guard( filter.m_filterData.flags, filter.m_filterData.flags | physx::PxQueryFlag::eNO_BLOCK );
            physx::PxBoxGeometry const boxGeo( ToPx( halfExtents ) );
            bool const result = m_pScene->overlap( boxGeo, physx::PxTransform( ToPx( position ), ToPx( orientation ) ), outResults, filter.m_filterData, &filter );
            return result;
        }

        //-------------------------------------------------------------------------

        template<int N>
        bool ShapeSweep( physx::PxShape* pShape, Quaternion const& orientation, Vector const& start, Vector const& unitDirection, float distance, QueryFilter& filter, SweepResultBuffer<N>& outResults )
        {
            KRG_ASSERT( m_readLockAcquired );
            KRG_ASSERT( unitDirection.IsNormalized3() && distance > 0 );

            outResults.m_sweepStart = start;
            outResults.m_sweepEnd = Vector::MultiplyAdd( unitDirection, Vector( distance ), start );
            outResults.m_orientation = orientation;

            bool const result = m_pScene->sweep( pShape->getGeometry().any(), ToPx( Transform( orientation, start ) ), ToPx( unitDirection ), distance, outResults, filter.m_hitFlags, filter.m_filterData, &filter );
            outResults.CalculateFinalShapePosition( s_sweepSeperationDistance );
            return result;
        }

        template<int N>
        bool ShapeSweep( physx::PxShape* pShape, Quaternion const& orientation, Vector const& start, Vector const& end, QueryFilter& filter, SweepResultBuffer<N>& outResults )
        {
            KRG_ASSERT( m_readLockAcquired );
            Vector unitDirection, distance;
            ( end - start ).ToDirectionAndLength3( unitDirection, distance );
            KRG_ASSERT( !unitDirection.IsNearZero3() );

            outResults.m_sweepStart = start;
            outResults.m_sweepEnd = end;
            outResults.m_orientation = orientation;

            bool const result = m_pScene->sweep( pShape->getGeometry().any(), ToPx( Transform( orientation, start ) ), ToPx( unitDirection ), distance.m_x, outResults, filter.m_hitFlags, filter.m_filterData, &filter );
            outResults.CalculateFinalShapePosition( s_sweepSeperationDistance );
            return result;
        }

        // Perform an overlap query. NOTE: This overlap may not agree with the results of a sweep query with regards to the initially overlapping state
        // Note: Overlap results will never have the block hit set!
        template<int N>
        bool ShapeOverlap( physx::PxShape* pShape, Quaternion const& orientation, Vector const& start, QueryFilter& filter, OverlapResultBuffer<N>& outResults )
        {
            KRG_ASSERT( m_readLockAcquired );

            outResults.m_position = start;
            outResults.m_orientation = orientation;

            // Set the no block value for overlaps
            TScopedGuardValue guard( filter.m_filterData.flags, filter.m_filterData.flags | physx::PxQueryFlag::eNO_BLOCK );
            bool const result = m_pScene->overlap( pShape->getGeometry().any(), ToPx( Transform( orientation, start ) ), outResults, filter.m_filterData, &filter );
            return result;
        }

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

        physx::PxRigidActor* CreateActor( PhysicsShapeComponent* pComponent ) const;
        physx::PxShape* CreateShape( PhysicsShapeComponent* pComponent, physx::PxRigidActor* pActor ) const;

        bool CreateCharacterActorAndShape( CharacterComponent* pComponent ) const;

    private:

        PhysicsSystem*                                          m_pPhysicsSystem = nullptr;
        physx::PxScene*                                         m_pScene = nullptr;

        TIDVector<ComponentID, CharacterComponent*>             m_characterComponents;
        TIDVector<ComponentID, PhysicsShapeComponent*>          m_physicsShapeComponents;
        TIDVector<ComponentID, PhysicsShapeComponent*>          m_dynamicShapeComponents; // TODO: profile and see if we need to use a dynamic pool

        #if KRG_DEVELOPMENT_TOOLS
        std::atomic<bool>                                       m_readLockAcquired = false;     // Assertion helper
        std::atomic<bool>                                       m_writeLockAcquired = false;    // Assertion helper

        bool                                                    m_drawDynamicActorBounds = false;
        bool                                                    m_drawKinematicActorBounds = false;

        uint32                                                  m_sceneDebugFlags = 0;
        float                                                   m_debugDrawDistance = 10.0f;
        #endif
    };
}