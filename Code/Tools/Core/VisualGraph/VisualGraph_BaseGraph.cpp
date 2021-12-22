#include "VisualGraph_BaseGraph.h"
#include "System/TypeSystem/TypeSerialization.h"

//-------------------------------------------------------------------------

namespace KRG::VisualGraph
{
    static void TraverseHierarchy( BaseNode const* pNode, TVector<BaseNode const*>& nodePath )
    {
        KRG_ASSERT( pNode != nullptr );
        nodePath.emplace_back( pNode );

        if ( pNode->HasParentGraph() && !pNode->GetParentGraph()->IsRootGraph() )
        {
            TraverseHierarchy( pNode->GetParentGraph()->GetParentNode(), nodePath );
        }
    }

    //-------------------------------------------------------------------------

    BaseNode::~BaseNode()
    {
        KRG_ASSERT( m_pParentGraph == nullptr );
        KRG_ASSERT( m_pChildGraph == nullptr );
        KRG_ASSERT( m_pSecondaryGraph == nullptr );
        KRG_ASSERT( !m_ID.IsValid() );
    }

    void BaseNode::Initialize( BaseGraph* pParentGraph )
    {
        m_ID = UUID::GenerateID();
        m_pParentGraph = pParentGraph;
    }

    void BaseNode::Shutdown()
    {
        if ( m_pChildGraph != nullptr )
        {
            m_pChildGraph->Shutdown();
            KRG::Delete( m_pChildGraph );
        }

        if ( m_pSecondaryGraph != nullptr )
        {
            m_pSecondaryGraph->Shutdown();
            KRG::Delete( m_pSecondaryGraph );
        }

        m_pParentGraph = nullptr;
        m_ID.Clear();
    }

    BaseNode* BaseNode::CreateNodeFromSerializedData( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& nodeObjectValue, BaseGraph* pParentGraph )
    {
        auto const& typeDataObjectValue = nodeObjectValue[s_typeDataKey];
        KRG_ASSERT( typeDataObjectValue.IsObject() );

        auto const& typeIDValue = typeDataObjectValue[TypeSystem::Serialization::s_typeIDKey];
        KRG_ASSERT( typeIDValue.IsString() );

        TypeSystem::TypeID const nodeTypeID = typeIDValue.GetString();
        auto pTypeInfo = typeRegistry.GetTypeInfo( nodeTypeID );
        KRG_ASSERT( pTypeInfo != nullptr );

        BaseNode* pNode = TypeSystem::Serialization::CreateAndReadNativeType<BaseNode>( typeRegistry, nodeObjectValue[s_typeDataKey] );
        pNode->m_pParentGraph = pParentGraph;
        pNode->Serialize( typeRegistry, nodeObjectValue );
        return pNode;
    }

    void BaseNode::Destroy()
    {
         KRG_ASSERT( HasParentGraph() );
         GetParentGraph()->DestroyNode( m_ID );
    }

    String BaseNode::GetPathFromRoot() const
    {
        TVector<BaseNode const*> path;
        TraverseHierarchy( this, path );

        //-------------------------------------------------------------------------

        String pathString;
        for ( auto iter = path.rbegin(); iter != path.rend(); ++iter )
        {
            pathString += ( *iter )->GetDisplayName();
            if ( iter != ( path.rend() - 1 ) )
            {
                pathString += "/";
            }
        }

        return pathString;
    }

    void BaseNode::Serialize( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& nodeObjectValue )
    {
        KRG_ASSERT( nodeObjectValue.IsObject() );

        //-------------------------------------------------------------------------

        // Note serialization is not symmetric for the nodes, since the node creation also deserializes registered values

        //-------------------------------------------------------------------------

        SerializeCustom( typeRegistry, nodeObjectValue );

        //-------------------------------------------------------------------------

        KRG::Delete( m_pChildGraph );

        auto const childGraphValueIter = nodeObjectValue.FindMember( s_childGraphKey );
        if ( childGraphValueIter != nodeObjectValue.MemberEnd() )
        {
            KRG_ASSERT( childGraphValueIter->value.IsObject() );
            auto& graphObjectValue = childGraphValueIter->value;
            m_pChildGraph = BaseGraph::CreateGraphFromSerializedData( typeRegistry, graphObjectValue, this );
        }

        //-------------------------------------------------------------------------

        KRG::Delete( m_pSecondaryGraph );

        auto const secondaryGraphValueIter = nodeObjectValue.FindMember( s_secondaryChildGraphKey );
        if ( secondaryGraphValueIter != nodeObjectValue.MemberEnd() )
        {
            KRG_ASSERT( secondaryGraphValueIter->value.IsObject() );
            auto& graphObjectValue = secondaryGraphValueIter->value;
            m_pSecondaryGraph = BaseGraph::CreateGraphFromSerializedData( typeRegistry, graphObjectValue, this );
        }
    }

