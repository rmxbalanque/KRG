#include "Animation_EditorGraphNode_Vectors.h"
#include "Tools/Animation/GraphEditor/EditorGraph/Animation_EditorGraph_Compilation.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::GraphNodes
{
    void VectorInfoEditorNode::Initialize( VisualGraph::BaseGraph* pParent )
    {
        EditorGraphNode::Initialize( pParent );
        CreateOutputPin( "Result", GraphValueType::Float, true );
        CreateInputPin( "Vector", GraphValueType::Vector );
    }

    GraphNodeIndex VectorInfoEditorNode::Compile( EditorGraphCompilationContext& context ) const
    {
        VectorInfoNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<VectorInfoNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            auto pInputNode = GetConnectedInputNode<EditorGraphNode>( 0 );
            if ( pInputNode != nullptr )
            {
                GraphNodeIndex const compiledNodeIdx = pInputNode->Compile( context );
                if ( compiledNodeIdx != InvalidIndex )
                {
                    pSettings->m_inputValueNodeIdx = compiledNodeIdx;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                context.LogError( this, "Disconnected input pin!" );
                return false;
            }

            //-------------------------------------------------------------------------

            pSettings->m_desiredInfo = m_desiredInfo;
        }
        return pSettings->m_nodeIdx;
    }

    //-------------------------------------------------------------------------

    void VectorNegateEditorNode::Initialize( VisualGraph::BaseGraph* pParent )
    {
        EditorGraphNode::Initialize( pParent );
        CreateOutputPin( "Result", GraphValueType::Vector, true );
        CreateInputPin( "Vector", GraphValueType::Vector );
    }

    GraphNodeIndex VectorNegateEditorNode::Compile( EditorGraphCompilationContext& context ) const
    {
        VectorNegateNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<VectorNegateNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            auto pInputNode = GetConnectedInputNode<EditorGraphNode>( 0 );
            if ( pInputNode != nullptr )
            {
                GraphNodeIndex const compiledNodeIdx = pInputNode->Compile( context );
                if ( compiledNodeIdx != InvalidIndex )
                {
                    pSettings->m_inputValueNodeIdx = compiledNodeIdx;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                context.LogError( this, "Disconnected input pin!" );
                return false;
            }
        }
        return pSettings->m_nodeIdx;
    }
}