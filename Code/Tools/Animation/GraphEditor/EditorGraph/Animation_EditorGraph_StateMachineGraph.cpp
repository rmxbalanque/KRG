#include "Animation_EditorGraph_StateMachineGraph.h"
#include "Animation_EditorGraph_Compilation.h"
#include "Engine/Animation/Graph/Nodes/Animation_RuntimeGraphNode_StateMachine.h"

//-------------------------------------------------------------------------
// STATE NODES
//-------------------------------------------------------------------------

namespace KRG::Animation::GraphNodes
{
    void StateBaseEditorNode::DrawExtraControls( VisualGraph::DrawContext const& ctx )
    {
        // State events
        //-------------------------------------------------------------------------

        InlineString string;
        auto CreateEventString = [&] ( TVector<StringID> const& IDs )
        {
            string.clear();
            for ( int32 i = 0; i < (int32) IDs.size(); i++ )
            {
                if ( !IDs[i].IsValid() )
                {
                    continue;
                }

                string += IDs[i].c_str();

                if ( i != IDs.size() - 1 )
                {
                    string += ", ";
                }
            }
        };

        auto CreateTimedEventString = [&] ( TVector<TimedStateEvent> const& events )
        {
            string.clear();
            for ( int32 i = 0; i < (int32) events.size(); i++ )
            {
                if ( !events[i].m_ID.IsValid() )
                {
                    continue;
                }

                string += events[i].m_ID.c_str();

                if ( i != events.size() - 1 )
                {
                    string += ", ";
                }
            }
        };

        CreateEventString( m_entryEvents );
        ImGui::Text( "Entry: %s", string.c_str() );

        CreateEventString( m_executeEvents );
        ImGui::Text( "Execute: %s", string.c_str() );

        CreateEventString( m_exitEvents );
        ImGui::Text( "Exit: %s", string.c_str() );

        CreateTimedEventString( m_timeRemainingEvents );
        ImGui::Text( "Time Left: %s", string.c_str() );

        CreateTimedEventString( m_timeElapsedEvents );
        ImGui::Text( "Time Elapsed: %s", string.c_str() );

        // Draw separator
        //-------------------------------------------------------------------------

        ImVec2 const originalCursorPos = ImGui::GetCursorScreenPos();
        float const width = Math::Max( GetSize().x, 40.0f );
        ImGui::InvisibleButton( "Spacer", ImVec2( width, 10 ) );
        ctx.m_pDrawList->AddLine( originalCursorPos + ImVec2( 0, 4 ), originalCursorPos + ImVec2( GetSize().x, 4 ), ImColor( ImGuiX::Style::s_itemColorLight ) );

        // Draw runtime debug info
        //-------------------------------------------------------------------------

        bool shouldDrawEmptyDebugInfoBlock = true;
        auto pDebugContext = reinterpret_cast<DebugContext*>( ctx.m_pUserContext );
        if ( pDebugContext != nullptr )
        {
            GraphNodeIndex runtimeNodeIdx = pDebugContext->GetRuntimeGraphNodeIndex( GetID() );
            if ( runtimeNodeIdx != InvalidIndex && pDebugContext->IsNodeActive( runtimeNodeIdx ) )
            {
                PoseNodeDebugInfo const debugInfo = pDebugContext->GetPoseNodeDebugInfo( runtimeNodeIdx );
                DrawPoseNodeDebugInfo( ctx, GetSize().x, debugInfo );
                shouldDrawEmptyDebugInfoBlock = false;
            }
        }

        if ( shouldDrawEmptyDebugInfoBlock )
        {
            DrawEmptyPoseNodeDebugInfo( ctx, GetSize().x );
        }
    }

    //-------------------------------------------------------------------------

    void StateLayerDataEditorNode::Initialize( VisualGraph::BaseGraph* pParent )
    {
        EditorGraphNode::Initialize( pParent );
        CreateInputPin( "Layer Weight", GraphValueType::Float );
        CreateInputPin( "Layer Mask", GraphValueType::BoneMask );
    }

    //-------------------------------------------------------------------------