    void BaseNode::Serialize( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonWriter& writer ) const
    {
        writer.StartObject();

        writer.Key( s_typeDataKey );
        TypeSystem::Serialization::WriteNativeType( typeRegistry, this, writer );

        //-------------------------------------------------------------------------

        SerializeCustom( typeRegistry, writer );

        //-------------------------------------------------------------------------

        if ( HasChildGraph() )
        {
            writer.Key( s_childGraphKey );
            GetChildGraph()->Serialize( typeRegistry, writer );
        }

        //-------------------------------------------------------------------------

        if ( HasSecondaryGraph() )
        {
            writer.Key( s_secondaryChildGraphKey );
            GetSecondaryGraph()->Serialize( typeRegistry, writer );
        }

        writer.EndObject();
    }

    void BaseNode::SetSecondaryGraph( BaseGraph* pGraph )
    {
        KRG_ASSERT( pGraph != nullptr && m_pSecondaryGraph == nullptr );
        pGraph->Initialize( this );
        m_pSecondaryGraph = pGraph;
    }

    void BaseNode::SetChildGraph( BaseGraph* pGraph )
    {
        KRG_ASSERT( pGraph != nullptr && m_pChildGraph == nullptr );
        pGraph->Initialize( this );
        m_pChildGraph = pGraph;
    }

    void BaseNode::BeginModification()
    {
        // Gets forwarded to the parent graph since all nodes have parent graph
        m_pParentGraph->BeginModification();
    }

    void BaseNode::EndModification()
    {
        // Gets forwarded to the parent graph since all nodes have parent graph
        m_pParentGraph->EndModification();
    }

    void BaseNode::SetCanvasPosition( ImVec2 const& newPosition )
    {
        BeginModification();
        m_canvasPosition = newPosition;
        EndModification();
    }

    UUID BaseNode::RegenerateIDs( THashMap<UUID, UUID>& IDMapping )
    {
        UUID const originalID = m_ID;
        m_ID = UUID::GenerateID();

        KRG_ASSERT( IDMapping.find( originalID ) == IDMapping.end() );
        IDMapping.insert( TPair<UUID, UUID>( originalID, m_ID ) );

        if ( m_pChildGraph != nullptr )
        {
            m_pChildGraph->RegenerateIDs( IDMapping );
        }

        if ( m_pSecondaryGraph != nullptr )
        {
            m_pSecondaryGraph->RegenerateIDs( IDMapping );
        }

        return m_ID;
    }

    //-------------------------------------------------------------------------

    TMultiUserEventInternal<BaseGraph*> BaseGraph::s_onEndModification;
    TMultiUserEventInternal<BaseGraph*> BaseGraph::s_onBeginModification;

    //-------------------------------------------------------------------------

    BaseGraph::~BaseGraph()
    {
        KRG_ASSERT( m_pParentNode == nullptr );
        KRG_ASSERT( m_nodes.empty() );
        KRG_ASSERT( !m_ID.IsValid() );
    }

    void BaseGraph::Initialize( BaseNode* pParentNode )
    {
        m_ID = UUID::GenerateID();
        m_pParentNode = pParentNode;
    }

    void BaseGraph::Shutdown()
    {
        for ( auto pNode : m_nodes )
        {
            pNode->Shutdown();
            KRG::Delete( pNode );
        }

        m_nodes.clear();
        m_pParentNode = nullptr;
        m_ID.Clear();
    }

    BaseGraph* BaseGraph::GetRootGraph()
    {
        auto pRootGraph = this;
        while ( pRootGraph->m_pParentNode != nullptr )
        {
            pRootGraph = pRootGraph->m_pParentNode->GetParentGraph();
        }

        return pRootGraph;
    }

    void BaseGraph::BeginModification()
    {
        auto pRootGraph = GetRootGraph();

        if ( pRootGraph->m_beginModificationCallCount == 0 )
        {
            if ( s_onBeginModification.HasBoundUsers() )
            {
                s_onBeginModification.Execute( pRootGraph );
            }
        }
        pRootGraph->m_beginModificationCallCount++;
    }

    void BaseGraph::EndModification()
    {
        auto pRootGraph = GetRootGraph();

        KRG_ASSERT( pRootGraph->m_beginModificationCallCount > 0 );
        pRootGraph->m_beginModificationCallCount--;

        if ( pRootGraph->m_beginModificationCallCount == 0 )
        {
            if ( s_onEndModification.HasBoundUsers() )
            {
                s_onEndModification.Execute( pRootGraph );
            }
        }
    }

