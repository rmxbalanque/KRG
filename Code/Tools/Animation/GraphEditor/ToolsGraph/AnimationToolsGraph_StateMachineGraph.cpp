#include "AnimationToolsGraph_StateMachineGraph.h"
#include "Nodes/AnimationToolsNode_EntryStateOverrides.h"
#include "Nodes/AnimationToolsNode_Transitions.h"
#include "Nodes/AnimationToolsNode_State.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void StateMachineGraph::Initialize( GraphEditor::BaseNode* pParentNode )
    {
        GraphEditor::StateMachineGraph::Initialize( pParentNode );

        //-------------------------------------------------------------------------

        m_pEntryOverridesNode = KRG::New<Tools_EntryStateOverrideNode>();
        m_pEntryOverridesNode->Initialize( this );
        m_pEntryOverridesNode->SetCanvasPosition( ImVec2( 50, 50 ) );
        AddNode( m_pEntryOverridesNode );

        m_pGlobalTransitionsNode = KRG::New<Tools_GlobalTransitionsNode>();
        m_pGlobalTransitionsNode->Initialize( this );
        m_pGlobalTransitionsNode->SetCanvasPosition( ImVec2( 100, 50 ) );
        AddNode( m_pGlobalTransitionsNode );

        auto pDefaultStateNode = KRG::New<Tools_StateNode>();
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
        auto pStateNode = KRG::New<Tools_StateNode>();
        pStateNode->Initialize( this );
        pStateNode->SetCanvasPosition( mouseCanvasPos );
        AddNode( pStateNode );

        //-------------------------------------------------------------------------

        UpdateDependentNodes();
    }

    void StateMachineGraph::CreateNewOffState( ImVec2 const& mouseCanvasPos )
    {
        auto pStateNode = KRG::New<Tools_OffStateNode>();
        pStateNode->Initialize( this );
        pStateNode->SetCanvasPosition( mouseCanvasPos );
        AddNode( pStateNode );

        //-------------------------------------------------------------------------

        UpdateDependentNodes();
    }

    GraphEditor::SM::TransitionConduit* StateMachineGraph::CreateTransitionNode() const
    {
        return KRG::New<Tools_TransitionConduitNode>();
    }

    //-------------------------------------------------------------------------

    bool StateMachineGraph::CanDeleteNode( GraphEditor::BaseNode const* pNode ) const
    {
        auto pStateNode = TryCast<Tools_StateBaseNode>( pNode );
        if ( pStateNode != nullptr )
        {
            auto const stateNodes = FindAllNodesOfType<Tools_StateBaseNode>( GraphEditor::SearchMode::Localized, GraphEditor::SearchTypeMatch::Derived );
            return stateNodes.size() > 1;
        }

        return GraphEditor::StateMachineGraph::CanDeleteNode( pNode );
    }

    void StateMachineGraph::UpdateDependentNodes()
    {
        m_pEntryOverridesNode->UpdateEntryStateNodeInputPins();
        m_pGlobalTransitionsNode->UpdateGlobalTransitionNodes();
    }

    void StateMachineGraph::SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& graphObjectValue )
    {
        GraphEditor::StateMachineGraph::SerializeCustom( typeRegistry, graphObjectValue );

        auto const entryOverrideNodes = FindAllNodesOfType<Tools_EntryStateOverrideNode>();
        KRG_ASSERT( entryOverrideNodes.size() == 1 );
        m_pEntryOverridesNode = entryOverrideNodes.back();

        auto const globalTransitionNodes = FindAllNodesOfType<Tools_GlobalTransitionsNode>();
        KRG_ASSERT( globalTransitionNodes.size() == 1 );
        m_pGlobalTransitionsNode = globalTransitionNodes.back();
    }
}