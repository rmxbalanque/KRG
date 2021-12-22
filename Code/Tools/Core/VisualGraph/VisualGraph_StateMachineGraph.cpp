#include "VisualGraph_StateMachineGraph.h"

//-------------------------------------------------------------------------

namespace KRG::VisualGraph
{
    void StateMachineGraph::SetDefaultEntryState( UUID const& newDefaultEntryStateID )
    {
        KRG_ASSERT( newDefaultEntryStateID.IsValid() && FindNode( newDefaultEntryStateID ) != nullptr );
        ScopedGraphModification sgm( this );
        m_entryStateID = newDefaultEntryStateID;
    }

    bool StateMachineGraph::DoesTransitionConduitExist( SM::State const* pStartState, SM::State const* pEndState ) const
    {
        KRG_ASSERT( pStartState != nullptr && pEndState != nullptr );
        KRG_ASSERT( pStartState != pEndState );

        auto const conduits = FindAllNodesOfType<SM::TransitionConduit>( SearchMode::Localized, SearchTypeMatch::Derived );
        for ( auto pConduit : conduits )
        {
            if ( pConduit->m_startStateID == pStartState->m_ID && pConduit->m_endStateID == pEndState->m_ID )
            {
                return true;
            }
        }

        return false;
    }

    bool StateMachineGraph::CanCreateTransitionConduit( SM::State const* pStartState, SM::State const* pEndState ) const
    {
        KRG_ASSERT( pStartState != nullptr && pEndState != nullptr );

        if ( pStartState == pEndState )
        {
            return false;
        }

        if ( DoesTransitionConduitExist( pStartState, pEndState ) )
        {
            return false;
        }

        return true;
    }

    void StateMachineGraph::CreateTransitionConduit( SM::State const* pStartState, SM::State const* pEndState )
    {
        KRG_ASSERT( CanCreateTransitionConduit( pStartState, pEndState ) );

        ScopedGraphModification sgm( this );
        auto pTransitionNode = CreateTransitionNode();
        pTransitionNode->Initialize( this );
        pTransitionNode->m_startStateID = pStartState->m_ID;
        pTransitionNode->m_endStateID = pEndState->m_ID;
        AddNode( pTransitionNode );
    }

    void StateMachineGraph::PreDestroyNode( BaseNode* pNodeAboutToBeDestroyed )
    {
        auto pStateNode = TryCast<SM::State>( pNodeAboutToBeDestroyed );
        if ( pStateNode != nullptr )
        {
            auto const transitions = FindAllNodesOfType<SM::TransitionConduit>();
            for ( auto pTransition : transitions )
            {
                if ( pTransition->m_startStateID == pStateNode->m_ID || pTransition->m_endStateID == pStateNode->m_ID )
                {
                    DestroyNode( pTransition->GetID() );
                }
            }
        }

        BaseGraph::PreDestroyNode( pNodeAboutToBeDestroyed );
    }

    void StateMachineGraph::PostDestroyNode( UUID const& nodeID )
    {
        if ( m_entryStateID == nodeID )
        {
            UpdateEntryState();
        }

        BaseGraph::PostDestroyNode( nodeID );
    }

    void StateMachineGraph::SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& graphObjectValue )
    {
        BaseGraph::SerializeCustom( typeRegistry, graphObjectValue );

        // Ensure that the entry state is valid
        if ( FindNode( m_entryStateID ) == nullptr )
        {
            UpdateEntryState();
        }
    }

    void StateMachineGraph::UpdateEntryState()
    {
        m_entryStateID.Clear();

        // Set to the first state we find
        for ( auto pNode : m_nodes )
        {
            if ( IsOfType<SM::State>( pNode ) )
            {
                m_entryStateID = pNode->GetID();
                break;
            }
        }
    }

    UUID StateMachineGraph::RegenerateIDs( THashMap<UUID, UUID>& IDMapping )
    {
        UUID const originalID = BaseGraph::RegenerateIDs( IDMapping );

        // Update conduit UUIDs
        //-------------------------------------------------------------------------

        for ( auto pNode : m_nodes )
        {
            if ( auto pConduitNode = TryCast<SM::TransitionConduit>( pNode ) )
            {
                pConduitNode->m_startStateID = IDMapping.at( pConduitNode->m_startStateID );
                pConduitNode->m_endStateID = IDMapping.at( pConduitNode->m_endStateID );
            }
        }

        return originalID;
    }
}