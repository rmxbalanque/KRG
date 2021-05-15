#pragma once

#include "AnimGraphBoneMaskPool.h"
#include "AnimGraphDataset.h"
#include "AnimGraphEvents.h"
#include "TaskSystem/AnimGraphTaskSystem.h"
#include "System/Core/Math/Transform.h"
#include "System/Core/Time/Time.h"
#include "AnimGraphCommon.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    //-------------------------------------------------------------------------
    // Debug
    //-------------------------------------------------------------------------

    #if KRG_DEVELOPMENT_TOOLS
    class RootMotionActionRecorder
    {

    public:

        enum class Action : uint16
        {
            Unknown = 0,
            Sample,
            Modification,
            Blend,
        };

        struct RecordedAction
        {
            RecordedAction( NodeIndex nodeIdx, Action action, Transform const& rootMotionDelta, int32 hierarchyLevel )
                : m_rootMotionDelta( rootMotionDelta )
                , m_nodeIdx( nodeIdx )
                , m_action( action )
                , m_hierarchyLevel( hierarchyLevel )
            {}

            Transform                   m_rootMotionDelta;
            NodeIndex                   m_nodeIdx;
            Action                      m_action;
            int32                       m_hierarchyLevel;
        };

    public:

        inline TVector<RecordedAction> const& GetRecordedActions() const { return m_recordedActions; }
        inline void Reset() { m_recordedActions.clear(); }

        //-------------------------------------------------------------------------

        // Create a blend context so we can track displacement operation hierarchies
        KRG_FORCE_INLINE void PushBlendHierarchyLevel() { m_hierarchyLevel++; }

        // Destroy a blend context
        KRG_FORCE_INLINE void PopBlendHierarchyLevel() { KRG_ASSERT( m_hierarchyLevel > 0 ); m_hierarchyLevel--; }

        //-------------------------------------------------------------------------

        KRG_FORCE_INLINE void RecordSampling( NodeIndex nodeIdx, Transform const& rootMotionDelta )
        {
            KRG_ASSERT( nodeIdx != InvalidIndex );
            m_recordedActions.emplace_back( RecordedAction( nodeIdx, Action::Sample, rootMotionDelta, m_hierarchyLevel ) );
        }

        KRG_FORCE_INLINE void RecordModification( NodeIndex nodeIdx, Transform const& rootMotionDelta )
        {
            KRG_ASSERT( nodeIdx != InvalidIndex );
            m_recordedActions.emplace_back( RecordedAction( nodeIdx, Action::Modification, rootMotionDelta, m_hierarchyLevel ) );
        }

        // Blend operations automatically pop a blend context
        KRG_FORCE_INLINE void RecordBlend( NodeIndex nodeIdx, Transform const& rootMotionDelta )
        {
            KRG_ASSERT( nodeIdx != InvalidIndex );
            PopBlendHierarchyLevel();
            m_recordedActions.emplace_back( RecordedAction( nodeIdx, Action::Blend, rootMotionDelta, m_hierarchyLevel ) );
        }

    private:

        TVector<RecordedAction>         m_recordedActions;
        int32                           m_hierarchyLevel = 0;
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

        void Initialize( TaskSystem* pTaskSystem, GraphDataSet const* pDataSet, Pose const* pPreviousPose );
        void Shutdown();

        inline bool IsValid() const { return m_pDataSet != nullptr && m_pTaskSystem != nullptr && m_pPreviousPose != nullptr; }
        void Update( Seconds const deltaTime, Transform const& prevDisplacementDelta, Transform const& currentWorldTransform );
        inline Skeleton const* GetSkeleton() const { return m_pDataSet->GetSkeleton(); }

        // Debugging
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        inline void TrackActiveNode( NodeIndex nodeIdx ) { KRG_ASSERT( nodeIdx != InvalidIndex ); m_activeNodes.emplace_back( nodeIdx ); }
        inline TVector<NodeIndex> const& GetActiveNodes() const { return m_activeNodes; }
        inline RootMotionActionRecorder* GetRootMotionActionRecorder() { return &m_rootMotionActionRecorder; }
        #endif

    private:

        GraphContext( GraphContext const& ) = delete;
        GraphContext& operator=( GraphContext& ) = delete;

    public:

        TaskSystem* const                       m_pTaskSystem = nullptr;
        GraphDataSet const* const               m_pDataSet = nullptr;
        Pose const* const                       m_pPreviousPose = nullptr;
        Seconds                                 m_deltaTime = 0.0f;
        Transform                               m_worldTransform = Transform::Identity;
        Transform                               m_worldTransformInverse = Transform::Identity;
        SampledEventsBuffer                     m_sampledEvents;
        uint32                                  m_updateID = 0;
        BranchState                             m_branchState = BranchState::Active;

        BoneMaskPool                            m_boneMaskPool;
        GraphLayerContext                       m_layerContext;

    private:

        #if KRG_DEVELOPMENT_TOOLS
        RootMotionActionRecorder                m_rootMotionActionRecorder; // Allows nodes to record root motion operations
        TVector<NodeIndex>                      m_activeNodes;
        #endif
    };

}