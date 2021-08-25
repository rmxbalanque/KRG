#include "AnimationToolsNode_EntryStateOverrides.h"
#include "AnimationToolsNode_State.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void EntryStateOverrideConditionsNode::Update( TInlineVector<StateBaseToolsNode*, 20> const& states )
    {
        int32 const numOriginalInputPins = GetNumInputPins();

        TInlineVector<StateBaseToolsNode*, 20> pinsToCreate;
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
                pinsToRemove[pinIdx].Reset();
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
            CreateInputPin( pState->GetDisplayName(), NodeValueType::Bool );
            m_pinToStateMapping.emplace_back( pState->GetID() );
        }
    }

    //-------------------------------------------------------------------------

    EntryStateOverrideGraph::EntryStateOverrideGraph()
        : FlowToolGraph( GraphType::ValueTree )
    {}

    void EntryStateOverrideGraph::Initialize( GraphEditor::BaseNode* pParentNode )
    {
        FlowToolGraph::Initialize( pParentNode );
        CreateNode<EntryStateOverrideConditionsNode>();
    }

    void EntryStateOverrideGraph::Update( TInlineVector<StateBaseToolsNode*, 20> const& states )
    {
        auto conditionNodes = FindAllNodesOfType<EntryStateOverrideConditionsNode>();
        KRG_ASSERT( conditionNodes.size() == 1 );
        auto pConditionNode = conditionNodes.back();
        pConditionNode->Update( states );
    }

    //-------------------------------------------------------------------------

    void EntryStateOverrideNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        GraphEditor::SM::Node::Initialize( pParent );
        SetSecondaryGraph( KRG::New<EntryStateOverrideGraph>() );
    }

    FlowToolsNode const* EntryStateOverrideNode::GetEntryConditionNodeForState( UUID const& stateID ) const
    {
        auto conditionNodes = GetSecondaryGraph()->FindAllNodesOfType<EntryStateOverrideConditionsNode>();
        KRG_ASSERT( conditionNodes.size() == 1 );
        auto pConditionNode = conditionNodes.back();

        //-------------------------------------------------------------------------

        int32 const pinIdx = VectorFindIndex( pConditionNode->m_pinToStateMapping, stateID );
        KRG_ASSERT( pinIdx != InvalidIndex );
        return TryCast<FlowToolsNode const>( pConditionNode->GetConnectedInputNode( pinIdx ) );
    }
}