#include "AnimationToolsNode_Results.h"
#include "Tools/Animation/Graph/AnimationGraphTools_Compilation.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    ResultToolsNode::ResultToolsNode( NodeValueType valueType )
        : m_valueType( valueType )
    {}

    void ResultToolsNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        FlowToolsNode::Initialize( pParent );
        CreateInputPin( "Out", m_valueType );
    }

    NodeIndex ResultToolsNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        // Get connected node and compile it
        auto pConnectedNode = GetConnectedInputNode<FlowToolsNode>( 0 );
        if ( pConnectedNode != nullptr )
        {
            return pConnectedNode->Compile( context );
        }

        context.LogError( this, "Result node not connected" );
        return InvalidIndex;
    }
}