    void StateEditorNode::Initialize( VisualGraph::BaseGraph* pParent )
    {
        VisualGraph::SM::State::Initialize( pParent );

        auto pBlendTree = KRG::New<FlowGraph>( GraphType::BlendTree );
        pBlendTree->CreateNode<ResultEditorNode>( GraphValueType::Pose );
        SetChildGraph( pBlendTree );

        auto pValueTree = KRG::New<FlowGraph>( GraphType::ValueTree );
        pValueTree->CreateNode<StateLayerDataEditorNode>();
        SetSecondaryGraph( pValueTree );
    }

    ResultEditorNode const* StateEditorNode::GetBlendTreeRootNode() const
    {
        auto resultNodes = GetChildGraph()->FindAllNodesOfType<ResultEditorNode>();
        KRG_ASSERT( resultNodes.size() == 1 );
        return resultNodes[0];
    }

    StateLayerDataEditorNode const* StateEditorNode::GetLayerDataNode() const
    {
        auto dataNodes = GetSecondaryGraph()->FindAllNodesOfType<StateLayerDataEditorNode>();
        KRG_ASSERT( dataNodes.size() == 1 );
        return dataNodes[0];
    }

    //-------------------------------------------------------------------------

    void OffStateEditorNode::DrawExtraControls( VisualGraph::DrawContext const& ctx )
    {
        {
            ImGuiX::ScopedFont font( ImGuiX::Font::Large, Colors::Red );
            ImGui::Text( KRG_ICON_TIMES );
        }

        StateBaseEditorNode::DrawExtraControls( ctx );
    }
}

//-------------------------------------------------------------------------
// ENTRY STATE OVERRIDES
//-------------------------------------------------------------------------

namespace KRG::Animation::GraphNodes
{
    void EntryStateOverrideConditionsEditorNode::UpdateInputPins()
    {
        auto pEntryStateOverridesNode = GetParentGraph()->GetParentNode();
        KRG_ASSERT( pEntryStateOverridesNode != nullptr );

        auto pParentStateMachineGraph = pEntryStateOverridesNode->GetParentGraph();
        KRG_ASSERT( pParentStateMachineGraph != nullptr );

        auto stateNodes = pParentStateMachineGraph->FindAllNodesOfType<StateBaseEditorNode>( VisualGraph::SearchMode::Localized, VisualGraph::SearchTypeMatch::Derived );

        //-------------------------------------------------------------------------

        int32 const numOriginalInputPins = GetNumInputPins();

        TInlineVector<StateBaseEditorNode*, 20> pinsToCreate;
        TInlineVector<UUID, 20> pinsToRemove;

        for ( auto const& pin : GetInputPins() )
        {
            pinsToRemove.emplace_back( pin.m_ID );
        }

        // Check which pins are invalid
        //-------------------------------------------------------------------------

        for ( auto pState : stateNodes )
        {
            int32 const pinIdx = VectorFindIndex( m_pinToStateMapping, pState->GetID() );
            if ( pinIdx == InvalidIndex )
            {
                pinsToCreate.emplace_back( pState );
            }
            else // Found the pin
            {
                pinsToRemove[pinIdx].Clear();
                auto pInputPin = GetInputPin( pinIdx );
                pInputPin->m_name = pState->GetDisplayName();
            }
        }

        // Remove invalid pins
        //-------------------------------------------------------------------------

        for ( auto const& pinID : pinsToRemove )
        {
            if ( pinID.IsValid() )
            {
                int32 const pinIdx = GetInputPinIndex( pinID );
                DestroyInputPin( pinIdx );
                m_pinToStateMapping.erase( m_pinToStateMapping.begin() + pinIdx );
            }
        }

        // Add new pins
        //-------------------------------------------------------------------------

        for ( auto pState : pinsToCreate )
        {
            CreateInputPin( pState->GetDisplayName(), GraphValueType::Bool );
            m_pinToStateMapping.emplace_back( pState->GetID() );
        }
    }

    //-------------------------------------------------------------------------

    void EntryStateOverrideConduitEditorNode::Initialize( VisualGraph::BaseGraph* pParent )
    {
        VisualGraph::SM::Node::Initialize( pParent );
        SetSecondaryGraph( KRG::New<EntryStateOverrideEditorGraph>() );
    }
}

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    void EntryStateOverrideEditorGraph::Initialize( VisualGraph::BaseNode* pParentNode )
    {
        FlowGraph::Initialize( pParentNode );
        CreateNode<GraphNodes::EntryStateOverrideConditionsEditorNode>();
    }

    void EntryStateOverrideEditorGraph::OnShowGraph()
    {
        auto conditionNodes = FindAllNodesOfType<GraphNodes::EntryStateOverrideConditionsEditorNode>();
        KRG_ASSERT( conditionNodes.size() == 1 );
        auto pConditionNode = conditionNodes.back();
        pConditionNode->UpdateInputPins();
    }
}

