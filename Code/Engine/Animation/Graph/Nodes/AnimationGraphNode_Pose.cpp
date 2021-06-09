#include "AnimationGraphNode_Pose.h"
#include "Engine/Animation/Graph/TaskSystem/Tasks/AnimationGraphTask_DefaultPose.h"
#include "Engine/Animation/Graph/TaskSystem/Tasks/AnimationGraphTask_Sample.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void ZeroPoseNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<ZeroPoseNode>( nodePtrs, options );
    }

    void ZeroPoseNode::InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime )
    {
        m_previousTime = m_currentTime;
        m_duration = 1 / 30.0f;
    }

    PoseNodeResult ZeroPoseNode::Update( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );
        MarkNodeActive( context );
        auto pSettings = GetSettings<ZeroPoseNode>();

        PoseNodeResult Result;
        Result.m_sampledEventRange = SampledEventRange( context.m_sampledEvents.GetNumEvents() );
        Result.m_taskIdx = context.m_pTaskSystem->RegisterTask<Tasks::DefaultPoseTask>( GetNodeIdx(), Pose::InitialState::ZeroPose );
        return Result;
    }

    //-------------------------------------------------------------------------

    void ReferencePoseNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<ReferencePoseNode>( nodePtrs, options );
    }

    void ReferencePoseNode::InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime )
    {
        m_previousTime = m_currentTime;
        m_duration = 1 / 30.0f;
    }

    PoseNodeResult ReferencePoseNode::Update( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );
        MarkNodeActive( context );
        auto pSettings = GetSettings<ReferencePoseNode>();

        PoseNodeResult Result;
        Result.m_sampledEventRange = SampledEventRange( context.m_sampledEvents.GetNumEvents() );
        Result.m_taskIdx = context.m_pTaskSystem->RegisterTask<Tasks::DefaultPoseTask>( GetNodeIdx(), Pose::InitialState::ReferencePose );
        return Result;
    }

    //-------------------------------------------------------------------------

    void AnimationPoseNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<AnimationPoseNode>( nodePtrs, options );
        SetNodePtrFromIndex( nodePtrs, m_poseTimeValueNodeIdx, pNode->m_pPoseTimeValue );

        auto pSettings = pNode->GetSettings<AnimationPoseNode>();
        pNode->m_pAnimation = pDataSet->GetAnimationClip( pSettings->m_sourceIndex );
    }

    bool AnimationPoseNode::IsValid() const
    {
        return PoseNode::IsValid() && m_pAnimation != nullptr;
    }

    void AnimationPoseNode::InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime )
    {
        KRG_ASSERT( context.IsValid() && m_pPoseTimeValue != nullptr );
        auto pSettings = GetSettings<AnimationPoseNode>();

        PoseNode::InitializeInternal( context, initialTime );
        m_pPoseTimeValue->Initialize( context );
    }

    void AnimationPoseNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pPoseTimeValue != nullptr );
        m_pPoseTimeValue->Shutdown( context );
        PoseNode::ShutdownInternal( context );
    }

    PoseNodeResult AnimationPoseNode::Update( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );
        auto Settings = GetSettings<PoseNode>();

        PoseNodeResult Result;
        if ( !IsValid() )
        {
            return Result;
        }

        KRG_ASSERT( m_pAnimation != nullptr );

        MarkNodeActive( context );
        m_currentTime = m_previousTime = GetTimeValue( context );

        //-------------------------------------------------------------------------

        Result.m_sampledEventRange = SampledEventRange( context.m_sampledEvents.GetNumEvents() );
        Result.m_taskIdx = context.m_pTaskSystem->RegisterTask<Tasks::SampleTask>( GetNodeIdx(), m_pAnimation, m_currentTime );
        return Result;
    }

    float AnimationPoseNode::GetTimeValue( GraphContext& context )
    {
        KRG_ASSERT( IsValid() );

        float timeValue = 0.0f;
        if ( m_pPoseTimeValue != nullptr )
        {
            auto pSettings = GetSettings<AnimationPoseNode>();
            timeValue = m_pPoseTimeValue->GetValue<float>( context );
            timeValue = pSettings->m_inputTimeRemapRange.GetPercentageThroughClamped( timeValue );
        }

        KRG_ASSERT( timeValue >= 0.0f && timeValue <= 1.0f );
        return Percentage( timeValue );
    }
}