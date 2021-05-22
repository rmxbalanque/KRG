#include "AnimationGraphNode_Pose.h"
#include "Engine/Animation/Graph/TaskSystem/Tasks/AnimationGraphTask_DefaultPose.h"
#include "Engine/Animation/Graph/TaskSystem/Tasks/AnimationGraphTask_Sample.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void DefaultPoseNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const
    {
        auto pNode = CreateNode<DefaultPoseNode>( nodePtrs, options );
    }

    void DefaultPoseNode::InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime )
    {
        m_previousTime = m_currentTime;
        m_duration = 1 / 30.0f;
    }

    UpdateResult DefaultPoseNode::Update( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );
        MarkNodeActive( context );
        auto pSettings = GetSettings<DefaultPoseNode>();

        UpdateResult Result;
        Result.m_sampledEventRange = SampledEventRange( context.m_sampledEvents.GetNumEvents() );
        Result.m_taskIdx = context.m_pTaskSystem->RegisterTask<Tasks::DefaultPoseTask>( GetNodeIdx(), pSettings->m_poseType );
        return Result;
    }

    //-------------------------------------------------------------------------

    void PoseNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const
    {
        auto pNode = CreateNode<PoseNode>( nodePtrs, options );
        SetNodePtrFromIndex( nodePtrs, m_poseTimeValueNodeIdx, pNode->m_pPoseTimeValue );
    }

    bool PoseNode::IsValid() const
    {
        return AnimationNode::IsValid() && m_pAnimation != nullptr;
    }

    void PoseNode::InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime )
    {
        KRG_ASSERT( context.IsValid() && m_pPoseTimeValue != nullptr );

        AnimationNode::InitializeInternal( context, initialTime );
        m_pPoseTimeValue->Initialize( context );

        // Try to get the animation record for this source node
        auto pSettings = GetSettings<PoseNode>();
        m_pAnimation = context.m_pDataSet->GetAnimationClip( pSettings->m_animationID );
    }

    void PoseNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() && m_pPoseTimeValue != nullptr );

        m_pAnimation = nullptr;
        m_pPoseTimeValue->Shutdown( context );
        AnimationNode::ShutdownInternal( context );
    }

    UpdateResult PoseNode::Update( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );
        auto Settings = GetSettings<PoseNode>();

        UpdateResult Result;
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

    float PoseNode::GetTimeValue( GraphContext& context )
    {
        KRG_ASSERT( IsValid() );

        float timeValue = 0.0f;
        if ( m_pPoseTimeValue != nullptr )
        {
            auto pSettings = GetSettings<PoseNode>();
            timeValue = m_pPoseTimeValue->GetValue<float>( context );
            timeValue = pSettings->m_remapRange.GetPercentageThroughClamped( timeValue );
        }

        KRG_ASSERT( timeValue >= 0.0f && timeValue <= 1.0f );
        return Percentage( timeValue );
    }
}