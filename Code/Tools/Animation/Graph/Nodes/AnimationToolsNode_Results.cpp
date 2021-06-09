#include "AnimationToolsNode_Results.h"
#include "Tools/Animation/Graph/AnimationGraphTools_Compilation.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    ResultToolsNode::ResultToolsNode( NodeValueType type )
        : ToolsNode()
    {
        CreateInputPin( "Out", type );
    }

    NodeIndex ResultToolsNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        // Get connected node and compile it
        auto pConnectedNode = GetConnectedInputNode<ToolsNode>( 0 );
        if ( pConnectedNode != nullptr )
        {
            return pConnectedNode->Compile( context );
        }

        context.LogError( this, "Result node not connected" );
        return InvalidIndex;
    }
}