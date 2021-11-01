#include "GraphEditor.h"
#include "Tools/Core/TypeSystem/Serialization/TypeSerialization.h"

//-------------------------------------------------------------------------

namespace KRG::GraphEditor
{
    static void TraverseHierarchy( GraphEditor::BaseNode const* pNode, TVector<GraphEditor::BaseNode const*>& nodePath )
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
        m_ID.Reset();
    }

    BaseNode* BaseNode::CreateNodeFromSerializedData( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& nodeObjectValue, BaseGraph* pParentGraph )
    {
        TypeSystem::TypeID const nodeTypeID = nodeObjectValue["TypeData"]["TypeID"].GetString();
        auto pTypeInfo = typeRegistry.GetTypeInfo( nodeTypeID );
        KRG_ASSERT( pTypeInfo != nullptr );

        BaseNode* pNode = TypeSystem::Serialization::CreateAndReadNativeType<BaseNode>( typeRegistry, nodeObjectValue["TypeData"] );
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
        TVector<GraphEditor::BaseNode const*> path;
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

        auto const childGraphValueIter = nodeObjectValue.FindMember( "ChildGraph" );
        if ( childGraphValueIter != nodeObjectValue.MemberEnd() )
        {
            KRG_ASSERT( childGraphValueIter->value.IsObject() );
            auto& graphObjectValue = childGraphValueIter->value;
            m_pChildGraph = BaseGraph::CreateGraphFromSerializedData( typeRegistry, graphObjectValue, this );
        }

        //-------------------------------------------------------------------------

        KRG::Delete( m_pSecondaryGraph );

        auto const secondaryGraphValueIter = nodeObjectValue.FindMember( "SecondaryGraph" );
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

        writer.Key( "TypeData" );
        TypeSystem::Serialization::WriteNativeType( typeRegistry, writer, this );

        //-------------------------------------------------------------------------

        SerializeCustom( typeRegistry, writer );

        //-------------------------------------------------------------------------

        if ( HasChildGraph() )
        {
            writer.Key( "ChildGraph" );
            GetChildGraph()->Serialize( typeRegistry, writer );
        }

        //-------------------------------------------------------------------------

        if ( HasSecondaryGraph() )
        {
            writer.Key( "SecondaryGraph" );
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
        m_ID.Reset();
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
                PreDestroyNode( pNode );

                // Delete the node
                pNode->Shutdown();
                m_nodes.erase( iter );
                KRG::Delete( pNode );

                PostDestroyNode( nodeID );
                return;
            }
        }

        KRG_UNREACHABLE_CODE();
    }

    BaseGraph* BaseGraph::CreateGraphFromSerializedData( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& graphObjectValue, BaseNode* pParentNode )
    {
        TypeSystem::TypeID const graphTypeID = graphObjectValue["TypeData"]["TypeID"].GetString();
        auto pTypeInfo = typeRegistry.GetTypeInfo( graphTypeID );
        KRG_ASSERT( pTypeInfo != nullptr );

        BaseGraph* pGraph = TypeSystem::Serialization::CreateAndReadNativeType<BaseGraph>( typeRegistry, graphObjectValue["TypeData"] );
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

        for ( auto& nodeObjectValue : graphObjectValue["Nodes"].GetArray() )
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

        writer.Key( "TypeData" );
        TypeSystem::Serialization::WriteNativeType( typeRegistry, writer, this );

        // Nodes
        //-------------------------------------------------------------------------

        writer.Key( "Nodes" );
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
}