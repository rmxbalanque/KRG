#include "AnimationToolsNode_StateMachine.h"
#include "AnimationToolsNode_State.h"
#include "AnimationToolsNode_EntryStateOverrides.h"
#include "AnimationToolsNode_Transitions.h"
#include "Tools/Animation/Graph/AnimationGraphTools_StateMachineGraph.h"
#include "Tools/Animation/Graph/AnimationGraphTools_Compilation.h"
#include "Tools/Animation/Graph/Nodes/AnimationToolsNode_Results.h"
#include "Engine/Animation/Graph/Nodes/AnimationGraphNode_State.h"
#include "Engine/Animation/Graph/Nodes/AnimationGraphNode_StateMachine.h"
#include "Engine/Animation/Graph/Nodes/AnimationGraphNode_Transition.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void StateMachineToolsNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        FlowToolsNode::Initialize( pParent );
        CreateOutputPin( "Pose", NodeValueType::Pose );
        SetChildGraph( KRG::New<StateMachineToolsGraph>() );
    }

    NodeIndex StateMachineToolsNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        StateMachineNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<StateMachineNode>( this, pSettings );
        if ( state != NodeCompilationState::NeedCompilation )
        {
            return pSettings->m_nodeIdx;
        }

        // Get all necessary nodes for compilation
        //-------------------------------------------------------------------------
        
        auto pStateMachine = Cast<StateMachineToolsGraph>( GetChildGraph() );
        auto stateNodes = pStateMachine->FindAllNodesOfType<StateToolsNode>();
        int32 const numStateNodes = (int32) stateNodes.size();
        KRG_ASSERT( numStateNodes >= 1 );

        auto conduitNodes = pStateMachine->FindAllNodesOfType<TransitionConduitToolsNode>();
        int32 const numConduitNodes = (int32) conduitNodes.size();

        auto const globalTransitionNodes = pStateMachine->m_pGlobalTransitionsNode->GetSecondaryGraph()->FindAllNodesOfType<GlobalTransitionToolsNode>();

        // Compile all states
        //-------------------------------------------------------------------------

        THashMap<UUID, StateMachineNode::StateIndex> IDToStateIdxMap;
        THashMap<UUID, NodeIndex> IDToCompiledNodeIdxMap;

        for ( auto i = 0; i < numStateNodes; i++ )
        {
            auto pStateNode = stateNodes[i];

            // Compile state node
            auto& stateSettings = pSettings->m_stateSettings.emplace_back();
            stateSettings.m_stateNodeIdx = CompileState( context, pStateNode );
            if ( stateSettings.m_stateNodeIdx == InvalidIndex )
            {
                return InvalidIndex;
            }

            // Compile entry condition if it exists
            auto pEntryConditionNode = pStateMachine->m_pEntryOverridesNode->GetEntryConditionNodeForState( pStateNode->GetID() );
            if ( pEntryConditionNode != nullptr )
            {
                KRG_ASSERT( pEntryConditionNode->GetValueType() == NodeValueType::Bool );
                stateSettings.m_entryConditionNodeIdx = pEntryConditionNode->Compile( context );
                if ( stateSettings.m_entryConditionNodeIdx == InvalidIndex )
                {
                    return InvalidIndex;
                }
            }

            IDToStateIdxMap.insert( TPair<UUID, StateMachineNode::StateIndex>( pStateNode->GetID(), (StateMachineNode::StateIndex) i ) );
            IDToCompiledNodeIdxMap.insert( TPair<UUID, NodeIndex>( pStateNode->GetID(), stateSettings.m_stateNodeIdx ) );
        }

        // Compile all transitions
        //-------------------------------------------------------------------------

        auto ConduitSearchPredicate = [] ( TransitionConduitToolsNode* pConduit, UUID const& startStateID )
        {
            return pConduit->GetStartStateID() == startStateID;
        };

        for ( auto i = 0; i < numStateNodes; i++ )
        {
            auto pStateNode = stateNodes[i];

            // Check all conduits for any starting at the specified state
            //-------------------------------------------------------------------------

            // We need to ignore any global transitions that we have an explicit conduit for
            TInlineVector<GlobalTransitionToolsNode*, 20> globalTransitionNodesCopy = globalTransitionNodes;
            auto RemoveFromGlobalTransitions = [&globalTransitionNodesCopy] ( UUID const& endStateID )
            {
                for ( auto iter = globalTransitionNodesCopy.begin(); iter != globalTransitionNodesCopy.end(); ++iter )
                {
                    if ( ( *iter )->GetEndStateID() == endStateID )
                    {
                        globalTransitionNodesCopy.erase( iter );
                        break;
                    }
                }
            };

            auto TryCompileTransition = [&] ( TransitionToolsNode const* pTransitionNode, UUID const& endStateID )
            {
                // Transitions are only enabled if a condition is connected to them
                auto pConditionNode = pTransitionNode->GetConnectedInputNode<FlowToolsNode>( 0 );
                if ( pConditionNode != nullptr )
                {
                    KRG_ASSERT( pConditionNode->GetValueType() == NodeValueType::Bool );

                    auto& transitionSettings = pSettings->m_stateSettings[i].m_transitionSettings.emplace_back();
                    transitionSettings.m_targetStateIdx = IDToStateIdxMap[endStateID];

                    // Compile condition
                    transitionSettings.m_conditionNodeIdx = pConditionNode->Compile( context );
                    if ( transitionSettings.m_conditionNodeIdx == InvalidIndex )
                    {
                        return false;
                    }

                    // Compile transition
                    transitionSettings.m_transitionNodeIdx = CompileTransition( context, pTransitionNode, IDToCompiledNodeIdxMap[endStateID] );
                    if ( transitionSettings.m_transitionNodeIdx == InvalidIndex )
                    {
                        return false;
                    }
                }

                return true;
            };

            // Remove this state from the global transitions copy
            RemoveFromGlobalTransitions( pStateNode->GetID() );

            // Compile conduits
            for ( auto c = 0; c < numConduitNodes; c++ )
            {
                auto pConduit = conduitNodes[c];
                if ( pConduit->GetStartStateID() != pStateNode->GetID() )
                {
                    continue;
                }
                
                RemoveFromGlobalTransitions( pConduit->GetEndStateID() );

                // Compile transitions in conduit
                auto transitionNodes = pConduit->GetSecondaryGraph()->FindAllNodesOfType<TransitionToolsNode>();
                for ( auto pTransitionNode : transitionNodes )
                {
                    if ( !TryCompileTransition( pTransitionNode, pConduit->GetEndStateID() ) )
                    {
                        return InvalidIndex;
                    }
                }
            }

            // Global transitions
            //-------------------------------------------------------------------------

            for ( auto pGlobalTransition : globalTransitionNodesCopy )
            {
                if( !TryCompileTransition( pGlobalTransition, pGlobalTransition->GetEndStateID() ) )
                {
                    return InvalidIndex;
                }
            }
        }

        //-------------------------------------------------------------------------

        pSettings->m_defaultStateIndex = IDToStateIdxMap[pStateMachine->m_entryStateID];

        return pSettings->m_nodeIdx;
    }

    NodeIndex StateMachineToolsNode::CompileState( ToolsGraphCompilationContext& context, StateToolsNode const* pState ) const
    {
        KRG_ASSERT( pState != nullptr );

        StateNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<StateNode>( pState, pSettings );
        KRG_ASSERT( state == NodeCompilationState::NeedCompilation );

        //-------------------------------------------------------------------------

        auto pBlendTreeRoot = pState->GetBlendTreeRootNode();
        auto pLayerData = pState->GetLayerDataNode();
        KRG_ASSERT( pBlendTreeRoot != nullptr && pLayerData != nullptr );

        // Compile Blend Tree
        //-------------------------------------------------------------------------

        auto pBlendTreeNode = pBlendTreeRoot->GetConnectedInputNode<FlowToolsNode>( 0 );
        if ( pBlendTreeNode != nullptr )
        {
            pSettings->m_childNodeIdx = pBlendTreeNode->Compile( context );
            if ( pSettings->m_childNodeIdx == InvalidIndex )
            {
                return InvalidIndex;
            }
        }
        else
        {
            context.LogError( pBlendTreeRoot, "Disconnected blend tree root for state" );
            return InvalidIndex;
        }

        // Compile Layer Data
        //-------------------------------------------------------------------------

        auto pLayerWeightNode = pLayerData->GetConnectedInputNode<FlowToolsNode>( 0 );
        if ( pLayerWeightNode != nullptr )
        {
            pSettings->m_layerWeightNodeIdx = pLayerWeightNode->Compile( context );
            if ( pSettings->m_layerWeightNodeIdx == InvalidIndex )
            {
                return InvalidIndex;
            }
        }

        auto pLayerMaskNode = pLayerData->GetConnectedInputNode<FlowToolsNode>( 1 );
        if ( pLayerMaskNode != nullptr )
        {
            pSettings->m_layerBoneMaskNodeIdx = pLayerMaskNode->Compile( context );
            if ( pSettings->m_layerBoneMaskNodeIdx == InvalidIndex )
            {
                return InvalidIndex;
            }
        }

        // Transfer state events
        //-------------------------------------------------------------------------

        for ( auto const& ID : pState->m_entryEvents ) { pSettings->m_entryEvents.emplace_back( ID ); }
        for ( auto const& ID : pState->m_executeEvents ) { pSettings->m_executeEvents.emplace_back( ID ); }
        for ( auto const& ID : pState->m_exitEvents ) { pSettings->m_exitEvents.emplace_back( ID ); }
        for ( auto const& evt : pState->m_timeRemainingEvents ) { pSettings->m_timedRemainingEvents.emplace_back( StateNode::TimedEvent( evt.m_ID, evt.m_timeValue ) ); }
        for ( auto const& evt : pState->m_timeElapsedEvents ) { pSettings->m_timedElapsedEvents.emplace_back( StateNode::TimedEvent( evt.m_ID, evt.m_timeValue ) ); }

        //-------------------------------------------------------------------------

        return pSettings->m_nodeIdx;
    }

    NodeIndex StateMachineToolsNode::CompileTransition( ToolsGraphCompilationContext& context, TransitionToolsNode const* pTransition, NodeIndex targetStateNodeIdx ) const
    {
        KRG_ASSERT( pTransition != nullptr );
        TransitionNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<TransitionNode>( pTransition, pSettings );
        if ( state == NodeCompilationState::AlreadyCompiled )
        {
            return pSettings->m_nodeIdx;
        }

        //-------------------------------------------------------------------------

        auto pDurationOverrideNode = pTransition->GetConnectedInputNode<FlowToolsNode>( 1 );
        if ( pDurationOverrideNode != nullptr )
        {
            KRG_ASSERT( pDurationOverrideNode->GetValueType() == NodeValueType::Float );
            pSettings->m_durationOverrideNodeIdx = pDurationOverrideNode->Compile( context );
            if ( pSettings->m_durationOverrideNodeIdx == InvalidIndex )
            {
                return InvalidIndex;
            }
        }

        auto pSyncEventOffsetOverrideNode = pTransition->GetConnectedInputNode<FlowToolsNode>( 2 );
        if ( pSyncEventOffsetOverrideNode != nullptr )
        {
            KRG_ASSERT( pSyncEventOffsetOverrideNode->GetValueType() == NodeValueType::Float );
            pSettings->m_syncEventOffsetOverrideNodeIdx = pSyncEventOffsetOverrideNode->Compile( context );
            if ( pSettings->m_syncEventOffsetOverrideNodeIdx == InvalidIndex )
            {
                return InvalidIndex;
            }
        }

        //-------------------------------------------------------------------------

        pSettings->m_targetStateNodeIdx = targetStateNodeIdx;
        pSettings->m_blendWeightEasingType = pTransition->m_blendWeightEasingType;
        pSettings->m_rootMotionBlend = pTransition->m_rootMotionBlend;
        pSettings->m_duration = pTransition->m_duration;
        pSettings->m_syncEventOffset = pTransition->m_syncEventOffset;

        pSettings->m_transitionOptions.SetFlag( TransitionNode::TransitionOptions::Synchronized, pTransition->m_isSynchronized );
        pSettings->m_transitionOptions.SetFlag( TransitionNode::TransitionOptions::ClampDuration, pTransition->m_clampDurationToSource );
        pSettings->m_transitionOptions.SetFlag( TransitionNode::TransitionOptions::KeepSyncEventIndex, pTransition->m_keepSourceSyncEventIdx );
        pSettings->m_transitionOptions.SetFlag( TransitionNode::TransitionOptions::KeepSyncEventPercentage, pTransition->m_keepSourceSyncEventPercentageThrough );
        pSettings->m_transitionOptions.SetFlag( TransitionNode::TransitionOptions::ForcedTransitionAllowed, pTransition->m_canBeForced );

        //-------------------------------------------------------------------------

        return pSettings->m_nodeIdx;
    }
}