#include "AnimationToolsNode_Transitions.h"
#include "AnimationToolsNode_State.h"
#include "Tools/Animation/GraphEditor/ToolsGraph/AnimationToolsGraph_StateMachineGraph.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void Tools_TransitionNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        Tools_GraphNode::Initialize( pParent );

        CreateInputPin( "Condition", ValueType::Bool );
        CreateInputPin( "Duration Override", ValueType::Float );
        CreateInputPin( "Sync Event Override", ValueType::Float );
    }

    //-------------------------------------------------------------------------

    void Tools_TransitionConduitNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        GraphEditor::SM::TransitionConduit::Initialize( pParent );
        SetSecondaryGraph( KRG::New<FlowGraph>( GraphType::TransitionTree ) );
    }

    bool Tools_TransitionConduitNode::HasTransitions() const
    {
        return !GetSecondaryGraph()->FindAllNodesOfType<Tools_TransitionNode>().empty();
    }

    ImColor Tools_TransitionConduitNode::GetNodeColor() const
    {
        if ( !HasTransitions() )
        {
            return GraphEditor::VisualSettings::s_connectionColorInvalid;
        }

        return GraphEditor::VisualSettings::s_connectionColorValid;
    }

    //-------------------------------------------------------------------------

    void GlobalTransitionsToolsGraph::UpdateGlobalTransitionNodes()
    {
        auto pParentSM = Cast<StateMachineGraph>( GetParentNode()->GetParentGraph() );
        KRG_ASSERT( pParentSM != nullptr );
        auto stateNodes = pParentSM->FindAllNodesOfType<Tools_StateBaseNode>( GraphEditor::SearchMode::Localized, GraphEditor::SearchTypeMatch::Derived );

        //-------------------------------------------------------------------------

        auto globalTransitions = FindAllNodesOfType<Tools_GlobalTransitionNode>();

        //-------------------------------------------------------------------------

        TInlineVector<Tools_StateBaseNode*, 20> transitionsToCreate;
        TInlineVector<Tools_GlobalTransitionNode*, 20> transitionsToRemove;

        for ( auto const& pTransition : globalTransitions )
        {
            transitionsToRemove.emplace_back( pTransition );
        }

        // Check transition states
        //-------------------------------------------------------------------------

        auto SearchPredicate = [] ( Tools_GlobalTransitionNode* pNode, UUID const& ID ) 
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
            auto pNewTransition = CreateNode<Tools_GlobalTransitionNode>();
            pNewTransition->m_name = pState->GetDisplayName();
            pNewTransition->m_stateID = pState->GetID();
        }
    }

    bool GlobalTransitionsToolsGraph::HasGlobalTransitionForState( UUID const& stateID ) const
    {
        auto globalTransitions = FindAllNodesOfType<Tools_GlobalTransitionNode>();
        for ( auto pGlobalTransition : globalTransitions )
        {
            if ( pGlobalTransition->m_stateID == stateID )
            {
                return pGlobalTransition->GetConnectedInputNode( 0 ) != nullptr;
            }
        }

        KRG_UNREACHABLE_CODE();
        return false;
    }

    //-------------------------------------------------------------------------

    void Tools_GlobalTransitionsNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        GraphEditor::SM::Node::Initialize( pParent );
        SetSecondaryGraph( KRG::New<GlobalTransitionsToolsGraph>() );
    }

    void Tools_GlobalTransitionsNode::UpdateGlobalTransitionNodes()
    {
        auto pGlobalTransitionsGraph = Cast<GlobalTransitionsToolsGraph>( GetSecondaryGraph() );
        KRG_ASSERT( pGlobalTransitionsGraph != nullptr );
        pGlobalTransitionsGraph->UpdateGlobalTransitionNodes();
    }
}