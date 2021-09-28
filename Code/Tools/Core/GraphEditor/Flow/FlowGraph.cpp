#include "FlowGraph.h"
#include "System/Core/Logging/Log.h"
#include "Tools/Core/TypeSystem/Serialization/TypeSerialization.h"

//-------------------------------------------------------------------------

namespace KRG::GraphEditor::Flow
{
    Pin const* Node::GetInputPin( UUID const& pinID ) const
    {
        for ( auto const& pin : m_inputPins )
        {
            if ( pin.m_ID == pinID )
            {
                return &pin;
            }
        }

        return nullptr;
    }

    Pin const* Node::GetOutputPin( UUID const& pinID ) const
    {
        for ( auto const& pin : m_outputPins )
        {
            if ( pin.m_ID == pinID )
            {
                return &pin;
            }
        }

        return nullptr;
    }

    int32 Node::GetInputPinIndex( UUID const& pinID ) const
    {
        for ( auto i = 0; i < m_inputPins.size(); i++ )
        {
            if ( m_inputPins[i].m_ID == pinID )
            {
                return (int32) i;
            }
        }

        return InvalidIndex;
    }

    int32 Node::GetOutputPinIndex( UUID const& pinID ) const
    {
        for ( auto i = 0; i < m_outputPins.size(); i++ )
        {
            if ( m_outputPins[i].m_ID == pinID )
            {
                return (int32) i;
            }
        }

        return InvalidIndex;
    }

    //-------------------------------------------------------------------------

    void Node::CreateInputPin( char const* pPinName, uint32 valueType )
    {
        auto& newPin = m_inputPins.emplace_back( Pin() );
        newPin.m_name = pPinName;
        newPin.m_type = valueType;
        newPin.m_direction = Pin::Direction::In;
    }

    void Node::CreateOutputPin( char const* pPinName, uint32 valueType, bool allowMultipleOutputConnections )
    {
        auto& newPin = m_outputPins.emplace_back( Pin() );
        newPin.m_name = pPinName;
        newPin.m_type = valueType;
        newPin.m_direction = Pin::Direction::Out;
        newPin.m_allowMultipleOutConnections = allowMultipleOutputConnections;
    }

    void Node::DestroyInputPin( int32 pinIdx )
    {
        KRG_ASSERT( pinIdx >= 0 && pinIdx < m_inputPins.size() );
        static_cast<GraphEditor::FlowGraph*>( GetParentGraph() )->BreakAnyConnectionsForPin( m_inputPins[pinIdx].m_ID );
        m_inputPins.erase( m_inputPins.begin() + pinIdx );
        return;
    }

    void Node::DestroyOutputPin( int32 pinIdx )
    {
        KRG_ASSERT( pinIdx >= 0 && pinIdx < m_outputPins.size() );
        static_cast<GraphEditor::FlowGraph*>( GetParentGraph() )->BreakAnyConnectionsForPin( m_outputPins[pinIdx].m_ID );
        m_outputPins.erase( m_outputPins.begin() + pinIdx );
        return;
    }

    void Node::DestroyPin( UUID const& pinID )
    {
        for ( auto iter = m_inputPins.begin(); iter != m_inputPins.end(); ++iter )
        {
            if ( iter->m_ID == pinID )
            {
                static_cast<GraphEditor::FlowGraph*>( GetParentGraph() )->BreakAnyConnectionsForPin( pinID );
                m_inputPins.erase( iter );
                return;
            }
        }

        for ( auto iter = m_outputPins.begin(); iter != m_outputPins.end(); ++iter )
        {
            if ( iter->m_ID == pinID )
            {
                static_cast<GraphEditor::FlowGraph*>( GetParentGraph() )->BreakAnyConnectionsForPin( pinID );
                m_outputPins.erase( iter );
                return;
            }
        }

        KRG_UNREACHABLE_CODE();
    }

    void Node::CreateDynamicInputPin()
    {
        auto& newPin = m_inputPins.emplace_back( Pin() );
        newPin.m_name = GetNewDynamicInputPinName();
        newPin.m_type = GetDynamicInputPinValueType();
        newPin.m_direction = Pin::Direction::In;
        newPin.m_isDynamic = true;
        OnDynamicPinCreation( newPin.m_ID );
    }

