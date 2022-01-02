#include "Animation_RuntimeGraphNode_Selector.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::GraphNodes
{
    void SelectorNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, GraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<SelectorNode>( nodePtrs, options );

        for ( auto nodeIdx : m_optionNodeIndices )
        {
            SetNodePtrFromIndex( nodePtrs, nodeIdx, pNode->m_optionNodes.emplace_back() );
        }

        for ( auto nodeIdx : m_conditionNodeIndices )
        {
            SetNodePtrFromIndex( nodePtrs, nodeIdx, pNode->m_conditions.emplace_back() );
        }
    }

    int32 SelectorNode::SelectOption( GraphContext& context ) const
    {
        KRG_ASSERT( context.IsValid() );
        auto pSettings = GetSettings<SelectorNode>();

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

    void SelectorNode::InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime )
    {
        KRG_ASSERT( context.IsValid() );
        auto pSettings = GetSettings<SelectorNode>();

        PoseNode::InitializeInternal( context, initialTime );

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

    void SelectorNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );

        if ( m_pSelectedNode != nullptr )
        {
            m_pSelectedNode->Shutdown( context );
            m_pSelectedNode = nullptr;
        }

        PoseNode::ShutdownInternal( context );
    }

    GraphPoseNodeResult SelectorNode::Update( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );

        GraphPoseNodeResult result;
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

    GraphPoseNodeResult SelectorNode::Update( GraphContext& context, SyncTrackTimeRange const& updateRange )
    {
        KRG_ASSERT( context.IsValid() );

        GraphPoseNodeResult result;
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

    void SelectorNode::DeactivateBranch( GraphContext& context )
    {
        if ( IsValid() )
        {
            PoseNode::DeactivateBranch( context );
            m_pSelectedNode->DeactivateBranch( context );
        }
    }

    //-------------------------------------------------------------------------

    void AnimationSelectorNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, GraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<AnimationSelectorNode>( nodePtrs, options );

        for ( auto nodeIdx : m_optionNodeIndices )
        {
            SetNodePtrFromIndex( nodePtrs, nodeIdx, pNode->m_optionNodes.emplace_back() );
        }

        for ( auto nodeIdx : m_conditionNodeIndices )
        {
            SetNodePtrFromIndex( nodePtrs, nodeIdx, pNode->m_conditions.emplace_back() );
        }
    }

    int32 AnimationSelectorNode::SelectOption( GraphContext& context ) const
    {
        KRG_ASSERT( context.IsValid() );
        auto pSettings = GetSettings<SelectorNode>();

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

    void AnimationSelectorNode::InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime )
    {
        KRG_ASSERT( context.IsValid() );
        auto pSettings = GetSettings<SelectorNode>();

        PoseNode::InitializeInternal( context, initialTime );

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

    void AnimationSelectorNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );

        if ( m_pSelectedNode != nullptr )
        {
            m_pSelectedNode->Shutdown( context );
            m_pSelectedNode = nullptr;
        }

        PoseNode::ShutdownInternal( context );
    }

    GraphPoseNodeResult AnimationSelectorNode::Update( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );

        GraphPoseNodeResult result;
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

    GraphPoseNodeResult AnimationSelectorNode::Update( GraphContext& context, SyncTrackTimeRange const& updateRange )
    {
        KRG_ASSERT( context.IsValid() );

        GraphPoseNodeResult result;
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

    void AnimationSelectorNode::DeactivateBranch( GraphContext& context )
    {
        if ( IsValid() )
        {
            PoseNode::DeactivateBranch( context );
            m_pSelectedNode->DeactivateBranch( context );
        }
    }

    AnimationClip const* AnimationSelectorNode::GetAnimation() const
    {
        if ( m_pSelectedNode != nullptr )
        {
            return m_pSelectedNode->GetAnimation();
        }

        return nullptr;
    }

    void AnimationSelectorNode::DisableRootMotionSampling()
    {
        if ( m_pSelectedNode != nullptr )
        {
            m_pSelectedNode->DisableRootMotionSampling();
        }
    }

    bool AnimationSelectorNode::IsLooping() const
    {
        if ( m_pSelectedNode != nullptr )
        {
            return m_pSelectedNode->IsLooping();
        }

        return false;
    }
}