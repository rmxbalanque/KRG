#include "AnimationToolsNode_EntryStateOverrides.h"
#include "AnimationToolsNode_State.h"
#include "Tools/Animation/GraphEditor/ToolsGraph/AnimationToolsGraph_StateMachineGraph.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void Tools_EntryStateOverrideConditionsNode::UpdateInputPins( TInlineVector<Tools_StateBaseNode*, 20> const& states )
    {
        int32 const numOriginalInputPins = GetNumInputPins();

        TInlineVector<Tools_StateBaseNode*, 20> pinsToCreate;
        TInlineVector<UUID, 20> pinsToRemove;

        for ( auto const& pin : GetInputPins() )
        {
            pinsToRemove.emplace_back( pin.m_ID );
        }

        // Check which pins are invalid
        //-------------------------------------------------------------------------

        for ( auto pState : states )
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
            CreateInputPin( pState->GetDisplayName(), ValueType::Bool );
            m_pinToStateMapping.emplace_back( pState->GetID() );
        }
    }

    //-------------------------------------------------------------------------

    Tools_EntryStateOverrideGraph::Tools_EntryStateOverrideGraph()
        : FlowGraph( GraphType::ValueTree )
    {}

    void Tools_EntryStateOverrideGraph::Initialize( GraphEditor::BaseNode* pParentNode )
    {
        FlowGraph::Initialize( pParentNode );
        CreateNode<Tools_EntryStateOverrideConditionsNode>();
    }

    void Tools_EntryStateOverrideGraph::OnShowGraph()
    {
        auto pParentSM = Cast<StateMachineGraph>( GetParentNode()->GetParentGraph() );
        KRG_ASSERT( pParentSM != nullptr );
        auto stateNodes = pParentSM->FindAllNodesOfType<Tools_StateBaseNode>( GraphEditor::SearchMode::Localized, GraphEditor::SearchTypeMatch::Derived );

        auto conditionNodes = FindAllNodesOfType<Tools_EntryStateOverrideConditionsNode>();
        KRG_ASSERT( conditionNodes.size() == 1 );
        auto pConditionNode = conditionNodes.back();
        pConditionNode->UpdateInputPins( stateNodes );
    }

    //-------------------------------------------------------------------------

    void Tools_EntryStateOverrideNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        GraphEditor::SM::Node::Initialize( pParent );
        SetSecondaryGraph( KRG::New<Tools_EntryStateOverrideGraph>() );
    }

    Tools_GraphNode const* Tools_EntryStateOverrideNode::GetEntryConditionNodeForState( UUID const& stateID ) const
    {
        auto conditionNodes = GetSecondaryGraph()->FindAllNodesOfType<Tools_EntryStateOverrideConditionsNode>();
        KRG_ASSERT( conditionNodes.size() == 1 );
        auto pConditionNode = conditionNodes.back();

        //-------------------------------------------------------------------------

        int32 const pinIdx = VectorFindIndex( pConditionNode->m_pinToStateMapping, stateID );
        KRG_ASSERT( pinIdx != InvalidIndex );
        return TryCast<Tools_GraphNode const>( pConditionNode->GetConnectedInputNode( pinIdx ) );
    }

    void Tools_EntryStateOverrideNode::UpdateEntryStateNodeInputPins()
    {
        auto pParentSM = Cast<StateMachineGraph>( GetParentGraph() );
        KRG_ASSERT( pParentSM != nullptr );
        auto stateNodes = pParentSM->FindAllNodesOfType<Tools_StateBaseNode>( GraphEditor::SearchMode::Localized, GraphEditor::SearchTypeMatch::Derived );

        auto conditionNodes = GetSecondaryGraph()->FindAllNodesOfType<Tools_EntryStateOverrideConditionsNode>();
        KRG_ASSERT( conditionNodes.size() == 1 );
        auto pConditionNode = conditionNodes.back();
        pConditionNode->UpdateInputPins( stateNodes );
    }
}