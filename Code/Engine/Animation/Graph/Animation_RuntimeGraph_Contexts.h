#pragma once

#include "Animation_RuntimeGraph_Events.h"
#include "Animation_RuntimeGraph_Common.h"
#include "Engine/Animation/AnimationBoneMask.h"
#include "Engine/Animation/TaskSystem/Animation_TaskSystem.h"
#include "System/Core/Math/Transform.h"
#include "System/Core/Time/Time.h"

//-------------------------------------------------------------------------

namespace KRG::Physics { class Scene; }

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    //-------------------------------------------------------------------------
    // Debug
    //-------------------------------------------------------------------------

    #if KRG_DEVELOPMENT_TOOLS
    class RootMotionActionRecorder
    {

    public:

        enum class ActionType : uint16
        {
            Unknown = 0,
            Sample,
            Modification,
            Blend,
        };

        struct RecordedAction
        {
            RecordedAction( GraphNodeIndex nodeIdx, ActionType actionType, Transform const& rootMotionDelta )
                : m_rootMotionDelta( rootMotionDelta )
                , m_nodeIdx( nodeIdx )
                , m_actionType( actionType )
            {}

            Transform                   m_rootMotionDelta;
            GraphNodeIndex              m_nodeIdx;
            ActionType                  m_actionType;
            TInlineVector<int16, 2>     m_dependencies;
        };

    public:

        inline TVector<RecordedAction> const& GetRecordedActions() const { return m_recordedActions; }
        inline void Reset() { m_recordedActions.clear(); }

        //-------------------------------------------------------------------------

        inline bool HasRecordedActions() const { return !m_recordedActions.empty(); }

        KRG_FORCE_INLINE int16 GetLastActionIndex() const { return (int16) m_recordedActions.size() - 1; }

        KRG_FORCE_INLINE int16 RecordSampling( GraphNodeIndex nodeIdx, Transform const& rootMotionDelta )
        {
            KRG_ASSERT( nodeIdx != InvalidIndex );
            int16 const idx = (int16) m_recordedActions.size();
            m_recordedActions.emplace_back( nodeIdx, ActionType::Sample, rootMotionDelta);
            return idx;
        }

        KRG_FORCE_INLINE int16 RecordModification( GraphNodeIndex nodeIdx, Transform const& rootMotionDelta )
        {
            KRG_ASSERT( nodeIdx != InvalidIndex );
            int16 const previousIdx = GetLastActionIndex();
            KRG_ASSERT( previousIdx >= 0 );
            int16 const idx = (int16) m_recordedActions.size();
            auto& action = m_recordedActions.emplace_back( nodeIdx, ActionType::Modification, rootMotionDelta );
            action.m_dependencies.emplace_back( previousIdx );
            return idx;
        }

        // Blend operations automatically pop a blend context
        KRG_FORCE_INLINE int16 RecordBlend( GraphNodeIndex nodeIdx, int16 originalRootMotionActionIdx0, int16 originalRootMotionActionIdx1, Transform const& rootMotionDelta )
        {
            KRG_ASSERT( nodeIdx != InvalidIndex );
            int16 const idx = (int16) m_recordedActions.size();
            auto& action = m_recordedActions.emplace_back( nodeIdx, ActionType::Blend, rootMotionDelta );
            action.m_dependencies.emplace_back( originalRootMotionActionIdx0 );

            // It's possible for the use to only have a single source for a blend since not all nodes register root motion actions
            if ( originalRootMotionActionIdx0 == originalRootMotionActionIdx1 )
            {
                action.m_dependencies.emplace_back( InvalidIndex );
            }
            else
            {
                KRG_ASSERT( originalRootMotionActionIdx1 > originalRootMotionActionIdx0 );
                action.m_dependencies.emplace_back( originalRootMotionActionIdx1 );
            }

            return idx;
        }

    private:

        TVector<RecordedAction>         m_recordedActions;
    };
    #endif

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

    class GraphContext
    {

    public:

        GraphContext();

        void Initialize( uint64 userID, TaskSystem* pTaskSystem, Pose const* pPreviousPose );
        void Shutdown();

        inline bool IsValid() const { return m_pSkeleton != nullptr && m_pTaskSystem != nullptr && m_pPreviousPose != nullptr; }
        void Update( Seconds const deltaTime, Transform const& currentWorldTransform, Physics::Scene* pPhysicsScene );

        // Debugging
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        inline void TrackActiveNode( GraphNodeIndex nodeIdx ) { KRG_ASSERT( nodeIdx != InvalidIndex ); m_activeNodes.emplace_back( nodeIdx ); }
        inline TVector<GraphNodeIndex> const& GetActiveNodes() const { return m_activeNodes; }
        inline RootMotionActionRecorder* GetRootMotionActionRecorder() { return &m_rootMotionActionRecorder; }
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
        RootMotionActionRecorder                m_rootMotionActionRecorder; // Allows nodes to record root motion operations
        TVector<GraphNodeIndex>                 m_activeNodes;
        #endif
    };

}