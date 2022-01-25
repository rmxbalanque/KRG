#pragma once

#include "Animation_RuntimeGraph_Events.h"
#include "Animation_RuntimeGraph_Common.h"
#include "Animation_RuntimeGraph_RootMotionRecorder.h"
#include "Engine/Animation/AnimationBoneMask.h"
#include "Engine/Animation/TaskSystem/Animation_TaskSystem.h"
#include "System/Core/Math/Transform.h"
#include "System/Core/Time/Time.h"

//-------------------------------------------------------------------------

namespace KRG::Physics { class Scene; }

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class RootMotionRecorder;

    //-------------------------------------------------------------------------
    // Layer Context
    //-------------------------------------------------------------------------

    struct GraphLayerContext
    {
        KRG_FORCE_INLINE bool IsSet() const { return m_isCurrentlyInLayer; }

        KRG_FORCE_INLINE void BeginLayer()
        {
            m_isCurrentlyInLayer = true;
            m_layerWeight = 1.0f;
            m_pLayerMask = nullptr;
        }

        KRG_FORCE_INLINE void EndLayer()
        {
            m_isCurrentlyInLayer = false;
            m_layerWeight = 0.0f;
            m_pLayerMask = nullptr;
        }

    public:

        BoneMask*                               m_pLayerMask = nullptr;
        float                                   m_layerWeight = 0.0f;
        bool                                    m_isCurrentlyInLayer = false;
    };

    //-------------------------------------------------------------------------
    // Graph Context
    //-------------------------------------------------------------------------

    class GraphContext final
    {

    public:

        GraphContext();

        void Initialize( uint64 userID, TaskSystem* pTaskSystem, Pose const* pPreviousPose, RootMotionRecorder* pRootMotionRecorder );
        void Shutdown();

        inline bool IsValid() const { return m_pSkeleton != nullptr && m_pTaskSystem != nullptr && m_pPreviousPose != nullptr; }
        void Update( Seconds const deltaTime, Transform const& currentWorldTransform, Physics::Scene* pPhysicsScene );

        // Debugging
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        inline void TrackActiveNode( GraphNodeIndex nodeIdx ) { KRG_ASSERT( nodeIdx != InvalidIndex ); m_activeNodes.emplace_back( nodeIdx ); }
        inline TVector<GraphNodeIndex> const& GetActiveNodes() const { return m_activeNodes; }
        inline RootMotionRecorder* GetRootMotionActionRecorder() { return m_pRootMotionActionRecorder; }
        inline RootMotionRecorder const* GetRootMotionActionRecorder() const { return m_pRootMotionActionRecorder; }
        #endif

    private:

        GraphContext( GraphContext const& ) = delete;
        GraphContext& operator=( GraphContext& ) = delete;

    public:

        uint64                                  m_graphUserID = 0;
        TaskSystem* const                       m_pTaskSystem = nullptr;
        Skeleton const* const                   m_pSkeleton = nullptr;
        Pose const* const                       m_pPreviousPose = nullptr;
        Seconds                                 m_deltaTime = 0.0f;
        Transform                               m_worldTransform = Transform::Identity;
        Transform                               m_worldTransformInverse = Transform::Identity;
        SampledEventsBuffer                     m_sampledEvents;
        uint32                                  m_updateID = 0;
        BranchState                             m_branchState = BranchState::Active;
        Physics::Scene*                         m_pPhysicsScene = nullptr;

        BoneMaskPool                            m_boneMaskPool;
        GraphLayerContext                       m_layerContext;

    private:

        #if KRG_DEVELOPMENT_TOOLS
        RootMotionRecorder* const               m_pRootMotionActionRecorder = nullptr; // Allows nodes to record root motion operations
        TVector<GraphNodeIndex>                 m_activeNodes;
        #endif
    };

}