    void Node::DestroyDynamicInputPin( UUID const& pinID )
    {
        auto pPin = GetInputPin( pinID );
        KRG_ASSERT( pPin != nullptr && pPin->IsDynamicPin() );
        OnDynamicPinDestruction( pinID );
        DestroyPin( pinID );
    }

    //-------------------------------------------------------------------------

    Flow::Node* Node::GetConnectedInputNode( int32 inputPinIdx ) const
    {
        KRG_ASSERT( inputPinIdx >= 0 && inputPinIdx < m_inputPins.size() );
        KRG_ASSERT( HasParentGraph() );
        auto const pParentGraph = (FlowGraph const*) GetParentGraph();
        return pParentGraph->GetConnectedNodeForInputPin( m_inputPins[inputPinIdx].m_ID );
    }

    //-------------------------------------------------------------------------

    void Node::SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& nodeObjectValue )
    {
        KRG_ASSERT( nodeObjectValue.IsObject() );

        //-------------------------------------------------------------------------

        m_inputPins.clear();
        for ( auto& pinObjectValue : nodeObjectValue["InputPins"].GetArray() )
        {
            Pin& pin = m_inputPins.emplace_back();
            pin.m_direction = Pin::Direction::In;
            pin.m_ID = UUID( pinObjectValue["ID"].GetString() );
            pin.m_name = pinObjectValue["Name"].GetString();
            pin.m_type = pinObjectValue["Type"].GetUint();
            pin.m_isDynamic = pinObjectValue["IsDynamic"].GetBool();
        }

        //-------------------------------------------------------------------------

        m_outputPins.clear();
        for ( auto& pinObjectValue : nodeObjectValue["OutputPins"].GetArray() )
        {
            Pin& pin = m_outputPins.emplace_back();
            pin.m_direction = Pin::Direction::Out;
            pin.m_ID = UUID( pinObjectValue["ID"].GetString() );
            pin.m_name = pinObjectValue["Name"].GetString();
            pin.m_type = pinObjectValue["Type"].GetUint();
            pin.m_allowMultipleOutConnections = pinObjectValue["AllowMultipleConnections"].GetBool();
        }
    }

    void Node::SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonWriter& writer ) const
    {
        writer.Key( "InputPins" );
        writer.StartArray();
        for ( auto const& pin : m_inputPins )
        {
            KRG_ASSERT( pin.IsInputPin() );

            writer.StartObject();

            writer.Key( "ID" );
            writer.String( pin.m_ID.ToString().c_str() );

            writer.Key( "Name" );
            writer.String( pin.m_name.c_str() );

            writer.Key( "Type" );
            writer.Uint( pin.m_type );

            writer.Key( "IsDynamic" );
            writer.Bool( pin.m_isDynamic );

            writer.EndObject();
        }
        writer.EndArray();

        //-------------------------------------------------------------------------

        writer.Key( "OutputPins" );
        writer.StartArray();
        for ( auto const& pin : m_outputPins )
        {
            KRG_ASSERT( pin.IsOutputPin() );

            writer.StartObject();

            writer.Key( "ID" );
            writer.String( pin.m_ID.ToString().c_str() );

            writer.Key( "Name" );
            writer.String( pin.m_name.c_str() );

            writer.Key( "Type" );
            writer.Uint( pin.m_type );

            writer.Key( "AllowMultipleConnections" );
            writer.Bool( pin.m_allowMultipleOutConnections );

            writer.EndObject();
        }
        writer.EndArray();
    }
}

//-------------------------------------------------------------------------

namespace KRG::GraphEditor
{
    using namespace Flow;

    //-------------------------------------------------------------------------

    void FlowGraph::PreDestroyNode( BaseNode* pNode )
    {
        BreakAllConnectionsForNode( pNode->GetID() );
        BaseGraph::PreDestroyNode( pNode );
    }

