#pragma once
#include "System/Animation/AnimationPose.h"
#include "Engine/Animation/Graph/Animation_RuntimeGraph_Node.h"
#include "Engine/Animation/Graph/Animation_RuntimeGraph_Resources.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::GraphNodes
{
    class KRG_ENGINE_ANIMATION_API ZeroPoseNode final : public PoseNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public PoseNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, GraphDataSet const* pDataSet, InitOptions options ) const override;
        };

    private:

        virtual SyncTrack const& GetSyncTrack() const override { return SyncTrack::s_defaultTrack; }
        virtual void InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime ) override;
        virtual GraphPoseNodeResult Update( GraphContext& context ) override;
        virtual GraphPoseNodeResult Update( GraphContext& context, SyncTrackTimeRange const& updateRange ) override { return Update( context ); }
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API ReferencePoseNode final : public PoseNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public PoseNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, GraphDataSet const* pDataSet, InitOptions options ) const override;
        };

    private:

        virtual SyncTrack const& GetSyncTrack() const override { return SyncTrack::s_defaultTrack; }
        virtual void InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime ) override;
        virtual GraphPoseNodeResult Update( GraphContext& context ) override;
        virtual GraphPoseNodeResult Update( GraphContext& context, SyncTrackTimeRange const& updateRange ) override { return Update( context ); }
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API AnimationPoseNode final : public PoseNode
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public PoseNode::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( PoseNode::Settings, m_poseTimeValueNodeIdx, m_dataSlotIndex, m_inputTimeRemapRange );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, GraphDataSet const* pDataSet, InitOptions options ) const override;

            GraphNodeIndex                           m_poseTimeValueNodeIdx = InvalidIndex;
            DataSetSlotIndex                    m_dataSlotIndex = InvalidIndex;
            FloatRange                          m_inputTimeRemapRange = FloatRange( 0, 1 ); // Time range allows for remapping a time value that is not a normalized time to the animation
        };

    public:

        virtual bool IsValid() const override;

    private:

        virtual SyncTrack const& GetSyncTrack() const override { return SyncTrack::s_defaultTrack; }
        virtual void InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;

        virtual GraphPoseNodeResult Update( GraphContext& context ) override;
        virtual GraphPoseNodeResult Update( GraphContext& context, SyncTrackTimeRange const& updateRange ) override { return Update( context ); }

        float GetTimeValue( GraphContext& context );

    private:

        FloatValueNode*                         m_pPoseTimeValue = nullptr;
        AnimationClip const*                    m_pAnimation = nullptr;
    };
}