//-------------------------------------------------------------------------
// TRANSITIONS
//-------------------------------------------------------------------------

namespace KRG::Animation::GraphNodes
{
    void TransitionEditorNode::Initialize( VisualGraph::BaseGraph* pParent )
    {
        EditorGraphNode::Initialize( pParent );

        CreateInputPin( "Condition", GraphValueType::Bool );
        CreateInputPin( "Duration Override", GraphValueType::Float );
        CreateInputPin( "Sync Event Override", GraphValueType::Float );
    }

    void TransitionEditorNode::DrawInfoText( VisualGraph::DrawContext const& ctx )
    {
        switch ( m_rootMotionBlend )
        {
            case RootMotionBlendMode::Blend:
            ImGui::Text( "Blend Root Motion" );
            break;

            case RootMotionBlendMode::IgnoreSource:
            ImGui::Text( "Ignore Source Root Motion" );
            break;

            case RootMotionBlendMode::IgnoreTarget:
            ImGui::Text( "Ignore Target Root Motion" );
            break;
        }

        //-------------------------------------------------------------------------

        ImGui::Text( "Duration: %.2fs", m_duration.ToFloat() );

        if ( m_isSynchronized )
        {
            ImGui::Text( "Synchronized" );
        }
        else
        {
            if ( m_keepSourceSyncEventIdx && m_keepSourceSyncEventPercentageThrough )
            {
                ImGui::Text( "Match Sync Event and Percentage" );
            }
            else if ( m_keepSourceSyncEventIdx )
            {
                ImGui::Text( "Match Sync Event" );
            }
            else if ( m_keepSourceSyncEventPercentageThrough )
            {
                ImGui::Text( "Match Sync Percentage" );
            }
        }

        ImGui::Text( "Sync Offset: %.2f", m_syncEventOffset );

        //-------------------------------------------------------------------------

        if ( m_canBeForced )
        {
            ImGui::Text( "Forced" );
        }
    }

    //-------------------------------------------------------------------------

    void TransitionConduitEditorNode::Initialize( VisualGraph::BaseGraph* pParent )
    {
        VisualGraph::SM::TransitionConduit::Initialize( pParent );
        SetSecondaryGraph( KRG::New<FlowGraph>( GraphType::TransitionTree ) );
    }

    bool TransitionConduitEditorNode::HasTransitions() const
    {
        return !GetSecondaryGraph()->FindAllNodesOfType<TransitionEditorNode>().empty();
    }

    ImColor TransitionConduitEditorNode::GetNodeColor() const
    {
        if ( !HasTransitions() )
        {
            return VisualGraph::VisualSettings::s_connectionColorInvalid;
        }

        return VisualGraph::VisualSettings::s_connectionColorValid;
    }

    //-------------------------------------------------------------------------

