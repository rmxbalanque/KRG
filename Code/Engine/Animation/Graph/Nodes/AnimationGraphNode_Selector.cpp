#include "AnimationGraphNode_Selector.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void FSelectorNode::OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet )
    {
        AnimationNode::OnConstruct( pSettings, nodePtrs, dataSet );
        auto pNodeSettings = GetSettings<FSelectorNode>();

        for ( auto nodeIdx : pNodeSettings->m_optionNodeIndices )
        {
            SetNodePtrFromIndex( nodePtrs, nodeIdx, m_optionNodes.emplace_back() );
            KRG_ASSERT( m_optionNodes.back() != nullptr );
        }

        for ( auto nodeIdx : pNodeSettings->m_conditionNodeIndices )
        {
            SetNodePtrFromIndex( nodePtrs, nodeIdx, m_conditions.emplace_back() );
            KRG_ASSERT( m_conditions.back() != nullptr );
        }
    }

    int32 FSelectorNode::SelectOption( GraphContext& context ) const
    {
        KRG_ASSERT( context.IsValid() );
        auto pSettings = GetSettings<FSelectorNode>();

        // Select a valid option
        if ( m_optionNodes.size() > 0 )
        {
            int32 SelectedIdx = InvalidIndex;

            auto const numOptions = m_optionNodes.size();
            KRG_ASSERT( m_optionNodes.size() == m_conditions.size() );

            // Initialize conditions
            for ( auto pConditionNode : m_conditions )
            {
                pConditionNode->Initialize( context );
            }

            // Check all conditions
            for ( auto i = 0; i < numOptions; i++ )
            {
                if ( m_conditions[i]->GetValue<bool>( context ) )
                {
                    SelectedIdx = i;
                    break;
                }
            }

            // Shutdown all conditions
            for ( auto pConditionNode : m_conditions )
            {
                pConditionNode->Shutdown( context );
            }

            return SelectedIdx;
        }
        else
        {
            return InvalidIndex;
        }
    }

    void FSelectorNode::InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime )
    {
        KRG_ASSERT( context.IsValid() );
        auto pSettings = GetSettings<FSelectorNode>();

        AnimationNode::InitializeInternal( context, initialTime );

        // Select option and try to create transient data for it
        m_selectedOptionIdx = SelectOption( context );
        if ( m_selectedOptionIdx != InvalidIndex )
        {
            m_pSelectedNode = m_optionNodes[m_selectedOptionIdx];
            m_pSelectedNode->Initialize( context, initialTime );

            if ( m_pSelectedNode->IsValid() )
            {
                m_duration = m_pSelectedNode->GetDuration();
                m_previousTime = m_pSelectedNode->GetPreviousTime();
                m_currentTime = m_pSelectedNode->GetCurrentTime();
            }
            else
            {
                m_pSelectedNode->Shutdown( context );
                m_pSelectedNode = nullptr;
            }
        }
        else
        {
            KRG_LOG_WARNING( "Animation", "Failed to select a valid option!" );
        }
    }

    void FSelectorNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );

        if ( m_pSelectedNode != nullptr )
        {
            m_pSelectedNode->Shutdown( context );
            m_pSelectedNode = nullptr;
        }

        AnimationNode::ShutdownInternal( context );
    }

    UpdateResult FSelectorNode::Update( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );

        UpdateResult result;
        if ( IsValid() )
        {
            MarkNodeActive( context );

            // Copy node instance data, this node acts like a passthrough
            result = m_pSelectedNode->Update( context );
            m_duration = m_pSelectedNode->GetDuration();
            m_previousTime = m_pSelectedNode->GetPreviousTime();
            m_currentTime = m_pSelectedNode->GetCurrentTime();
            KRG_ASSERT( context.m_sampledEvents.IsValidRange( result.m_sampledEventRange ) );
        }
        else
        {
            result.m_sampledEventRange = SampledEventRange( context.m_sampledEvents.GetNumEvents() );
        }

        return result;
    }

    UpdateResult FSelectorNode::Update( GraphContext& context, SyncTrackTimeRange const& updateRange )
    {
        KRG_ASSERT( context.IsValid() );

        UpdateResult result;
        if ( IsValid() )
        {
            MarkNodeActive( context );

            // Copy node instance data, this node acts like a passthrough
            result = m_pSelectedNode->Update( context, updateRange );
            m_duration = m_pSelectedNode->GetDuration();
            m_previousTime = m_pSelectedNode->GetPreviousTime();
            m_currentTime = m_pSelectedNode->GetCurrentTime();
            KRG_ASSERT( context.m_sampledEvents.IsValidRange( result.m_sampledEventRange ) );
        }
        else
        {
            result.m_sampledEventRange = SampledEventRange( context.m_sampledEvents.GetNumEvents() );
        }

        return result;
    }

    void FSelectorNode::DeactivateBranch( GraphContext& context )
    {
        if ( IsValid() )
        {
            AnimationNode::DeactivateBranch( context );
            m_pSelectedNode->DeactivateBranch( context );
        }
    }

    //-------------------------------------------------------------------------

    void FAnimationSelectorNode::OnConstruct( GraphNode::Settings const* pSettings, TVector<GraphNode*> const& nodePtrs, AnimationGraphDataSet const& dataSet )
    {
        AnimationNode::OnConstruct( pSettings, nodePtrs, dataSet );
        auto pNodeSettings = GetSettings<FSelectorNode>();

        for ( auto nodeIdx : pNodeSettings->m_optionNodeIndices )
        {
            SetNodePtrFromIndex( nodePtrs, nodeIdx, m_optionNodes.emplace_back() );
            KRG_ASSERT( m_optionNodes.back() != nullptr );
        }

        for ( auto nodeIdx : pNodeSettings->m_conditionNodeIndices )
        {
            SetNodePtrFromIndex( nodePtrs, nodeIdx, m_conditions.emplace_back() );
            KRG_ASSERT( m_conditions.back() != nullptr );
        }
    }

    int32 FAnimationSelectorNode::SelectOption( GraphContext& context ) const
    {
        KRG_ASSERT( context.IsValid() );
        auto pSettings = GetSettings<FSelectorNode>();

        // Select a valid option
        if ( m_optionNodes.size() > 0 )
        {
            int32 selectedIdx = InvalidIndex;
            auto const numOptions = m_optionNodes.size();
            KRG_ASSERT( m_optionNodes.size() == m_conditions.size() );

            // Initialize conditions
            for ( auto pConditionNode : m_conditions )
            {
                pConditionNode->Initialize( context );
            }

            // Check all conditions
            for ( auto i = 0; i < numOptions; i++ )
            {
                if ( m_conditions[i]->GetValue<bool>( context ) )
                {
                    selectedIdx = i;
                    break;
                }
            }

            // Shutdown all conditions
            for ( auto pConditionNode : m_conditions )
            {
                pConditionNode->Shutdown( context );
            }

            return selectedIdx;
        }
        else
        {
            return InvalidIndex;
        }
    }

    void FAnimationSelectorNode::InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime )
    {
        KRG_ASSERT( context.IsValid() );
        auto pSettings = GetSettings<FSelectorNode>();

        AnimationNode::InitializeInternal( context, initialTime );

        // Select option and try to create transient data for it
        m_selectedOptionIdx = SelectOption( context );
        if ( m_selectedOptionIdx != InvalidIndex )
        {
            m_pSelectedNode = m_optionNodes[m_selectedOptionIdx];
            m_pSelectedNode->Initialize( context, initialTime );

            if ( m_pSelectedNode->IsValid() )
            {
                m_duration = m_pSelectedNode->GetDuration();
                m_previousTime = m_pSelectedNode->GetPreviousTime();
                m_currentTime = m_pSelectedNode->GetCurrentTime();
            }
            else
            {
                m_pSelectedNode->Shutdown( context );
                m_pSelectedNode = nullptr;
            }
        }
        else
        {
            KRG_LOG_WARNING( "Animation", "Failed to select a valid option!" );
        }
    }

    void FAnimationSelectorNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );

        if ( m_pSelectedNode != nullptr )
        {
            m_pSelectedNode->Shutdown( context );
            m_pSelectedNode = nullptr;
        }

        AnimationNode::ShutdownInternal( context );
    }

    UpdateResult FAnimationSelectorNode::Update( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );

        UpdateResult result;
        if ( IsValid() )
        {
            MarkNodeActive( context );

            // Copy node instance data, this node acts like a passthrough
            result = m_pSelectedNode->Update( context );
            m_duration = m_pSelectedNode->GetDuration();
            m_previousTime = m_pSelectedNode->GetPreviousTime();
            m_currentTime = m_pSelectedNode->GetCurrentTime();
            KRG_ASSERT( context.m_sampledEvents.IsValidRange( result.m_sampledEventRange ) );
        }
        else
        {
            result.m_sampledEventRange = SampledEventRange( context.m_sampledEvents.GetNumEvents() );
        }

        return result;
    }

    UpdateResult FAnimationSelectorNode::Update( GraphContext& context, SyncTrackTimeRange const& updateRange )
    {
        KRG_ASSERT( context.IsValid() );

        UpdateResult result;
        if ( IsValid() )
        {
            MarkNodeActive( context );

            // Copy node instance data, this node acts like a passthrough
            result = m_pSelectedNode->Update( context, updateRange );
            m_duration = m_pSelectedNode->GetDuration();
            m_previousTime = m_pSelectedNode->GetPreviousTime();
            m_currentTime = m_pSelectedNode->GetCurrentTime();
            KRG_ASSERT( context.m_sampledEvents.IsValidRange( result.m_sampledEventRange ) );
        }
        else
        {
            result.m_sampledEventRange = SampledEventRange( context.m_sampledEvents.GetNumEvents() );
        }

        return result;
    }

    void FAnimationSelectorNode::DeactivateBranch( GraphContext& context )
    {
        if ( IsValid() )
        {
            AnimationNode::DeactivateBranch( context );
            m_pSelectedNode->DeactivateBranch( context );
        }
    }

    AnimationClip const* FAnimationSelectorNode::GetAnimation() const
    {
        if ( m_pSelectedNode != nullptr )
        {
            return m_pSelectedNode->GetAnimation();
        }

        return nullptr;
    }

    void FAnimationSelectorNode::DisableRootMotionSampling()
    {
        if ( m_pSelectedNode != nullptr )
        {
            m_pSelectedNode->DisableRootMotionSampling();
        }
    }

    bool FAnimationSelectorNode::IsLooping() const
    {
        if ( m_pSelectedNode != nullptr )
        {
            return m_pSelectedNode->IsLooping();
        }

        return false;
    }
}