    Flow::Node* FlowGraph::GetNodeForPinID( UUID const& pinID ) const
    {
        for ( auto pNode : m_nodes )
        {
            auto pFlowNode = Cast<Flow::Node>( pNode );
            if ( pFlowNode->HasPin( pinID ) )
            {
                return pFlowNode;
            }
        }

        return nullptr;
    }

    void FlowGraph::CreateDynamicPin( UUID const& nodeID )
    {
        auto pNode = GetNode( nodeID );
        pNode->CreateDynamicInputPin();
    }

    void FlowGraph::DestroyDynamicPin( UUID const& nodeID, UUID const& pinID )
    {
        auto pNode = GetNode( nodeID );
        auto pPin = pNode->GetInputPin( pinID );
        KRG_ASSERT( pPin->IsDynamicPin() && pPin->IsInputPin() );

        BreakAnyConnectionsForPin( pPin->m_ID );
        pNode->DestroyDynamicInputPin( pPin->m_ID );
    }

    //-------------------------------------------------------------------------

    bool FlowGraph::IsValidConnection( Node* pOutputPinNode, Pin* pOutputPin, Node* pInputPinNode, Pin* pInputPin ) const
    {
        KRG_ASSERT( pOutputPinNode != nullptr && pOutputPin != nullptr );
        KRG_ASSERT( pOutputPinNode->HasOutputPin( pOutputPin->m_ID ) );
        KRG_ASSERT( pInputPinNode != nullptr && pInputPin != nullptr );
        KRG_ASSERT( pInputPinNode->HasInputPin( pInputPin->m_ID ) );

        //-------------------------------------------------------------------------

        if ( CheckForCyclicConnection( pOutputPinNode, pInputPinNode ) )
        {
            return false;
        }

        if ( !pInputPinNode->IsValidConnection( pInputPin->m_ID, pOutputPinNode, pOutputPin->m_ID ) )
        {
            return false;
        }

        return true;
    }

    bool FlowGraph::TryMakeConnection( Node* pOutputPinNode, Pin* pOutputPin, Node* pInputPinNode, Pin* pInputPin )
    {
        if ( !IsValidConnection( pOutputPinNode, pOutputPin, pInputPinNode, pInputPin ) )
        {
            return false;
        }

        //-------------------------------------------------------------------------

        // Should we break any existing connections
        if ( !pOutputPin->m_allowMultipleOutConnections )
        {
            BreakAnyConnectionsForPin( pOutputPin->m_ID );
        }

        // Break any existing connections for the end pin
        BreakAnyConnectionsForPin( pInputPin->m_ID );

        // Create connection
        auto& newConnection = m_connections.emplace_back();
        newConnection.m_pStartNode = pOutputPinNode;
        newConnection.m_startPinID = pOutputPin->m_ID;
        newConnection.m_pEndNode = pInputPinNode;
        newConnection.m_endPinID = pInputPin->m_ID;

        return true;
    }

    void FlowGraph::BreakConnection( UUID const& connectionID )
    {
        for ( int32 i = int32( m_connections.size() ) - 1; i >= 0; i-- )
        {
            if ( m_connections[i].m_ID == connectionID )
            {
                m_connections.erase_unsorted( m_connections.begin() + i );
                return;
            }
        }

        KRG_UNREACHABLE_CODE();
    }

    void FlowGraph::BreakAnyConnectionsForPin( UUID const& pinID )
    {
        for ( int32 i = int32( m_connections.size() ) - 1; i >= 0; i-- )
        {
            if ( m_connections[i].m_startPinID == pinID || m_connections[i].m_endPinID == pinID )
            {
                m_connections.erase_unsorted( m_connections.begin() + i );
            }
        }
    }

    void FlowGraph::BreakAllConnectionsForNode( Flow::Node* pNode )
    {
        KRG_ASSERT( pNode != nullptr );

        for ( int32 i = int32( m_connections.size() ) - 1; i >= 0; i-- )
        {
            if ( m_connections[i].m_pStartNode == pNode || m_connections[i].m_pEndNode == pNode )
            {
                m_connections.erase_unsorted( m_connections.begin() + i );
            }
        }
    }

