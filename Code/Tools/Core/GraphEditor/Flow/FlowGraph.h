#pragma once

#include "../GraphEditor.h"
#include "System/Core/Types/String.h"
#include "System/Core/Types/UUID.h"

//-------------------------------------------------------------------------
// Flow Graph
//-------------------------------------------------------------------------
// A basic node graph, allowing you to edit a set a of nodes visually
// While this uses the KRG reflection system, the serialization is custom since its serialized to a specific json format

namespace KRG::GraphEditor
{
    class FlowGraph;
    class FlowGraphView;

    //-------------------------------------------------------------------------

    namespace Flow
    {
        struct VisualSettings
        {
            constexpr static uint32 const   s_nodeTitleColor = IM_COL32( 0, 0, 0, 255 );
        };

        //-------------------------------------------------------------------------
        // Flow Graph Pin
        //-------------------------------------------------------------------------
        // A pin (connection point) for a node's input/outputs

        struct Pin
        {
            enum class Direction
            {
                In,
                Out
            };

            inline bool IsInputPin() const { return m_direction == Direction::In; }
            inline bool IsOutputPin() const { return m_direction == Direction::Out; }
            inline bool IsDynamicPin() const { return m_isDynamic; }

            UUID                    m_ID = UUID::GenerateID();
            String                  m_name;
            uint32                  m_type; // Generic type that allows user to set custom data be it StringIDs or enum values
            Direction               m_direction;
            ImVec2                  m_screenPosition; // Updated each frame (Screen Space)
            ImVec2                  m_estimatedSize = ImVec2( -1, -1 ); // Updated on size calculation
            bool                    m_isDynamic = false; // Only relevant for input pins
            bool                    m_allowMultipleOutConnections = false; // Only relevant for output pins
        };

        //-------------------------------------------------------------------------
        // Flow Graph Node
        //-------------------------------------------------------------------------
        // Base class for a node inside of a flow graph, provides helpers for all common operations
        // Users are expected to derive their own custom nodes from this node 

        class KRG_TOOLS_CORE_API Node : public BaseNode
        {
            friend FlowGraph;
            friend FlowGraphView;

            KRG_REGISTER_TYPE( Node );

        public:

            using BaseNode::BaseNode;

            // Node 
            //-------------------------------------------------------------------------

            // Get node category name (separated via '/')
            virtual char const* GetCategory() const { return nullptr; }

            // Pins
            //-------------------------------------------------------------------------

            // Does this node support dynamic inputs
            virtual bool SupportsDynamicInputPins() const { return false; }

            // Whats the name of the dynamic inputs
            virtual InlineString<100> GetNewDynamicInputPinName() const { return "Pin"; }

            // What's the value type of the dynamic inputs
            virtual uint32 GetDynamicInputPinValueType() const { return 0; }

            // Does this node have an output pin
            inline bool HasOutputPin() const { return !m_outputPins.empty(); }

            // Get the number of input pins on this node
            inline int32 GetNumInputPins() const { return (int32) m_inputPins.size(); }

            // Get the number of input pins on this node
            inline int32 GetNumOutputPins() const { return (int32) m_outputPins.size(); }

            // Get all input pins
            inline TInlineVector<Pin, 4> const& GetInputPins() const { return m_inputPins; }

            // Get all output pins
            inline TInlineVector<Pin, 1> const& GetOutputPins() const { return m_outputPins; }

            // Does an input pin exist with this ID
            inline bool HasInputPin( UUID const& pinID ) const { return GetInputPin( pinID ) != nullptr; }

            // Does an input pin exist with this ID
            inline bool HasOutputPin( UUID const& pinID ) const { return GetOutputPin( pinID ) != nullptr; }

            // Get an input pin for this node
            inline Pin* GetInputPin( int32 pinIdx ) { KRG_ASSERT( pinIdx >= 0 && pinIdx < m_inputPins.size() ); return &m_inputPins[pinIdx]; }

            // Get an input pin for this node
            inline Pin const* GetInputPin( int32 pinIdx ) const { KRG_ASSERT( pinIdx >= 0 && pinIdx < m_inputPins.size() ); return &m_inputPins[pinIdx]; }

            // Get an output pin for this node
            inline Pin* GetOutputPin( int32 pinIdx = 0 ) { KRG_ASSERT( pinIdx >= 0 && pinIdx < m_outputPins.size() ); return &m_outputPins[pinIdx]; }

            // Get an output pin for this node
            inline Pin const* GetOutputPin( int32 pinIdx = 0 ) const { KRG_ASSERT( pinIdx >= 0 && pinIdx < m_outputPins.size() ); return &m_outputPins[pinIdx]; }

            // Get a specific input pin via ID
            inline Pin const* GetInputPin( UUID const& pinID ) const;

            // Get a specific input pin via ID
            inline Pin* GetInputPin( UUID const& pinID ) { return const_cast<Pin*>( const_cast<Node const*>( this )->GetInputPin( pinID ) ); }

            // Get a specific output pin via ID
            inline Pin const* GetOutputPin( UUID const& pinID ) const;

            // Get a specific output pin via ID
            inline Pin* GetOutputPin( UUID const& pinID ) { return const_cast<Pin*>( const_cast<Node const*>( this )->GetOutputPin( pinID ) ); }

            // Does the specified pin ID exist on this node
            inline bool HasPin( UUID const& pinID ) const { return HasInputPin( pinID ) || HasOutputPin( pinID ); }