    void GlobalTransitionConduitEditorNode::Initialize( VisualGraph::BaseGraph* pParent )
    {
        VisualGraph::SM::Node::Initialize( pParent );
        SetSecondaryGraph( KRG::New<GlobalTransitionsEditorGraph>() );
    }
}

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    void GlobalTransitionsEditorGraph::UpdateGlobalTransitionNodes()
    {
        auto pParentSM = Cast<StateMachineGraph>( GetParentNode()->GetParentGraph() );
        KRG_ASSERT( pParentSM != nullptr );
        auto stateNodes = pParentSM->FindAllNodesOfType<GraphNodes::StateBaseEditorNode>( VisualGraph::SearchMode::Localized, VisualGraph::SearchTypeMatch::Derived );

        //-------------------------------------------------------------------------

        auto globalTransitions = FindAllNodesOfType<GraphNodes::GlobalTransitionEditorNode>();

        //-------------------------------------------------------------------------

        TInlineVector<GraphNodes::StateBaseEditorNode*, 20> transitionsToCreate;
        TInlineVector<GraphNodes::GlobalTransitionEditorNode*, 20> transitionsToRemove;

        for ( auto const& pTransition : globalTransitions )
        {
            transitionsToRemove.emplace_back( pTransition );
        }

        // Check transition states
        //-------------------------------------------------------------------------

        auto SearchPredicate = [] ( GraphNodes::GlobalTransitionEditorNode* pNode, UUID const& ID )
        {
            if ( pNode != nullptr )
            {
                return pNode->m_stateID == ID;
            }
            else
            {
                return false;
            }
        };

        for ( auto pState : stateNodes )
        {
            int32 const stateIdx = VectorFindIndex( transitionsToRemove, pState->GetID(), SearchPredicate );
            if ( stateIdx == InvalidIndex )
            {
                transitionsToCreate.emplace_back( pState );
            }
            else // Found the transition, so update the name
            {
                transitionsToRemove[stateIdx]->m_name = pState->GetDisplayName();
                transitionsToRemove[stateIdx] = nullptr;
            }
        }

        // Remove invalid transitions
        //-------------------------------------------------------------------------

        for ( auto const& pTransition : transitionsToRemove )
        {
            if ( pTransition != nullptr )
            {
                pTransition->Destroy();
            }
        }

        // Add new transitions
        //-------------------------------------------------------------------------

        for ( auto pState : transitionsToCreate )
        {
            auto pNewTransition = CreateNode<GraphNodes::GlobalTransitionEditorNode>();
            pNewTransition->m_name = pState->GetDisplayName();
            pNewTransition->m_stateID = pState->GetID();
        }
    }
}

//-------------------------------------------------------------------------
// STATE MACHINE
//-------------------------------------------------------------------------

namespace KRG::Animation::GraphNodes
{
    void StateMachineEditorNode::Initialize( VisualGraph::BaseGraph* pParent )
    {
        EditorGraphNode::Initialize( pParent );
        CreateOutputPin( "Pose", GraphValueType::Pose );
        SetChildGraph( KRG::New<StateMachineGraph>() );
    }

    GraphNodeIndex StateMachineEditorNode::Compile( EditorGraphCompilationContext& context ) const
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
        auto stateNodes = pStateMachine->FindAllNodesOfType<StateBaseEditorNode>( VisualGraph::SearchMode::Localized, VisualGraph::SearchTypeMatch::Derived );
        int32 const numStateNodes = (int32) stateNodes.size();
        KRG_ASSERT( numStateNodes >= 1 );

        auto conduitNodes = pStateMachine->FindAllNodesOfType<TransitionConduitEditorNode>();
        int32 const numConduitNodes = (int32) conduitNodes.size();

        auto globalTransitionNodes = pStateMachine->GetGlobalTransitionConduit()->GetSecondaryGraph()->FindAllNodesOfType<GlobalTransitionEditorNode>();

        // Compile all states
        //-------------------------------------------------------------------------

        THashMap<UUID, StateMachineNode::StateIndex> IDToStateIdxMap;
        THashMap<UUID, GraphNodeIndex> IDToCompiledNodeIdxMap;

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
            auto pEntryConditionNode = pStateMachine->GetEntryConditionNodeForState( pStateNode->GetID() );
            if ( pEntryConditionNode != nullptr )
            {
                KRG_ASSERT( pEntryConditionNode->GetValueType() == GraphValueType::Bool );
                stateSettings.m_entryConditionNodeIdx = pEntryConditionNode->Compile( context );
                if ( stateSettings.m_entryConditionNodeIdx == InvalidIndex )
                {
                    return InvalidIndex;
                }
            }

