#include "AnimationToolsNode_Transitions.h"
#include "AnimationToolsNode_State.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void TransitionToolsNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        FlowToolsNode::Initialize( pParent );

        CreateInputPin( "Condition", NodeValueType::Bool );
        CreateInputPin( "Duration Override", NodeValueType::Float );
        CreateInputPin( "Sync Event Override", NodeValueType::Float );
    }

    //-------------------------------------------------------------------------

    void TransitionConduitToolsNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        GraphEditor::SM::TransitionConduit::Initialize( pParent );
        SetSecondaryGraph( KRG::New<FlowToolGraph>( GraphType::TransitionTree ) );
    }

    //-------------------------------------------------------------------------

    void GlobalTransitionsToolsGraph::Update( TInlineVector<StateToolsNode*, 20> const& states )
    {
        auto globalTransitions = FindAllNodesOfType<GlobalTransitionToolsNode>();

        //-------------------------------------------------------------------------

        TInlineVector<StateToolsNode*, 20> transitionsToCreate;
        TInlineVector<GlobalTransitionToolsNode*, 20> transitionsToRemove;

        for ( auto const& pTransition : globalTransitions )
        {
            transitionsToRemove.emplace_back( pTransition );
        }

        // Check transition states
        //-------------------------------------------------------------------------

        auto SearchPredicate = [] ( GlobalTransitionToolsNode* pNode, UUID const& ID ) 
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

        for ( auto pState : states )
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
            auto pNewTransition = CreateNode<GlobalTransitionToolsNode>();
            pNewTransition->m_name = pState->GetDisplayName();
            pNewTransition->m_stateID = pState->GetID();
        }
    }

    //-------------------------------------------------------------------------

    void GlobalTransitionsToolsNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        GraphEditor::SM::Node::Initialize( pParent );
        SetSecondaryGraph( KRG::New<GlobalTransitionsToolsGraph>() );
    }
}