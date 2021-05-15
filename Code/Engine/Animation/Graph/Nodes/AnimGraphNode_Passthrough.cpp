#include "AnimGraphNode_Passthrough.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    SyncTrack const& FPassthroughNode::GetSyncTrack() const
    {
        if ( IsValid() )
        {
            return m_pChildNode->GetSyncTrack();
        }
        else
        {
            return SyncTrack::s_defaultTrack;
        }
    }

    void FPassthroughNode::OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, GraphDataSet const& dataSet )
    {
        AnimationNode::OnConstruct( pSettings, nodePtrs, dataSet );
        KRG_ASSERT( GetSettings<FPassthroughNode>()->m_childNodeIdx != InvalidIndex );
        SetNodePtrFromIndex( nodePtrs, GetSettings<FPassthroughNode>()->m_childNodeIdx, m_pChildNode );
    }

    void FPassthroughNode::InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime )
    {
        KRG_ASSERT( context.IsValid() );
        AnimationNode::InitializeInternal( context, initialTime );

        m_pChildNode->Initialize( context, initialTime );

        if ( m_pChildNode->IsValid() )
        {
            m_duration = m_pChildNode->GetDuration();
            m_previousTime = m_pChildNode->GetPreviousTime();
            m_currentTime = m_pChildNode->GetCurrentTime();
        }
        else
        {
            m_previousTime = m_currentTime = 0.0f;
            m_duration = 1.0f;
        }
    }

    void FPassthroughNode::ShutdownInternal( GraphContext& context )
    {
        m_pChildNode->Shutdown( context );
        AnimationNode::ShutdownInternal( context );
    }

    AnimationNode::UpdateResult FPassthroughNode::Update( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );
        MarkNodeActive( context );

        AnimationNode::UpdateResult result;

        // Forward child node results
        if ( IsChildValid() )
        {
            result = m_pChildNode->Update( context );
            m_duration = m_pChildNode->GetDuration();
            m_previousTime = m_pChildNode->GetPreviousTime();
            m_currentTime = m_pChildNode->GetCurrentTime();
        }
        else
        {
            result.m_sampledEventRange = SampledEventRange( context.m_sampledEvents.GetNumEvents() );
        }

        return result;
    }

    AnimationNode::UpdateResult FPassthroughNode::Update( GraphContext& context, SyncTrackTimeRange const& updateRange )
    {
        KRG_ASSERT( context.IsValid() );
        MarkNodeActive( context );

        AnimationNode::UpdateResult result;

        // Forward child node results
        if ( IsChildValid() )
        {
            result = m_pChildNode->Update( context, updateRange );
            m_duration = m_pChildNode->GetDuration();
            m_previousTime = m_pChildNode->GetPreviousTime();
            m_currentTime = m_pChildNode->GetCurrentTime();
        }
        else
        {
            result.m_sampledEventRange = SampledEventRange( context.m_sampledEvents.GetNumEvents() );
        }

        return result;
    }

    void FPassthroughNode::DeactivateBranch( GraphContext& context )
    {
        if ( IsValid() )
        {
            AnimationNode::DeactivateBranch( context );

            if ( IsChildValid() )
            {
                m_pChildNode->DeactivateBranch( context );
            }
        }
    }
}