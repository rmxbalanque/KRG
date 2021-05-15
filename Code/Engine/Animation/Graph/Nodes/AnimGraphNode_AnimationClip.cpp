#include "AnimGraphNode_AnimationClip.h"
#include "Engine/Animation/Graph/TaskSystem/Tasks/AnimGraphTask_DefaultPose.h"
#include "Engine/Animation/Graph/TaskSystem/Tasks/AnimGraphTask_Sample.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void AnimationClipNode::OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, GraphDataSet const& dataSet )
    {
        AnimationClipReferenceNode::OnConstruct( pSettings, nodePtrs, dataSet );
        SetNodePtrFromIndex( nodePtrs, GetSettings<AnimationClipNode>()->m_playInReverseValueNodeIdx, m_pPlayInReverseValueNode );
    }

    bool AnimationClipNode::IsValid() const
    {
        return AnimationClipReferenceNode::IsValid() && m_pAnimation != nullptr;
    }

    void AnimationClipNode::InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime )
    {
        AnimationClipReferenceNode::InitializeInternal( context, initialTime );
        auto pSettings = GetSettings<AnimationClipNode>();

        // Try to get the animation record for this source node
        m_pAnimation = context.m_pDataSet->GetAnimationClip( pSettings->m_animationID );

        // Try to create play in reverse node
        if ( m_pPlayInReverseValueNode != nullptr )
        {
            m_pPlayInReverseValueNode->Initialize( context );
        }

        // Initialize state data
        m_duration = m_pAnimation->GetDuration();
        m_shouldSampleRootMotion = pSettings->m_shouldSampleRootMotion;
        m_shouldPlayInReverse = false;

        // Calculate start time
        m_currentTime = m_previousTime = m_pAnimation->GetSyncTrack().GetPercentageThrough( initialTime );
        KRG_ASSERT( m_currentTime >= 0.0f && m_currentTime <= 1.0f );
    }

    void AnimationClipNode::ShutdownInternal( GraphContext& context )
    {
        if ( m_pAnimation != nullptr )
        {
            if ( m_pPlayInReverseValueNode != nullptr )
            {
                m_pPlayInReverseValueNode->Shutdown( context );
            }

            m_pAnimation = nullptr;
        }

        m_currentTime = m_previousTime = 0.0f;
        AnimationClipReferenceNode::ShutdownInternal( context );
    }

    AnimationNode::UpdateResult AnimationClipNode::Update( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );

        if ( !IsValid() )
        {
            return AnimationNode::UpdateResult();
        }

        MarkNodeActive( context );
        KRG_ASSERT( m_currentTime >= 0.0f && m_currentTime <= 1.0f );

        // Should we change the playback direction?
        if ( m_pPlayInReverseValueNode != nullptr )
        {
            if ( m_shouldPlayInReverse != m_pPlayInReverseValueNode->GetValue<bool>( context ) )
            {
                m_shouldPlayInReverse = !m_shouldPlayInReverse;
                m_currentTime = ( 1.0f - m_currentTime );
                if ( m_currentTime == 1.0f )
                {
                    m_currentTime = 0.0f;
                }
            }
        }

        //-------------------------------------------------------------------------

        Percentage const deltaPercentage = Percentage( context.m_deltaTime / m_duration );

        auto pSettings = GetSettings<AnimationClipNode>();
        if ( !pSettings->m_allowLooping )
        {
            // We might have come from a sync update so ensure the previous time is set to the normalized current time
            m_previousTime = m_currentTime;
            m_currentTime = ( m_previousTime + deltaPercentage ).GetClamped( false );
        }
        else // Regular update
        {
            m_previousTime = m_currentTime;
            m_currentTime += deltaPercentage;
            if ( m_currentTime > 1 )
            {
                m_currentTime = m_currentTime.GetNormalizedTime();
                m_loopCount++;
            }
        }

        //-------------------------------------------------------------------------

        return CalculateResult( context, true );
    }

    AnimationNode::UpdateResult AnimationClipNode::Update( GraphContext& context, SyncTrackTimeRange const& updateRange )
    {
        KRG_ASSERT( context.IsValid() );

        if ( !IsValid() )
        {
            return AnimationNode::UpdateResult();
        }

        MarkNodeActive( context );
        KRG_ASSERT( m_currentTime >= 0.0f && m_currentTime <= 1.0f );

        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        if ( m_pPlayInReverseValueNode != nullptr || m_shouldPlayInReverse )
        {
            KRG_LOG_WARNING( "Animation", "'Play reversed' has no effect when used with time synchronization!" );
        }
        #endif

        // Handle single frame animations
        if ( m_duration == 0.0f )
        {
            m_previousTime = 0.0f;
            m_currentTime = 0.0f;
        }
        else // Regular time update
        {
            m_previousTime = GetSyncTrack().GetPercentageThrough( updateRange.m_startTime );
            m_currentTime = GetSyncTrack().GetPercentageThrough( updateRange.m_endTime );
            if ( m_currentTime < m_previousTime )
            {
                m_loopCount++;
            }
        }

        //-------------------------------------------------------------------------

        return CalculateResult( context, true );
    }

    AnimationNode::UpdateResult AnimationClipNode::CalculateResult( GraphContext& context, bool isSynchronizedUpdate ) const
    {
        KRG_ASSERT( m_pAnimation != nullptr );
        auto pSettings = GetSettings<AnimationClipNode>();

        AnimationNode::UpdateResult result;

        //-------------------------------------------------------------------------

        if ( m_shouldPlayInReverse )
        {
            KRG_UNIMPLEMENTED_FUNCTION()
        }

        // Sample root motion and events
        //-------------------------------------------------------------------------

        TInlineVector<Event const*, 10> sampledAnimationEvents;

        bool hasLooped = m_previousTime > m_currentTime;
        if ( hasLooped )
        {
            KRG_ASSERT( pSettings->m_allowLooping || isSynchronizedUpdate );

            TRange<Percentage> const preLoopUpdateRange( m_previousTime, 1.0f );
            TRange<Percentage> const postLoopUpdateRange( 0, m_currentTime );

            result.m_rootMotionDelta = m_pAnimation->GetRootTransformDelta( preLoopUpdateRange );
            result.m_rootMotionDelta = m_pAnimation->GetRootTransformDelta( postLoopUpdateRange ) * result.m_rootMotionDelta;

            m_pAnimation->GetEventsForRange( preLoopUpdateRange, sampledAnimationEvents );
            m_pAnimation->GetEventsForRange( postLoopUpdateRange, sampledAnimationEvents );
        }
        else // Just sample the current range
        {
            TRange<Percentage> const updateRange( m_previousTime, m_currentTime );
            result.m_rootMotionDelta = m_pAnimation->GetRootTransformDelta( updateRange );
            m_pAnimation->GetEventsForRange ( updateRange, sampledAnimationEvents );
        }

        // Record root motion operation
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        context.GetRootMotionActionRecorder()->RecordSampling( GetNodeIdx(), result.m_rootMotionDelta );
        #endif

        // Post-process sampled events
        //-------------------------------------------------------------------------

        Seconds const currentAnimTimeSeconds = m_currentTime * m_pAnimation->GetDuration();
        bool const isFromInactiveBranch = ( context.m_branchState == BranchState::Inactive );

        for ( auto pEvent : sampledAnimationEvents )
        {
            Percentage const percentageThroughEvent = pEvent->GetTimeRange().GetPercentageThrough( currentAnimTimeSeconds );
            auto& createdEvent = context.m_sampledEvents.EmplaceAnimEvent( pEvent, percentageThroughEvent, GetNodeIdx() );

            if ( isFromInactiveBranch )
            {
                createdEvent.GetFlags().SetFlag( SampledEvent::Flags::FromInactiveBranch );
            }
        }

        result.m_sampledEventRange = SampledEventRange( context.m_sampledEvents.GetNumEvents(), context.m_sampledEvents.GetNumEvents() + (int16) sampledAnimationEvents.size() );

        // Register pose tasks
        //-------------------------------------------------------------------------

        Percentage const sampleTime = m_shouldPlayInReverse ? Percentage( 1.0f - m_currentTime.ToFloat() ) : m_currentTime;
        result.m_taskIdx = context.m_pTaskSystem->RegisterTask<Tasks::SampleTask>( GetNodeIdx(), m_pAnimation, sampleTime );
        return result;
    }
}