    void BaseGraph::FindAllNodesOfType( TypeSystem::TypeID typeID, TInlineVector<BaseNode*, 20>& results, SearchMode mode, SearchTypeMatch typeMatch ) const
    {
        for ( auto pNode : m_nodes )
        {
            if ( pNode->GetTypeID() == typeID )
            {
                results.emplace_back( pNode );
            }
            else if ( typeMatch == SearchTypeMatch::Derived )
            {
                if ( pNode->GetTypeInfo()->IsDerivedFrom( typeID ) )
                {
                    results.emplace_back( pNode );
                }
            }

            // If recursion is allowed
            if ( mode == SearchMode::Recursive )
            {
                if ( pNode->HasChildGraph() )
                {
                    pNode->GetChildGraph()->FindAllNodesOfType( typeID, results, mode, typeMatch );
                }

                //-------------------------------------------------------------------------

                if ( pNode->HasSecondaryGraph() )
                {
                    pNode->GetSecondaryGraph()->FindAllNodesOfType( typeID, results, mode, typeMatch );
                }
            }
        }
    }

    void BaseGraph::DestroyNode( UUID nodeID )
    {
        for ( auto iter = m_nodes.begin(); iter != m_nodes.end(); ++iter )
        {
            auto pNode = *iter;
            if ( pNode->GetID() == nodeID )
            {
                BeginModification();
                PreDestroyNode( pNode );

                // Delete the node
                pNode->Shutdown();
                m_nodes.erase( iter );
                KRG::Delete( pNode );

                PostDestroyNode( nodeID );
                EndModification();
                return;
            }
        }

        KRG_UNREACHABLE_CODE();
    }

    BaseGraph* BaseGraph::CreateGraphFromSerializedData( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& graphObjectValue, BaseNode* pParentNode )
    {
        TypeSystem::TypeID const graphTypeID = graphObjectValue[BaseNode::s_typeDataKey][TypeSystem::Serialization::s_typeIDKey].GetString();
        auto pTypeInfo = typeRegistry.GetTypeInfo( graphTypeID );
        KRG_ASSERT( pTypeInfo != nullptr );

        BaseGraph* pGraph = TypeSystem::Serialization::CreateAndReadNativeType<BaseGraph>( typeRegistry, graphObjectValue[BaseNode::s_typeDataKey] );
        pGraph->m_pParentNode = pParentNode;
        pGraph->Serialize( typeRegistry, graphObjectValue );
        return pGraph;
    }

    void BaseGraph::Serialize( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& graphObjectValue )
    {
        KRG_ASSERT( graphObjectValue.IsObject() );

        // Note serialization is not symmetric for the graphs, since the graph creation also deserializes registered values

        // Nodes
        //-------------------------------------------------------------------------

        for ( auto pNode : m_nodes )
        {
            KRG::Delete( pNode );
        }
        m_nodes.clear();

        for ( auto& nodeObjectValue : graphObjectValue[s_nodesKey].GetArray() )
        {
            m_nodes.emplace_back( BaseNode::CreateNodeFromSerializedData( typeRegistry, nodeObjectValue, this ) );
        }

        // Custom Data
        //-------------------------------------------------------------------------

        SerializeCustom( typeRegistry, graphObjectValue );
    }

    void BaseGraph::Serialize( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonWriter& writer ) const
    {
        writer.StartObject();

        writer.Key( BaseNode::s_typeDataKey );
        TypeSystem::Serialization::WriteNativeType( typeRegistry, this, writer );

        // Nodes
        //-------------------------------------------------------------------------

        writer.Key( s_nodesKey );
        writer.StartArray();

        for ( auto pNode : m_nodes )
        {
            pNode->Serialize( typeRegistry, writer );
        }

        writer.EndArray();

        // Custom Data
        //-------------------------------------------------------------------------

        SerializeCustom( typeRegistry, writer );

        //-------------------------------------------------------------------------

        writer.EndObject();
    }

    UUID BaseGraph::RegenerateIDs( THashMap<UUID, UUID>& IDMapping )
    {
        UUID const originalID = m_ID;
        m_ID = UUID::GenerateID();

        KRG_ASSERT( IDMapping.find( originalID ) == IDMapping.end() );
        IDMapping.insert( TPair<UUID, UUID>( originalID, m_ID ) );

        for( auto pNode : m_nodes )
        {
            pNode->RegenerateIDs( IDMapping );
        }

        return m_ID;
    }
}