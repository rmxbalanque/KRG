#include "AnimationToolsNode_StateMachine.h"
#include "AnimationToolsNode_State.h"
#include "AnimationToolsNode_EntryStateOverrides.h"
#include "AnimationToolsNode_Transitions.h"
#include "Tools/Animation/GraphEditor/ToolsGraph/AnimationToolsGraph_StateMachineGraph.h"
#include "Tools/Animation/GraphEditor/ToolsGraph/AnimationToolsGraph_Compilation.h"
#include "Tools/Animation/GraphEditor/ToolsGraph/Nodes/AnimationToolsNode_Results.h"
#include "Engine/Animation/Graph/Nodes/AnimationGraphNode_StateMachine.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void Tools_StateMachineNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        Tools_GraphNode::Initialize( pParent );
        CreateOutputPin( "Pose", ValueType::Pose );
        SetChildGraph( KRG::New<StateMachineGraph>() );
    }

    NodeIndex Tools_StateMachineNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        StateMachineNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<StateMachineNode>( this, pSettings );
        if ( state != NodeCompilationState::NeedCompilation )
        {
            return pSettings->m_nodeIdx;
        }

        // Get all necessary nodes for compilation
        //-------------------------------------------------------------------------
        
        auto pStateMachine = Cast<StateMachineGraph>( GetChildGraph() );
        auto stateNodes = pStateMachine->FindAllNodesOfType<Tools_StateBaseNode>( GraphEditor::SearchMode::Localized, GraphEditor::SearchTypeMatch::Derived );
        int32 const numStateNodes = (int32) stateNodes.size();
        KRG_ASSERT( numStateNodes >= 1 );

        auto conduitNodes = pStateMachine->FindAllNodesOfType<Tools_TransitionConduitNode>();
        int32 const numConduitNodes = (int32) conduitNodes.size();

        auto const globalTransitionNodes = pStateMachine->m_pGlobalTransitionsNode->GetSecondaryGraph()->FindAllNodesOfType<Tools_GlobalTransitionNode>();

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
                KRG_ASSERT( pEntryConditionNode->GetValueType() == ValueType::Bool );
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

        auto ConduitSearchPredicate = [] ( Tools_TransitionConduitNode* pConduit, UUID const& startStateID )
        {
            return pConduit->GetStartStateID() == startStateID;
        };

        for ( auto i = 0; i < numStateNodes; i++ )
        {
            auto pStateNode = stateNodes[i];

            // Check all conduits for any starting at the specified state
            //-------------------------------------------------------------------------

            // We need to ignore any global transitions that we have an explicit conduit for
            TInlineVector<Tools_GlobalTransitionNode*, 20> globalTransitionNodesCopy = globalTransitionNodes;
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

            auto TryCompileTransition = [&] ( Tools_TransitionNode const* pTransitionNode, UUID const& endStateID )
            {
                // Transitions are only enabled if a condition is connected to them
                auto pConditionNode = pTransitionNode->GetConnectedInputNode<Tools_GraphNode>( 0 );
                if ( pConditionNode != nullptr )
                {
                    KRG_ASSERT( pConditionNode->GetValueType() == ValueType::Bool );

                    auto& transitionSettings = pSettings->m_stateSettings[i].m_transitionSettings.emplace_back();
                    transitionSettings.m_targetStateIdx = IDToStateIdxMap[endStateID];

                    // Compile transition node
                    //-------------------------------------------------------------------------

                    transitionSettings.m_transitionNodeIdx = CompileTransition( context, pTransitionNode, IDToCompiledNodeIdxMap[endStateID] );
                    if ( transitionSettings.m_transitionNodeIdx == InvalidIndex )
                    {
                        return false;
                    }

                    // Compile condition tree
                    //-------------------------------------------------------------------------

                    TransitionNode::Settings* pCompiledTransitionSettings = nullptr;
                    NodeCompilationState const state = context.GetSettings<TransitionNode>( pTransitionNode, pCompiledTransitionSettings );
                    KRG_ASSERT( state == NodeCompilationState::AlreadyCompiled );

                    context.BeginTransitionConditionsCompilation( pCompiledTransitionSettings->m_duration, pCompiledTransitionSettings->m_durationOverrideNodeIdx );
                    transitionSettings.m_conditionNodeIdx = pConditionNode->Compile( context );
                    if ( transitionSettings.m_conditionNodeIdx == InvalidIndex )
                    {
                        return false;
                    }
                    context.EndTransitionConditionsCompilation();
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

                auto const foundSourceStateIter = IDToCompiledNodeIdxMap.find( pConduit->GetStartStateID() );
                KRG_ASSERT( foundSourceStateIter != IDToCompiledNodeIdxMap.end() );
                context.BeginConduitCompilation( foundSourceStateIter->second );

                // Compile transitions in conduit
                auto transitionNodes = pConduit->GetSecondaryGraph()->FindAllNodesOfType<Tools_TransitionNode>();
                for ( auto pTransitionNode : transitionNodes )
                {
                    if ( !TryCompileTransition( pTransitionNode, pConduit->GetEndStateID() ) )
                    {
                        return InvalidIndex;
                    }
                }

                context.EndConduitCompilation();
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

    NodeIndex Tools_StateMachineNode::CompileState( ToolsGraphCompilationContext& context, Tools_StateBaseNode const* pBaseStateNode ) const
    {
        KRG_ASSERT( pBaseStateNode != nullptr );

        StateNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<StateNode>( pBaseStateNode, pSettings );
        KRG_ASSERT( state == NodeCompilationState::NeedCompilation );

        //-------------------------------------------------------------------------

        for ( auto const& ID : pBaseStateNode->m_entryEvents ) { pSettings->m_entryEvents.emplace_back( ID ); }
        for ( auto const& ID : pBaseStateNode->m_executeEvents ) { pSettings->m_executeEvents.emplace_back( ID ); }
        for ( auto const& ID : pBaseStateNode->m_exitEvents ) { pSettings->m_exitEvents.emplace_back( ID ); }

        //-------------------------------------------------------------------------

        auto pBlendTreeStateNode = TryCast<Tools_StateNode>( pBaseStateNode );
        if ( pBlendTreeStateNode != nullptr )
        {
            // Compile Blend Tree
            //-------------------------------------------------------------------------

            auto pBlendTreeRoot = pBlendTreeStateNode->GetBlendTreeRootNode();
            KRG_ASSERT( pBlendTreeRoot != nullptr );

            auto pBlendTreeNode = pBlendTreeRoot->GetConnectedInputNode<Tools_GraphNode>( 0 );
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

            auto pLayerData = pBlendTreeStateNode->GetLayerDataNode();
            KRG_ASSERT( pLayerData != nullptr );

            auto pLayerWeightNode = pLayerData->GetConnectedInputNode<Tools_GraphNode>( 0 );
            if ( pLayerWeightNode != nullptr )
            {
                pSettings->m_layerWeightNodeIdx = pLayerWeightNode->Compile( context );
                if ( pSettings->m_layerWeightNodeIdx == InvalidIndex )
                {
                    return InvalidIndex;
                }
            }

            auto pLayerMaskNode = pLayerData->GetConnectedInputNode<Tools_GraphNode>( 1 );
            if ( pLayerMaskNode != nullptr )
            {
                pSettings->m_layerBoneMaskNodeIdx = pLayerMaskNode->Compile( context );
                if ( pSettings->m_layerBoneMaskNodeIdx == InvalidIndex )
                {
                    return InvalidIndex;
                }
            }

            // Transfer additional state events
            //-------------------------------------------------------------------------

            for ( auto const& evt : pBlendTreeStateNode->m_timeRemainingEvents ) { pSettings->m_timedRemainingEvents.emplace_back( StateNode::TimedEvent( evt.m_ID, evt.m_timeValue ) ); }
            for ( auto const& evt : pBlendTreeStateNode->m_timeElapsedEvents ) { pSettings->m_timedElapsedEvents.emplace_back( StateNode::TimedEvent( evt.m_ID, evt.m_timeValue ) ); }
        }
        else
        {
            auto pOffState = Cast<Tools_OffStateNode>( pBaseStateNode );
            pSettings->m_childNodeIdx = InvalidIndex;
            pSettings->m_isOffState = true;
        }

        //-------------------------------------------------------------------------

        return pSettings->m_nodeIdx;
    }

    NodeIndex Tools_StateMachineNode::CompileTransition( ToolsGraphCompilationContext& context, Tools_TransitionNode const* pTransitionNode, NodeIndex targetStateNodeIdx ) const
    {
        KRG_ASSERT( pTransitionNode != nullptr );
        TransitionNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<TransitionNode>( pTransitionNode, pSettings );
        if ( state == NodeCompilationState::AlreadyCompiled )
        {
            return pSettings->m_nodeIdx;
        }

        //-------------------------------------------------------------------------

        auto pDurationOverrideNode = pTransitionNode->GetConnectedInputNode<Tools_GraphNode>( 1 );
        if ( pDurationOverrideNode != nullptr )
        {
            KRG_ASSERT( pDurationOverrideNode->GetValueType() == ValueType::Float );
            pSettings->m_durationOverrideNodeIdx = pDurationOverrideNode->Compile( context );
            if ( pSettings->m_durationOverrideNodeIdx == InvalidIndex )
            {
                return InvalidIndex;
            }
        }

        auto pSyncEventOffsetOverrideNode = pTransitionNode->GetConnectedInputNode<Tools_GraphNode>( 2 );
        if ( pSyncEventOffsetOverrideNode != nullptr )
        {
            KRG_ASSERT( pSyncEventOffsetOverrideNode->GetValueType() == ValueType::Float );
            pSettings->m_syncEventOffsetOverrideNodeIdx = pSyncEventOffsetOverrideNode->Compile( context );
            if ( pSettings->m_syncEventOffsetOverrideNodeIdx == InvalidIndex )
            {
                return InvalidIndex;
            }
        }

        //-------------------------------------------------------------------------

        pSettings->m_targetStateNodeIdx = targetStateNodeIdx;
        pSettings->m_blendWeightEasingType = pTransitionNode->m_blendWeightEasingType;
        pSettings->m_rootMotionBlend = pTransitionNode->m_rootMotionBlend;
        pSettings->m_duration = pTransitionNode->m_duration;
        pSettings->m_syncEventOffset = pTransitionNode->m_syncEventOffset;

        pSettings->m_transitionOptions.SetFlag( TransitionNode::TransitionOptions::Synchronized, pTransitionNode->m_isSynchronized );
        pSettings->m_transitionOptions.SetFlag( TransitionNode::TransitionOptions::ClampDuration, pTransitionNode->m_clampDurationToSource );
        pSettings->m_transitionOptions.SetFlag( TransitionNode::TransitionOptions::KeepSyncEventIndex, pTransitionNode->m_keepSourceSyncEventIdx );
        pSettings->m_transitionOptions.SetFlag( TransitionNode::TransitionOptions::KeepSyncEventPercentage, pTransitionNode->m_keepSourceSyncEventPercentageThrough );
        pSettings->m_transitionOptions.SetFlag( TransitionNode::TransitionOptions::ForcedTransitionAllowed, pTransitionNode->m_canBeForced );

        //-------------------------------------------------------------------------

        return pSettings->m_nodeIdx;
    }
}