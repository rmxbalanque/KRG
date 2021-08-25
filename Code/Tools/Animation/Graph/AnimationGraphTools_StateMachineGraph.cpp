#include "AnimationGraphTools_StateMachineGraph.h"
#include "StateMachine/AnimationToolsNode_EntryStateOverrides.h"
#include "StateMachine/AnimationToolsNode_Transitions.h"
#include "StateMachine/AnimationToolsNode_State.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void StateMachineToolsGraph::Initialize( GraphEditor::BaseNode* pParentNode )
    {
        GraphEditor::StateMachineGraph::Initialize( pParentNode );

        //-------------------------------------------------------------------------

        m_pEntryOverridesNode = KRG::New<EntryStateOverrideNode>();
        m_pEntryOverridesNode->Initialize( this );
        m_pEntryOverridesNode->SetCanvasPosition( ImVec2( 50, 50 ) );
        AddNode( m_pEntryOverridesNode );

        m_pGlobalTransitionsNode = KRG::New<GlobalTransitionsToolsNode>();
        m_pGlobalTransitionsNode->Initialize( this );
        m_pGlobalTransitionsNode->SetCanvasPosition( ImVec2( 100, 50 ) );
        AddNode( m_pGlobalTransitionsNode );

        auto pDefaultStateNode = KRG::New<StateToolsNode>();
        pDefaultStateNode->Initialize( this );
        pDefaultStateNode->SetCanvasPosition( ImVec2( 0, 150 ) );
        AddNode( pDefaultStateNode );

        m_entryStateID = pDefaultStateNode->GetID();

        //-------------------------------------------------------------------------

        UpdateDependentNodes();
    }

    void StateMachineToolsGraph::CreateNewState( ImVec2 const& mouseCanvasPos )
    {
        auto pStateNode = KRG::New<StateToolsNode>();
        pStateNode->Initialize( this );
        pStateNode->SetCanvasPosition( mouseCanvasPos );
        AddNode( pStateNode );

        //-------------------------------------------------------------------------

        UpdateDependentNodes();
    }

    void StateMachineToolsGraph::CreateNewOffState( ImVec2 const& mouseCanvasPos )
    {
        auto pStateNode = KRG::New<OffStateToolsNode>();
        pStateNode->Initialize( this );
        pStateNode->SetCanvasPosition( mouseCanvasPos );
        AddNode( pStateNode );

        //-------------------------------------------------------------------------

        UpdateDependentNodes();
    }

    GraphEditor::SM::TransitionConduit* StateMachineToolsGraph::CreateTransitionNode() const
    {
        return KRG::New<TransitionConduitToolsNode>();
    }

    //-------------------------------------------------------------------------

    bool StateMachineToolsGraph::CanDeleteNode( GraphEditor::BaseNode const* pNode ) const
    {
        auto pStateNode = TryCast<StateBaseToolsNode>( pNode );
        if ( pStateNode != nullptr )
        {
            auto const stateNodes = FindAllNodesOfType<StateBaseToolsNode>( GraphEditor::SearchMode::Localized, GraphEditor::SearchTypeMatch::Derived );
            return stateNodes.size() > 1;
        }

        return GraphEditor::StateMachineGraph::CanDeleteNode( pNode );
    }

    void StateMachineToolsGraph::UpdateDependentNodes()
    {
        auto const stateNodes = FindAllNodesOfType<StateBaseToolsNode>( GraphEditor::SearchMode::Localized, GraphEditor::SearchTypeMatch::Derived );
        m_pEntryOverridesNode->Update( stateNodes );
        m_pGlobalTransitionsNode->Update( stateNodes );
        UpdateEntryState();
    }

    void StateMachineToolsGraph::PostDestroyNode( UUID const& nodeID )
    {
        UpdateDependentNodes();
        GraphEditor::StateMachineGraph::PostDestroyNode( nodeID );
    }

    void StateMachineToolsGraph::SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& graphObjectValue )
    {
        GraphEditor::StateMachineGraph::SerializeCustom( typeRegistry, graphObjectValue );

        auto const entryOverrideNodes = FindAllNodesOfType<EntryStateOverrideNode>();
        KRG_ASSERT( entryOverrideNodes.size() == 1 );
        m_pEntryOverridesNode = entryOverrideNodes.back();

        auto const globalTransitionNodes = FindAllNodesOfType<GlobalTransitionsToolsNode>();
        KRG_ASSERT( globalTransitionNodes.size() == 1 );
        m_pGlobalTransitionsNode = globalTransitionNodes.back();
    }
}