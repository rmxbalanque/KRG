#pragma once
#include "Engine/Animation/AnimationPose.h"
#include "Engine/Animation/Graph/AnimGraphNode.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class DefaultPoseNode final : public AnimationNode
    {

    public:

        struct Settings : public AnimationNode::Settings
        {
            Pose::InitialState  m_poseType = Pose::InitialState::ReferencePose;
        };

    private:

        virtual SyncTrack const& GetSyncTrack() const override { return SyncTrack::s_defaultTrack; }
        virtual void InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime ) override;
        virtual AnimationNode::UpdateResult Update( GraphContext& context ) override;
        virtual AnimationNode::UpdateResult Update( GraphContext& context, SyncTrackTimeRange const& updateRange ) override { return Update( context ); }
    };

    //-------------------------------------------------------------------------

    class PoseNode final : public AnimationNode
    {

    public:

        struct Settings : public AnimationNode::Settings
        {
            NodeIndex                       m_poseTimeValueNodeIdx = InvalidIndex;
            UUID                            m_animationID;

            // Time range allows for remapping a time value that is not a normalized time to the animation
            float                           m_timeRangeStart = 0;
            float                           m_timeRangeEnd = 1;
        };

    public:

        virtual bool IsValid() const;

    private:

        virtual SyncTrack const& GetSyncTrack() const override { return SyncTrack::s_defaultTrack; }
        virtual void OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, GraphDataSet const& dataSet ) override;
        virtual void InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;

        virtual AnimationNode::UpdateResult Update( GraphContext& context ) override;
        virtual AnimationNode::UpdateResult Update( GraphContext& context, SyncTrackTimeRange const& updateRange ) override { return Update( context ); }

        float GetTimeValue( GraphContext& context );

    private:

        ValueNodeFloat*                         m_pPoseTimeValue = nullptr;
        AnimationClip const*                    m_pAnimation = nullptr;
    };
}