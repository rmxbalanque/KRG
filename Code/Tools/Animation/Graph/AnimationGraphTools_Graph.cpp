#include "AnimationGraphTools_Graph.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    ToolsGraph::ToolsGraph( GraphType type )
        : m_type( type )
    {}

    void ToolsGraph::SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& graphObjectValue )
    {
        m_type = (GraphType) graphObjectValue["GraphType"].GetUint();
    }

    void ToolsGraph::SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonWriter& writer ) const
    {
        writer.Key( "GraphType" );
        writer.Uint( (uint8) m_type );
    }
}