    void FlowGraph::BreakAllConnectionsForNode( UUID const& nodeID )
    {
        KRG_ASSERT( nodeID.IsValid() );

        for ( int32 i = int32( m_connections.size() ) - 1; i >= 0; i-- )
        {
            if ( m_connections[i].m_pStartNode->GetID() == nodeID || m_connections[i].m_pEndNode->GetID() == nodeID )
            {
                m_connections.erase_unsorted( m_connections.begin() + i );
            }
        }
    }

    Flow::Node* FlowGraph::GetConnectedNodeForInputPin( UUID const& pinID ) const
    {
        for ( auto const& connection : m_connections )
        {
            if ( connection.m_endPinID == pinID )
            {
                KRG_ASSERT( connection.m_pEndNode == GetNodeForPinID( pinID ) );
                return connection.m_pStartNode;
            }
        }

        return nullptr;
    }

    void FlowGraph::GetConnectedOutputNodes( Flow::Node const* pOriginalStartNode, Flow::Node const* pNode, TInlineVector<Flow::Node const*, 10>& connectedNodes ) const
    {
        KRG_ASSERT( pNode != nullptr );

        for ( auto const& connection : m_connections )
        {
            if ( connection.m_pStartNode == pNode )
            {
                connectedNodes.emplace_back( connection.m_pEndNode );
                if ( pNode != pOriginalStartNode )
                {
                    GetConnectedOutputNodes( pOriginalStartNode, connection.m_pEndNode, connectedNodes );
                }
            }
        }
    }

    bool FlowGraph::CheckForCyclicConnection( Flow::Node const* pOutputNode, Flow::Node const* pInputNode ) const
    {
        KRG_ASSERT( pOutputNode != nullptr && pInputNode != nullptr );

        if ( pInputNode == pOutputNode )
        {
            return true;
        }

        TInlineVector<Flow::Node const*, 10> connectedNodes;
        GetConnectedOutputNodes( pInputNode, pInputNode, connectedNodes );
        return VectorContains( connectedNodes, pOutputNode );
    }

    //-------------------------------------------------------------------------

    void FlowGraph::SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& graphObjectValue )
    {
        KRG_ASSERT( graphObjectValue.IsObject() );

        m_connections.clear();
        for ( auto& connectionObjectValue : graphObjectValue["Connections"].GetArray() )
        {
            auto FindNode = [this] ( UUID const& ID ) -> Flow::Node*
            {
                for ( auto pNode : m_nodes )
                {
                    if ( pNode->GetID() == ID )
                    {
                        return Cast<Flow::Node>( pNode );
                    }
                }

                return nullptr;
            };

            //-------------------------------------------------------------------------

            KRG_ASSERT( connectionObjectValue.IsObject() );
            Connection& connection = m_connections.emplace_back();

            UUID const startNodeID = UUID( connectionObjectValue["StartNode"].GetString() );
            UUID const endNodeID = UUID( connectionObjectValue["EndNode"].GetString() );

            connection.m_pStartNode = FindNode( startNodeID );
            connection.m_pEndNode = FindNode( endNodeID );
            connection.m_startPinID = UUID( connectionObjectValue["StartPin"].GetString() );
            connection.m_endPinID = UUID( connectionObjectValue["EndPin"].GetString() );
        }
    }

    void FlowGraph::SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonWriter& writer ) const
    {
        writer.Key( "Connections" );
        writer.StartArray();

        for ( auto const& connection : m_connections )
        {
            writer.StartObject();

            writer.Key( "StartNode" );
            writer.String( connection.m_pStartNode->GetID().ToString().c_str() );

            writer.Key( "StartPin" );
            writer.String( connection.m_startPinID.ToString().c_str() );

            writer.Key( "EndNode" );
            writer.String( connection.m_pEndNode->GetID().ToString().c_str() );

            writer.Key( "EndPin" );
            writer.String( connection.m_endPinID.ToString().c_str() );

            writer.EndObject();
        }

        writer.EndArray();
    }
}