#pragma once
#include "Engine/Animation/AnimationPose.h"
#include "Engine/Animation/Graph/AnimationGraphNode.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class KRG_ENGINE_ANIMATION_API DefaultPoseNode final : public AnimationNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public AnimationNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( AnimationNode::Settings, m_poseType );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const override;

            Pose::InitialState  m_poseType = Pose::InitialState::ReferencePose;
        };

    private:

        virtual SyncTrack const& GetSyncTrack() const override { return SyncTrack::s_defaultTrack; }
        virtual void InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime ) override;
        virtual UpdateResult Update( GraphContext& context ) override;
        virtual UpdateResult Update( GraphContext& context, SyncTrackTimeRange const& updateRange ) override { return Update( context ); }
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API PoseNode final : public AnimationNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public AnimationNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( AnimationNode::Settings, m_poseTimeValueNodeIdx, m_animationID, m_remapRange );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const override;

            NodeIndex                       m_poseTimeValueNodeIdx = InvalidIndex;
            uint16                          m_animationID;
            FloatRange                      m_remapRange = FloatRange( 0, 1 ); // Time range allows for remapping a time value that is not a normalized time to the animation
        };

    public:

        virtual bool IsValid() const override;

    private:

        virtual SyncTrack const& GetSyncTrack() const override { return SyncTrack::s_defaultTrack; }
        virtual void InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;

        virtual UpdateResult Update( GraphContext& context ) override;
        virtual UpdateResult Update( GraphContext& context, SyncTrackTimeRange const& updateRange ) override { return Update( context ); }

        float GetTimeValue( GraphContext& context );

    private:

        ValueNodeFloat*                         m_pPoseTimeValue = nullptr;
        AnimationClip const*                    m_pAnimation = nullptr;
    };
}