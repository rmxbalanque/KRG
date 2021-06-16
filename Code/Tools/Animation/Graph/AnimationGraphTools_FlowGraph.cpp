#include "AnimationGraphTools_FlowGraph.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    FlowToolGraph::FlowToolGraph( GraphType type )
        : m_type( type )
    {}

    void FlowToolGraph::SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& graphObjectValue )
    {
        GraphEditor::FlowGraph::SerializeCustom( typeRegistry, graphObjectValue );

        m_type = (GraphType) graphObjectValue["GraphType"].GetUint();
    }

    void FlowToolGraph::SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonWriter& writer ) const
    {
        GraphEditor::FlowGraph::SerializeCustom( typeRegistry, writer );

        writer.Key( "GraphType" );
        writer.Uint( (uint8) m_type );
    }
}