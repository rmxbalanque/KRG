#include "AnimGraphNode_Pose.h"
#include "Engine/Animation/Graph/TaskSystem/Tasks/AnimGraphTask_DefaultPose.h"
#include "Engine/Animation/Graph/TaskSystem/Tasks/AnimGraphTask_Sample.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void DefaultPoseNode::InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime )
    {
        m_previousTime = m_currentTime;
        m_duration = 1 / 30.0f;
    }

    AnimationNode::UpdateResult DefaultPoseNode::Update( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );
        MarkNodeActive( context );
        auto pSettings = GetSettings<DefaultPoseNode>();

        AnimationNode::UpdateResult Result;
        Result.m_sampledEventRange = SampledEventRange( context.m_sampledEvents.GetNumEvents() );
        Result.m_taskIdx = context.m_pTaskSystem->RegisterTask<Tasks::DefaultPoseTask>( GetNodeIdx(), pSettings->m_poseType );
        return Result;
    }

    //-------------------------------------------------------------------------

    bool PoseNode::IsValid() const
    {
        return AnimationNode::IsValid() && m_pAnimation != nullptr;
    }

    void PoseNode::OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, GraphDataSet const& dataSet )
    {
        AnimationNode::OnConstruct( pSettings, nodePtrs, dataSet );
        SetNodePtrFromIndex( nodePtrs, GetSettings<PoseNode>()->m_poseTimeValueNodeIdx, m_pPoseTimeValue );
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

    AnimationNode::UpdateResult PoseNode::Update( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );
        auto Settings = GetSettings<PoseNode>();

        AnimationNode::UpdateResult Result;
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
            timeValue = m_pPoseTimeValue->GetValue<float>( context );

            // Remap the time value using the provided range
            auto pSettings = GetSettings<PoseNode>();
            if ( pSettings->m_timeRangeStart > pSettings->m_timeRangeEnd )
            {
                timeValue = TRange<float>( pSettings->m_timeRangeEnd, pSettings->m_timeRangeStart ).GetPercentageThroughClamped( timeValue );
                timeValue = 1.0f - timeValue;
            }
            else
            {
                timeValue = TRange<float>( pSettings->m_timeRangeStart, pSettings->m_timeRangeEnd ).GetPercentageThroughClamped( timeValue );
            }
        }

        KRG_ASSERT( timeValue >= 0.0f && timeValue <= 1.0f );
        return Percentage( timeValue );
    }
}