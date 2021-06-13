#pragma once

#include "AnimationGraphNode_State.h"
#include "Engine/Animation/AnimationBoneMask.h"
#include "Engine/Core/Math/Easing.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class KRG_ENGINE_ANIMATION_API TransitionNode final : public PoseNode
    {
        enum class SourceType
        {
            State,
            Transition,
            CachedPose
        };

    public:

        enum class RootMotionBlend : uint8
        {
            Blend,
            IgnoreSource,
            IgnoreTarget
        };

        enum class TransitionOptions : uint8
        {
            Synchronized,
            ClampDuration,
            KeepEventIndex,
            KeepEventPercentage,
            ForcedTransitionAllowed,
        };

        struct InitializationOptions
        {
            PoseNodeResult          m_sourceNodeResult;
            bool                    m_shouldCachePose = false;
        };

        struct Settings : public PoseNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( PoseNode::Settings, m_targetStateNodeIdx, m_durationOverrideNodeIdx, m_syncEventOffsetOverrideNodeIdx, m_blendWeightEasingType, m_rootMotionBlend, m_duration, m_syncEventOffset, m_transitionOptions );

        public:

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const override;

            inline bool IsSynchronized() const { return m_transitionOptions.IsFlagSet( TransitionOptions::Synchronized ); }
            inline bool ShouldClampDuration() const { return m_transitionOptions.IsFlagSet( TransitionOptions::ClampDuration ); }
            inline bool ShouldKeepEventIndex() const { return m_transitionOptions.IsFlagSet( TransitionOptions::KeepEventIndex ); }
            inline bool ShouldKeepEventPercentage() const { return m_transitionOptions.IsFlagSet( TransitionOptions::KeepEventPercentage ); }
            inline bool IsForcedTransitionAllowed() const { return m_transitionOptions.IsFlagSet( TransitionOptions::ForcedTransitionAllowed ); }

        public:

            NodeIndex                           m_targetStateNodeIdx = 0;
            NodeIndex                           m_durationOverrideNodeIdx = InvalidIndex;
            NodeIndex                           m_syncEventOffsetOverrideNodeIdx = InvalidIndex;
            Math::Easing::Type                  m_blendWeightEasingType = Math::Easing::Type::Linear;
            RootMotionBlend                     m_rootMotionBlend = RootMotionBlend::Blend;
            Seconds                             m_duration = 0;
            float                               m_syncEventOffset = 0;
            TBitFlags<TransitionOptions>        m_transitionOptions;
        };

    public:

        virtual SyncTrack const& GetSyncTrack() const override { return IsComplete() ? m_pTargetNode->GetSyncTrack() : SyncTrack::s_defaultTrack; }
        virtual PoseNodeResult Update( GraphContext& context ) override;
        virtual PoseNodeResult Update( GraphContext& context, SyncTrackTimeRange const& updateRange ) override;
        virtual void DeactivateBranch( GraphContext& context ) override;

        // Transition Info
        PoseNodeResult StartTransitionFromState( GraphContext& context, InitializationOptions const& options, StateNode* SourceState );
        PoseNodeResult StartTransitionFromTransition( GraphContext& context, InitializationOptions const& options, TransitionNode* SourceTransition, bool bForceTransition );
        inline bool IsComplete() const { return m_transitionProgress >= 1.0f; }
        inline SourceType GetSourceType() const { return m_sourceType; }

        inline bool IsForcedTransitionAllowed() const { return GetSettings<TransitionNode>()->IsForcedTransitionAllowed(); }
        inline bool IsSourceATransition() const { return m_sourceType == SourceType::Transition; }
        inline bool IsSourceAState() const { return m_sourceType == SourceType::State; }
        inline bool IsSourceACachedPose() const { return m_sourceType == SourceType::CachedPose; }

    private:

        virtual void InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;

        PoseNodeResult InitializeTargetStateAndUpdateTransition( GraphContext& context, InitializationOptions const& options );

        void UpdateProgress( GraphContext& context, bool isInitializing = false );
        void UpdateProgressClampedSynchronized( GraphContext& context, SyncTrackTimeRange const& updateRange, bool isInitializing = false );

        void UpdateLayerContext( GraphContext& context, GraphLayerContext const& sourceLayerContext, GraphLayerContext const& targetLayerContext );

        void EndSourceTransition( GraphContext& context );

        PoseNodeResult UpdateUnsynchronized( GraphContext& context );
        PoseNodeResult UpdateSynchronized( GraphContext& context, SyncTrackTimeRange const& updateRange );

        void UpdateCachedPoseBufferIDState( GraphContext& context );
        void TransferAdditionalPoseBufferIDs( TInlineVector<UUID, 2>& outInheritedCachedPoseBufferIDs );

        void RegisterPoseTasksAndUpdateDisplacement( GraphContext& context, PoseNodeResult const& sourceResult, PoseNodeResult const& targetResult, PoseNodeResult& outResult );

        inline void CalculateBlendWeight()
        {
            auto pSettings = GetSettings<TransitionNode>();
            m_blendWeight = EvaluateEasingFunction( pSettings->m_blendWeightEasingType, m_transitionProgress );
            m_blendWeight = Math::Clamp( m_blendWeight, 0.0f, 1.0f );
        }

    private:

        PoseNode*                               m_pSourceNode = nullptr;
        StateNode*                              m_pTargetNode = nullptr;
        FloatValueNode*                         m_pDurationOverrideNode = nullptr;
        FloatValueNode*                         m_pEventOffsetOverrideNode = nullptr;
        BoneMask                                m_boneMask;
        float                                   m_transitionProgress = 0;
        float                                   m_transitionDuration = 0; // This is either time in seconds, or percentage of the sync track
        int32                                   m_syncEventOffset = 0;
        float                                   m_blendWeight = 0;
        SyncTrack                               m_syncTrack;

        UUID                                    m_cachedPoseBufferID;                   // The buffer we are currently caching to
        UUID                                    m_sourceCachedPoseBufferID;             // The buffer we are reading from (in the case of an interrupted transition)
        TInlineVector<UUID, 2>                  m_inheritedCachedPoseBufferIDs;         // All cached buffers we have inherited when canceling a transition (need to be kept alive for one frame)
        float                                   m_sourceCachedPoseBlendWeight = 0.0f;

        SourceType                              m_sourceType = SourceType::State;
    };
}