            IDToStateIdxMap.insert( TPair<UUID, StateMachineNode::StateIndex>( pStateNode->GetID(), (StateMachineNode::StateIndex) i ) );
            IDToCompiledNodeIdxMap.insert( TPair<UUID, GraphNodeIndex>( pStateNode->GetID(), stateSettings.m_stateNodeIdx ) );
        }

        // Compile all transitions
        //-------------------------------------------------------------------------

        auto ConduitSearchPredicate = [] ( TransitionConduitEditorNode* pConduit, UUID const& startStateID )
        {
            return pConduit->GetStartStateID() == startStateID;
        };

        for ( auto i = 0; i < numStateNodes; i++ )
        {
            auto pStartStateNode = stateNodes[i];

            // Check all conduits for any starting at the specified state
            //-------------------------------------------------------------------------

            // We need to ignore any global transitions that we have an explicit conduit for
            TInlineVector<GlobalTransitionEditorNode const*, 20> globalTransitionNodesCopy = globalTransitionNodes;
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

            auto TryCompileTransition = [&] ( TransitionEditorNode const* pTransitionNode, UUID const& endStateID )
            {
                // Transitions are only enabled if a condition is connected to them
                auto pConditionNode = pTransitionNode->GetConnectedInputNode<EditorGraphNode>( 0 );
                if ( pConditionNode != nullptr )
                {
                    KRG_ASSERT( pConditionNode->GetValueType() == GraphValueType::Bool );

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

            // Remove ourselves state from the global transitions copy
            RemoveFromGlobalTransitions( pStartStateNode->GetID() );

            // Compile conduits
            for ( auto c = 0; c < numConduitNodes; c++ )
            {
                auto pConduit = conduitNodes[c];
                if ( pConduit->GetStartStateID() != pStartStateNode->GetID() )
                {
                    continue;
                }

                RemoveFromGlobalTransitions( pConduit->GetEndStateID() );

                auto const foundSourceStateIter = IDToCompiledNodeIdxMap.find( pConduit->GetStartStateID() );
                KRG_ASSERT( foundSourceStateIter != IDToCompiledNodeIdxMap.end() );
                context.BeginConduitCompilation( foundSourceStateIter->second );

                // Compile transitions in conduit
                auto transitionNodes = pConduit->GetSecondaryGraph()->FindAllNodesOfType<TransitionEditorNode>();
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
            // Compile all global transitions from this state to others

            for ( auto pGlobalTransition : globalTransitionNodesCopy )
            {
                auto const foundSourceStateIter = IDToCompiledNodeIdxMap.find( pStartStateNode->GetID() );
                KRG_ASSERT( foundSourceStateIter != IDToCompiledNodeIdxMap.end() );
                context.BeginConduitCompilation( foundSourceStateIter->second );

                if ( !TryCompileTransition( pGlobalTransition, pGlobalTransition->GetEndStateID() ) )
                {
                    return InvalidIndex;
                }
                context.EndConduitCompilation();
            }
        }

        //-------------------------------------------------------------------------

        pSettings->m_defaultStateIndex = IDToStateIdxMap[pStateMachine->GetDefaultEntryStateID()];

        return pSettings->m_nodeIdx;
    }

    GraphNodeIndex StateMachineEditorNode::CompileState( EditorGraphCompilationContext& context, StateBaseEditorNode const* pBaseStateNode ) const
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

        auto pBlendTreeStateNode = TryCast<StateEditorNode>( pBaseStateNode );
        if ( pBlendTreeStateNode != nullptr )
        {
            // Compile Blend Tree
            //-------------------------------------------------------------------------

            auto pBlendTreeRoot = pBlendTreeStateNode->GetBlendTreeRootNode();
            KRG_ASSERT( pBlendTreeRoot != nullptr );

            auto pBlendTreeNode = pBlendTreeRoot->GetConnectedInputNode<EditorGraphNode>( 0 );
            if ( pBlendTreeNode != nullptr )
            {
                pSettings->m_childNodeIdx = pBlendTreeNode->Compile( context );
                if ( pSettings->m_childNodeIdx == InvalidIndex )
                {
                    return InvalidIndex;
                }
            }

            // Compile Layer Data
            //-------------------------------------------------------------------------

            auto pLayerData = pBlendTreeStateNode->GetLayerDataNode();
            KRG_ASSERT( pLayerData != nullptr );

            auto pLayerWeightNode = pLayerData->GetConnectedInputNode<EditorGraphNode>( 0 );
            if ( pLayerWeightNode != nullptr )
            {
                pSettings->m_layerWeightNodeIdx = pLayerWeightNode->Compile( context );
                if ( pSettings->m_layerWeightNodeIdx == InvalidIndex )
                {
                    return InvalidIndex;
                }
            }

            auto pLayerMaskNode = pLayerData->GetConnectedInputNode<EditorGraphNode>( 1 );
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
            auto pOffState = Cast<OffStateEditorNode>( pBaseStateNode );
            pSettings->m_childNodeIdx = InvalidIndex;
            pSettings->m_isOffState = true;
        }

        //-------------------------------------------------------------------------

        return pSettings->m_nodeIdx;
    }

    GraphNodeIndex StateMachineEditorNode::CompileTransition( EditorGraphCompilationContext& context, TransitionEditorNode const* pTransitionNode, GraphNodeIndex targetStateNodeIdx ) const
    {
        KRG_ASSERT( pTransitionNode != nullptr );
        TransitionNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<TransitionNode>( pTransitionNode, pSettings );
        if ( state == NodeCompilationState::AlreadyCompiled )
        {
            return pSettings->m_nodeIdx;
        }

        //-------------------------------------------------------------------------

        auto pDurationOverrideNode = pTransitionNode->GetConnectedInputNode<EditorGraphNode>( 1 );
        if ( pDurationOverrideNode != nullptr )
        {
            KRG_ASSERT( pDurationOverrideNode->GetValueType() == GraphValueType::Float );
            pSettings->m_durationOverrideNodeIdx = pDurationOverrideNode->Compile( context );
            if ( pSettings->m_durationOverrideNodeIdx == InvalidIndex )
            {
                return InvalidIndex;
            }
        }

        auto pSyncEventOffsetOverrideNode = pTransitionNode->GetConnectedInputNode<EditorGraphNode>( 2 );
        if ( pSyncEventOffsetOverrideNode != nullptr )
        {
            KRG_ASSERT( pSyncEventOffsetOverrideNode->GetValueType() == GraphValueType::Float );
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

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    using namespace KRG::Animation::GraphNodes;

    //-------------------------------------------------------------------------

    void StateMachineGraph::Initialize( VisualGraph::BaseNode* pParentNode )
    {
        VisualGraph::ScopedNodeModification sgm( pParentNode );
        VisualGraph::StateMachineGraph::Initialize( pParentNode );

        //-------------------------------------------------------------------------

        m_pEntryOverridesConduit = KRG::New<EntryStateOverrideConduitEditorNode>();
        m_pEntryOverridesConduit->Initialize( this );
        m_pEntryOverridesConduit->SetCanvasPosition( ImVec2( 50, 50 ) );
        AddNode( m_pEntryOverridesConduit );

        m_pGlobalTransitionConduit = KRG::New<GlobalTransitionConduitEditorNode>();
        m_pGlobalTransitionConduit->Initialize( this );
        m_pGlobalTransitionConduit->SetCanvasPosition( ImVec2( 100, 50 ) );
        AddNode( m_pGlobalTransitionConduit );

        auto pDefaultStateNode = KRG::New<StateEditorNode>();
        pDefaultStateNode->Initialize( this );
        pDefaultStateNode->SetCanvasPosition( ImVec2( 0, 150 ) );
        AddNode( pDefaultStateNode );

        m_entryStateID = pDefaultStateNode->GetID();

        //-------------------------------------------------------------------------

        UpdateDependentNodes();
        UpdateEntryState();
    }

    void StateMachineGraph::CreateNewState( ImVec2 const& mouseCanvasPos )
    {
        VisualGraph::ScopedGraphModification sgm( this );

        auto pStateNode = KRG::New<StateEditorNode>();
        pStateNode->Initialize( this );
        pStateNode->SetCanvasPosition( mouseCanvasPos );
        AddNode( pStateNode );
        UpdateDependentNodes();
    }

    void StateMachineGraph::CreateNewOffState( ImVec2 const& mouseCanvasPos )
    {
        VisualGraph::ScopedGraphModification sgm( this );

        auto pStateNode = KRG::New<OffStateEditorNode>();
        pStateNode->Initialize( this );
        pStateNode->SetCanvasPosition( mouseCanvasPos );
        AddNode( pStateNode );
        UpdateDependentNodes();
    }

    VisualGraph::SM::TransitionConduit* StateMachineGraph::CreateTransitionNode() const
    {
        return KRG::New<TransitionConduitEditorNode>();
    }

    bool StateMachineGraph::CanDeleteNode( VisualGraph::BaseNode const* pNode ) const
    {
        auto pStateNode = TryCast<StateBaseEditorNode>( pNode );
        if ( pStateNode != nullptr )
        {
            auto const stateNodes = FindAllNodesOfType<StateBaseEditorNode>( VisualGraph::SearchMode::Localized, VisualGraph::SearchTypeMatch::Derived );
            return stateNodes.size() > 1;
        }

        return VisualGraph::StateMachineGraph::CanDeleteNode( pNode );
    }

    void StateMachineGraph::UpdateDependentNodes()
    {
        auto conditionNodes = m_pEntryOverridesConduit->GetSecondaryGraph()->FindAllNodesOfType<EntryStateOverrideConditionsEditorNode>();
        KRG_ASSERT( conditionNodes.size() == 1 );
        auto pConditionNode = conditionNodes.back();
        pConditionNode->UpdateInputPins();

        //-------------------------------------------------------------------------

        auto pGlobalTransitionsGraph = Cast<GlobalTransitionsEditorGraph>( m_pGlobalTransitionConduit->GetSecondaryGraph() );
        KRG_ASSERT( pGlobalTransitionsGraph != nullptr );
        pGlobalTransitionsGraph->UpdateGlobalTransitionNodes();
    }

    bool StateMachineGraph::HasGlobalTransitionForState( UUID const& stateID ) const
    {
        auto pGlobalTransitionsGraph = m_pGlobalTransitionConduit->GetSecondaryGraph();
        auto globalTransitions = pGlobalTransitionsGraph->FindAllNodesOfType<GlobalTransitionEditorNode>();
        for ( auto pGlobalTransition : globalTransitions )
        {
            if ( pGlobalTransition->m_stateID == stateID )
            {
                return pGlobalTransition->GetConnectedInputNode( 0 ) != nullptr;
            }
        }

        // Dont call this function with an invalid state ID
        KRG_UNREACHABLE_CODE();
        return false;
    }

    GraphNodes::EditorGraphNode const* StateMachineGraph::GetEntryConditionNodeForState( UUID const& stateID ) const
    {
        auto conditionNodes = m_pEntryOverridesConduit->GetSecondaryGraph()->FindAllNodesOfType<EntryStateOverrideConditionsEditorNode>();
        KRG_ASSERT( conditionNodes.size() == 1 );
        auto pConditionNode = conditionNodes.back();

        //-------------------------------------------------------------------------

        int32 const pinIdx = VectorFindIndex( pConditionNode->m_pinToStateMapping, stateID );
        KRG_ASSERT( pinIdx != InvalidIndex );
        return TryCast<EditorGraphNode const>( pConditionNode->GetConnectedInputNode( pinIdx ) );
    }

    UUID StateMachineGraph::RegenerateIDs( THashMap<UUID, UUID>& IDMapping )
    {
        UUID const originalID = VisualGraph::StateMachineGraph::RegenerateIDs( IDMapping );

        // Update entry state IDs
        //-------------------------------------------------------------------------

        auto conditionNodes = m_pEntryOverridesConduit->GetSecondaryGraph()->FindAllNodesOfType<EntryStateOverrideConditionsEditorNode>();
        KRG_ASSERT( conditionNodes.size() == 1 );
        auto pEntryStateConditionsNode = conditionNodes.back();

        for ( auto& stateID : pEntryStateConditionsNode->m_pinToStateMapping )
        {
            stateID = IDMapping.at( stateID );
        }

        // Update global transition IDs
        //-------------------------------------------------------------------------

        auto pGlobalTransitionsGraph = m_pGlobalTransitionConduit->GetSecondaryGraph();
        auto globalTransitions = pGlobalTransitionsGraph->FindAllNodesOfType<GlobalTransitionEditorNode>();

        for ( auto pNode : globalTransitions )
        {
            pNode->m_stateID = IDMapping.at( pNode->m_stateID );
        }

        return originalID;
    }

    void StateMachineGraph::SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& graphObjectValue )
    {
        VisualGraph::StateMachineGraph::SerializeCustom( typeRegistry, graphObjectValue );

        auto const entryOverrideNodes = FindAllNodesOfType<EntryStateOverrideConduitEditorNode>();
        KRG_ASSERT( entryOverrideNodes.size() == 1 );
        m_pEntryOverridesConduit = entryOverrideNodes.back();

        auto const globalTransitionNodes = FindAllNodesOfType<GlobalTransitionConduitEditorNode>();
        KRG_ASSERT( globalTransitionNodes.size() == 1 );
        m_pGlobalTransitionConduit = globalTransitionNodes.back();

        UpdateDependentNodes();
    }

    void StateMachineGraph::PostPasteNodes( TInlineVector<VisualGraph::BaseNode*, 20> const& pastedNodes )
    {
        VisualGraph::StateMachineGraph::PostPasteNodes( pastedNodes );
        ParameterReferenceEditorNode::RefreshParameterReferences( GetRootGraph() );
        UpdateDependentNodes();
    }
}