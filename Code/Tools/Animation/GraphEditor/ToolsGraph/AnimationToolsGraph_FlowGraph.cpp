#include "AnimationToolsGraph_FlowGraph.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    FlowGraph::FlowGraph( GraphType type )
        : m_type( type )
    {}

    void FlowGraph::SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& graphObjectValue )
    {
        GraphEditor::FlowGraph::SerializeCustom( typeRegistry, graphObjectValue );

        m_type = (GraphType) graphObjectValue["GraphType"].GetUint();
    }

    void FlowGraph::SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonWriter& writer ) const
    {
        GraphEditor::FlowGraph::SerializeCustom( typeRegistry, writer );

        writer.Key( "GraphType" );
        writer.Uint( (uint8) m_type );
    }
}