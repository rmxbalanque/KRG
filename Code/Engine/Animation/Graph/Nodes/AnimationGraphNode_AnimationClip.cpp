#include "AnimationGraphNode_AnimationClip.h"
#include "Engine/Animation/Graph/TaskSystem/Tasks/AnimationGraphTask_DefaultPose.h"
#include "Engine/Animation/Graph/TaskSystem/Tasks/AnimationGraphTask_Sample.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void AnimationClipNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<AnimationClipNode>( nodePtrs, options );
        SetOptionalNodePtrFromIndex( nodePtrs, m_playInReverseValueNodeIdx, pNode->m_pPlayInReverseValueNode );

        auto pSettings = pNode->GetSettings<AnimationClipNode>();
        pNode->m_pAnimation = pDataSet->GetAnimationClip( pSettings->m_dataSlotIdx );
    }

    bool AnimationClipNode::IsValid() const
    {
        return AnimationClipReferenceNode::IsValid() && m_pAnimation != nullptr;
    }

    void AnimationClipNode::InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime )
    {
        AnimationClipReferenceNode::InitializeInternal( context, initialTime );
        auto pSettings = GetSettings<AnimationClipNode>();

        // Try to create play in reverse node
        if ( m_pPlayInReverseValueNode != nullptr )
        {
            m_pPlayInReverseValueNode->Initialize( context );
        }

        // Initialize state data
        m_duration = m_pAnimation->GetDuration();
        m_shouldSampleRootMotion = pSettings->m_sampleRootMotion;
        m_shouldPlayInReverse = false;

        // Calculate start time
        m_currentTime = m_previousTime = m_pAnimation->GetSyncTrack().GetPercentageThrough( initialTime );
        KRG_ASSERT( m_currentTime >= 0.0f && m_currentTime <= 1.0f );
    }

    void AnimationClipNode::ShutdownInternal( GraphContext& context )
    {
        if ( m_pPlayInReverseValueNode != nullptr )
        {
            m_pPlayInReverseValueNode->Shutdown( context );
        }

        m_currentTime = m_previousTime = 0.0f;
        AnimationClipReferenceNode::ShutdownInternal( context );
    }

    PoseNodeResult AnimationClipNode::Update( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );

        if ( !IsValid() )
        {
            return PoseNodeResult();
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

    PoseNodeResult AnimationClipNode::Update( GraphContext& context, SyncTrackTimeRange const& updateRange )
    {
        KRG_ASSERT( context.IsValid() );

        if ( !IsValid() )
        {
            return PoseNodeResult();
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

    PoseNodeResult AnimationClipNode::CalculateResult( GraphContext& context, bool isSynchronizedUpdate ) const
    {
        KRG_ASSERT( m_pAnimation != nullptr );
        auto pSettings = GetSettings<AnimationClipNode>();

        PoseNodeResult result;

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

            result.m_rootMotionDelta = m_pAnimation->GetRootMotionDelta( m_previousTime, 1.0f );
            result.m_rootMotionDelta = m_pAnimation->GetRootMotionDelta( 0, m_currentTime ) * result.m_rootMotionDelta;

            m_pAnimation->GetEventsForRange( m_previousTime, 1.0f, sampledAnimationEvents );
            m_pAnimation->GetEventsForRange( 0, m_currentTime, sampledAnimationEvents );
        }
        else // Just sample the current range
        {
            result.m_rootMotionDelta = m_pAnimation->GetRootMotionDelta( m_previousTime, m_currentTime );
            m_pAnimation->GetEventsForRange ( m_previousTime, m_currentTime, sampledAnimationEvents );
        }

        // Record root motion operation
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        context.GetRootMotionActionRecorder()->RecordSampling( GetNodeIndex(), result.m_rootMotionDelta );
        #endif

        // Post-process sampled events
        //-------------------------------------------------------------------------

        Seconds const currentAnimTimeSeconds = m_currentTime * m_pAnimation->GetDuration();
        bool const isFromInactiveBranch = ( context.m_branchState == BranchState::Inactive );

        for ( auto pEvent : sampledAnimationEvents )
        {
            Percentage const percentageThroughEvent = pEvent->GetTimeRange().GetPercentageThrough( currentAnimTimeSeconds );
            auto& createdEvent = context.m_sampledEvents.EmplaceAnimEvent( GetNodeIndex(), pEvent, percentageThroughEvent );

            if ( isFromInactiveBranch )
            {
                createdEvent.GetFlags().SetFlag( SampledEvent::Flags::FromInactiveBranch );
            }
        }

        result.m_sampledEventRange = SampledEventRange( context.m_sampledEvents.GetNumEvents(), context.m_sampledEvents.GetNumEvents() + (int16) sampledAnimationEvents.size() );

        // Register pose tasks
        //-------------------------------------------------------------------------

        Percentage const sampleTime = m_shouldPlayInReverse ? Percentage( 1.0f - m_currentTime.ToFloat() ) : m_currentTime;
        result.m_taskIdx = context.m_pTaskSystem->RegisterTask<Tasks::SampleTask>( GetNodeIndex(), m_pAnimation, sampleTime );
        return result;
    }
}