            // Get the index for a specific input pin via ID
            int32 GetInputPinIndex( UUID const& pinID ) const;

            // Get the index for a specific output pin via ID
            int32 GetOutputPinIndex( UUID const& pinID ) const;

            // Connections
            //-------------------------------------------------------------------------

            Flow::Node* GetConnectedInputNode( int32 inputPinIdx ) const;

            template<typename T>
            T* GetConnectedInputNode( int32 inputPinIdx ) const { return TryCast<T>( GetConnectedInputNode( inputPinIdx ) ); }

            virtual bool IsValidConnection( UUID const& inputPinID, Node const* pOutputPinNode, UUID const& outputPinID ) const
            {
                auto pInputPin = GetInputPin( inputPinID );
                KRG_ASSERT( pInputPin != nullptr );

                auto pOutputPin = pOutputPinNode->GetOutputPin( outputPinID );
                KRG_ASSERT( pOutputPin != nullptr );

                return pInputPin->m_type == pOutputPin->m_type;
            }

            // Visual
            //-------------------------------------------------------------------------

            // What color should this pin and the connection for it be?
            virtual ImColor GetPinColor( Pin const& pin ) const { return 0xFF888888; }

        protected:

            // Override this if you want custom UI after/before the pin. Returns true if something was drawn, false otherwise
            virtual bool DrawPinControls( Pin const& pin ) { return false; }

            // Create a new dynamic pin
            void CreateDynamicInputPin();

            // Called just after creating a dynamic pin
            virtual void OnDynamicPinCreation( UUID pinID ) {}

            // Destroy a dynamic pin
            void DestroyDynamicInputPin( UUID const& pinID );

            // Called just before actually destroying a dynamic pin
            virtual void OnDynamicPinDestruction( UUID pinID ) {}

            void CreateInputPin( char const* pPinName, uint32 valueType );
            void CreateOutputPin( char const* pPinName, uint32 valueType, bool allowMultipleOutputConnections = false );
            void DestroyInputPin( int32 pinIdx );
            void DestroyOutputPin( int32 pinIdx );
            void DestroyPin( UUID const& pinID );

            // Serialization
            virtual void SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& nodeObjectValue ) override;
            virtual void SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonWriter& writer ) const override;

        private:

            Node( Node const& ) = delete;
            Node& operator=( Node const& ) = delete;

        private:

            TInlineVector<Pin, 4>       m_inputPins;
            TInlineVector<Pin, 1>       m_outputPins;
            ImVec2                      m_pinsRectSize = ImVec2( 0, 0 );
            Pin*                        m_pHoveredPin = nullptr;
        };

        //-------------------------------------------------------------------------
        // Flow Graph Connection
        //-------------------------------------------------------------------------
        // A connection is made between an output pin (start) and an input pin (end) on two different nodes

        struct Connection
        {
            UUID                        m_ID = UUID::GenerateID(); // Transient ID that is only needed for identification in the UI
            Node*                       m_pStartNode = nullptr;
            UUID                        m_startPinID;
            Node*                       m_pEndNode = nullptr;
            UUID                        m_endPinID;
        };
    }

    //-------------------------------------------------------------------------
    // Flow Graph
    //-------------------------------------------------------------------------
    // This is a base class as it only provide the most basic drawing and operation functionality
    // Users are expected to derive from this and provide mechanism to create nodes and other operations
    // Note: graphs own the memory for their nodes so they are responsible for managing it

    class KRG_TOOLS_CORE_API FlowGraph : public BaseGraph
    {
        KRG_REGISTER_TYPE( FlowGraph );

        friend FlowGraphView;

    public:

        // Nodes
        //-------------------------------------------------------------------------
        // Note: There are no default addition methods exposed, it is up to the derived graphs to create them

        // Destroys and deletes the specified node
        virtual void PreDestroyNode( BaseNode* pNode ) override;

        Flow::Node* GetNode( UUID const& nodeID ) const { return static_cast<Flow::Node*>( FindNode( nodeID ) ); }
        Flow::Node* GetNodeForPinID( UUID const& pinID ) const;

        void CreateDynamicPin( UUID const& nodeID );
        void DestroyDynamicPin( UUID const& nodeID, UUID const& pinID );

        // Connections
        //-------------------------------------------------------------------------

        bool IsValidConnection( Flow::Node* pOutputPinNode, Flow::Pin* pOutputPin, Flow::Node* pInputPinNode, Flow::Pin* pInputPin ) const;
        bool TryMakeConnection( Flow::Node* pOutputPinNode, Flow::Pin* pOutputPin, Flow::Node* pInputPinNode, Flow::Pin* pInputPin );
        void BreakConnection( UUID const& connectionID );

        void BreakAnyConnectionsForPin( UUID const& pinID );

        void BreakAllConnectionsForNode( Flow::Node* pNode );
        void BreakAllConnectionsForNode( UUID const& nodeID );

        Flow::Node* GetConnectedNodeForInputPin( UUID const& pinID ) const;

        void GetConnectedOutputNodes( Flow::Node const* pOriginalStartNode, Flow::Node const* pNode, TInlineVector<Flow::Node const*, 10>& connectedNodes ) const;

        // Serialization
        //-------------------------------------------------------------------------

        virtual void SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& graphObjectValue ) override;
        virtual void SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonWriter& writer ) const override;

    protected:

        bool CheckForCyclicConnection( Flow::Node const* pOutputNode, Flow::Node const* pInputNode ) const;

    public:

        TVector<Flow::Connection>       m_connections;
    };
}