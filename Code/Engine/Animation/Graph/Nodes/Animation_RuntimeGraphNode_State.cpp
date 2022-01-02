#include "Animation_RuntimeGraphNode_State.h"
#include "System/Core/Types/ScopedValue.h"
#include "Engine/Animation/AnimationBoneMask.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::GraphNodes
{
    void StateNode::Settings::InstantiateNode( TVector<GraphNode*> const& nodePtrs, GraphDataSet const* pDataSet, InitOptions options ) const
    {
        auto pNode = CreateNode<StateNode>( nodePtrs, options );
        SetOptionalNodePtrFromIndex( nodePtrs, m_layerBoneMaskNodeIdx, pNode->m_pBoneMaskNode );
        SetOptionalNodePtrFromIndex( nodePtrs, m_layerWeightNodeIdx, pNode->m_pLayerWeightNode );
        PassthroughNode::Settings::InstantiateNode( nodePtrs, pDataSet, GraphNode::Settings::InitOptions::OnlySetPointers );
    }

    //-------------------------------------------------------------------------

    void StateNode::InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime )
    {
        KRG_ASSERT( context.IsValid() );
        auto pStateSettings = GetSettings<StateNode>();

        PassthroughNode::InitializeInternal( context, initialTime );
        m_transitionState = TransitionState::None;
        m_elapsedTimeInState = 0.0f;
        m_sampledEventRange = SampledEventRange( context.m_sampledEvents.GetNumEvents() );

        if ( m_pBoneMaskNode != nullptr )
        {
            m_pBoneMaskNode->Initialize( context );
        }

        if ( m_pLayerWeightNode != nullptr )
        {
            m_pLayerWeightNode->Initialize( context );
        }

        // Flag this as the first update for this state, this will cause state entry events to be sampled for at least one update
        m_isFirstStateUpdate = true;
    }

    void StateNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );
        auto pStateSettings = GetSettings<StateNode>();

        // Ensure that we get the exit Events when we destroy the state
        for ( auto const& exitEventID : pStateSettings->m_exitEvents )
        {
            context.m_sampledEvents.EmplaceStateEvent( GetNodeIndex(), SampledEvent::Flags::StateExit, exitEventID );
        }

        if ( m_pBoneMaskNode != nullptr )
        {
            m_pBoneMaskNode->Shutdown( context );
        }

        if ( m_pLayerWeightNode != nullptr )
        {
            m_pLayerWeightNode->Shutdown( context );
        }

        m_transitionState = TransitionState::None;
        PassthroughNode::ShutdownInternal( context );
    }

    void StateNode::StartTransitionIn( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );
        m_transitionState = TransitionState::TransitioningIn;
    }

    void StateNode::StartTransitionOut( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );

        m_transitionState = TransitionState::TransitioningOut;

        // Since we update states before we register transitions, we need to ignore all previously sampled state events for this frame
        for ( auto i = m_sampledEventRange.m_startIdx; i < m_sampledEventRange.m_endIdx; i++ )
        {
            if ( context.m_sampledEvents[i].GetSourceNodeIndex() == GetNodeIndex() )
            {
                if ( context.m_sampledEvents[i].IsStateEvent() )
                {
                    context.m_sampledEvents[i].SetFlag( SampledEvent::Flags::Ignored, true );
                }
            }
        }

        // Deactivate this branch and re-sample the state events
        {
            TScopedGuardValue<BranchState> const branchStateScopedValue( context.m_branchState, BranchState::Inactive );
            SampleStateEvents( context );
            DeactivateBranch( context );
        }
    }

    void StateNode::SampleStateEvents( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );
        auto pStateSettings = GetSettings<StateNode>();

        // Sample Fixed Stage Events
        //-------------------------------------------------------------------------

        if ( m_isFirstStateUpdate || ( m_transitionState == TransitionState::TransitioningIn && context.m_branchState == BranchState::Active ) )
        {
            for ( auto const& entryEventID : pStateSettings->m_entryEvents )
            {
                context.m_sampledEvents.EmplaceStateEvent( GetNodeIndex(), SampledEvent::Flags::StateEntry, entryEventID );
            }
        }
        else if ( m_transitionState == TransitionState::None && context.m_branchState == BranchState::Active )
        {
            for ( auto const& executeEventID : pStateSettings->m_executeEvents )
            {
                context.m_sampledEvents.EmplaceStateEvent( GetNodeIndex(), SampledEvent::Flags::StateExecute, executeEventID );
            }
        }
        else if ( m_transitionState == TransitionState::TransitioningOut || context.m_branchState == BranchState::Inactive )
        {
            for ( auto const& exitEventID : pStateSettings->m_exitEvents )
            {
                context.m_sampledEvents.EmplaceStateEvent( GetNodeIndex(), SampledEvent::Flags::StateExit, exitEventID );
            }
        }

        // Sample Timed Events
        //-------------------------------------------------------------------------

        Seconds const currentTimeElapsed = m_currentTime * m_duration;
        for ( auto const& timedEvent : pStateSettings->m_timedElapsedEvents )
        {
            if ( currentTimeElapsed >= timedEvent.m_timeValue )
            {
                context.m_sampledEvents.EmplaceStateEvent( GetNodeIndex(), SampledEvent::Flags::StateTimed, timedEvent.m_ID );
            }
        }

        Seconds const currentTimeRemaining = ( 1.0f - m_currentTime ) * m_duration;
        for ( auto const& timedEvent : pStateSettings->m_timedRemainingEvents )
        {
            if ( currentTimeRemaining <= timedEvent.m_timeValue )
            {
                context.m_sampledEvents.EmplaceStateEvent( GetNodeIndex(), SampledEvent::Flags::StateTimed, timedEvent.m_ID );
            }
        }
    }

    void StateNode::UpdateLayerContext( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );

        // Early out if we are not in a layer
        if ( !context.m_layerContext.IsSet() )
        {
            return;
        }

        // Update layer weight
        //-------------------------------------------------------------------------

        auto pStateSettings = GetSettings<StateNode>();
        if ( pStateSettings->m_isOffState )
        {
            context.m_layerContext.m_layerWeight = 0.0f;
        }
        else if ( m_pLayerWeightNode != nullptr )
        {
            context.m_layerContext.m_layerWeight *= Math::Clamp( m_pLayerWeightNode->GetValue<float>( context ), 0.0f, 1.0f );
        }

        // Update bone mask
        //-------------------------------------------------------------------------

        if ( m_pBoneMaskNode != nullptr )
        {
            auto pBoneMask = m_pBoneMaskNode->GetValue<BoneMask const*>( context );
            if ( pBoneMask != nullptr )
            {
                // If we dont have a bone mask set, use a copy of the state's mask
                if ( context.m_layerContext.m_pLayerMask == nullptr )
                {
                    context.m_layerContext.m_pLayerMask = context.m_boneMaskPool.GetBoneMask();
                    context.m_layerContext.m_pLayerMask->CopyFrom( *pBoneMask );
                }
                else // If we already have a bone mask set, combine the bone masks
                {
                    context.m_layerContext.m_pLayerMask->CombineWith( *pBoneMask );
                }
            }
        }
    }

    GraphPoseNodeResult StateNode::Update( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );

        // Update child
        GraphPoseNodeResult result;
        result = PassthroughNode::Update( context );
        m_elapsedTimeInState += context.m_deltaTime;

        // Sample graph events ( we need to track the sampled range for this node explicitly )
        m_sampledEventRange = result.m_sampledEventRange;
        SampleStateEvents( context );
        m_sampledEventRange.m_endIdx = context.m_sampledEvents.GetNumEvents();

        // Update layer context and return
        UpdateLayerContext( context );
        m_isFirstStateUpdate = false;
        return result;
    }

    GraphPoseNodeResult StateNode::Update( GraphContext& context, SyncTrackTimeRange const& updateRange )
    {
        KRG_ASSERT( context.IsValid() );

        // Update child
        GraphPoseNodeResult result;
        result = PassthroughNode::Update( context, updateRange );
        m_elapsedTimeInState += context.m_deltaTime;

        // Sample graph events ( we need to track the sampled range for this node explicitly )
        m_sampledEventRange = result.m_sampledEventRange;
        SampleStateEvents( context );
        m_sampledEventRange.m_endIdx = context.m_sampledEvents.GetNumEvents();

        // Update layer context and return
        UpdateLayerContext( context );
        m_isFirstStateUpdate = false;
        return result;
    }

    void StateNode::DeactivateBranch( GraphContext& context )
    {
        KRG_ASSERT( context.IsValid() );

        if ( IsValid() )
        {
            PassthroughNode::DeactivateBranch( context );

            // Update all previously sampled events from this state to be from the inactive branch
            for ( auto i = m_sampledEventRange.m_startIdx; i < m_sampledEventRange.m_endIdx; i++ )
            {
                context.m_sampledEvents[i].SetFlag( SampledEvent::Flags::FromInactiveBranch, true );
            }

            // Force sample exit events when we are being deactivated (may result in duplicate exit event for a frame)
            auto pStateSettings = GetSettings<StateNode>();
            for ( auto const& exitEventID : pStateSettings->m_exitEvents )
            {
                context.m_sampledEvents.EmplaceStateEvent( GetNodeIndex(), SampledEvent::Flags::StateExit, exitEventID );
            }
        }
    }
}