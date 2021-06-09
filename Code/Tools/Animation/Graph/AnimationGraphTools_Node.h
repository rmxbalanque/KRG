#pragma once
#include "AnimationGraphTools_Common.h"
#include "Tools/Core/GraphEditor/FlowGraph.h"
#include "Engine/Animation/Graph/AnimationGraphCommon.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class ToolsGraphCompilationContext;

    //-------------------------------------------------------------------------

    class ToolsNode : public GraphEditor::Flow::Node
    {
        KRG_REGISTER_TYPE( ToolsNode );

    public:

        // Get the type of node this is, this is either the output type for the nodes with output or the first input for nodes with no outputs
        KRG_FORCE_INLINE NodeValueType GetValueType() const
        {
            KRG_ASSERT( GetNumOutputPins() == 1 );
            return NodeValueType( GetOutputPin( 0 )->m_type );
        }

        virtual ImColor GetColor() const final { return GetColorForValueType( GetValueType() ); }

        virtual ImColor GetPinColor( GraphEditor::Flow::Pin const& pin ) const final { return GetColorForValueType( (NodeValueType) pin.m_type ); }

        // Get the types of graphs that this node is allowed to be placed in
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const = 0;

        // Is this node a persistent node i.e. is it always initialized 
        virtual bool IsPersistentNode() const { return false; }

        // Compile this node into its runtime representation. Returns the node index of the compiled node.
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const { return InvalidIndex; }

    protected:

        KRG_FORCE_INLINE void CreateInputPin( char const* pPinName, NodeValueType pinType ) { GraphEditor::Flow::Node::CreateInputPin( pPinName, (uint32) pinType ); }
        KRG_FORCE_INLINE void CreateOutputPin( char const* pPinName, NodeValueType pinType, bool allowMultipleOutputConnections = false ) { GraphEditor::Flow::Node::CreateOutputPin( pPinName, (uint32) pinType, allowMultipleOutputConnections ); }
    };
}