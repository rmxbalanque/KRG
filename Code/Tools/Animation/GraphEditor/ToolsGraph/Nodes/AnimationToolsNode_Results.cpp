#include "AnimationToolsNode_Results.h"
#include "Tools/Animation/GraphEditor/ToolsGraph/AnimationToolsGraph_Compilation.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    Tools_ResultNode::Tools_ResultNode( ValueType valueType )
        : m_valueType( valueType )
    {}

    void Tools_ResultNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        Tools_GraphNode::Initialize( pParent );
        CreateInputPin( "Out", m_valueType );
    }

    NodeIndex Tools_ResultNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        // Get connected node and compile it
        auto pConnectedNode = GetConnectedInputNode<Tools_GraphNode>( 0 );
        if ( pConnectedNode != nullptr )
        {
            return pConnectedNode->Compile( context );
        }

        context.LogError( this, "Result node not connected" );
        return InvalidIndex;
    }
}