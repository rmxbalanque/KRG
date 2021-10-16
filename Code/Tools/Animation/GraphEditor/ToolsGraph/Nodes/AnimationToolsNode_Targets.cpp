#include "AnimationToolsNode_Targets.h"
#include "Tools/Animation/GraphEditor/ToolsGraph/AnimationToolsGraph_Compilation.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void Tools_IsTargetSetNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        Tools_GraphNode::Initialize( pParent );
        CreateOutputPin( "Result", ValueType::Bool, true );
        CreateInputPin( "Target", ValueType::Target );
    }

    NodeIndex Tools_IsTargetSetNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        IsTargetSetNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<IsTargetSetNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            auto pInputNode = GetConnectedInputNode<Tools_GraphNode>( 0 );
            if ( pInputNode != nullptr )
            {
                NodeIndex const compiledNodeIdx = pInputNode->Compile( context );
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

    //-------------------------------------------------------------------------

    void Tools_TargetInfoNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        Tools_GraphNode::Initialize( pParent );
        CreateOutputPin( "Result", ValueType::Float, true );
        CreateInputPin( "Target", ValueType::Target );
    }

    NodeIndex Tools_TargetInfoNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        TargetInfoNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<TargetInfoNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            auto pInputNode = GetConnectedInputNode<Tools_GraphNode>( 0 );
            if ( pInputNode != nullptr )
            {
                NodeIndex const compiledNodeIdx = pInputNode->Compile( context );
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

            pSettings->m_expectedCoordinateSpace = m_expectedCoordinateSpace;
            pSettings->m_infoType = m_infoType;
        }
        return pSettings->m_nodeIdx;
    }

    //-------------------------------------------------------------------------

    void Tools_TargetOffsetNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        Tools_GraphNode::Initialize( pParent );
        CreateOutputPin( "Result", ValueType::Target, true );
        CreateInputPin( "Target", ValueType::Target );
    }

    NodeIndex Tools_TargetOffsetNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        TargetOffsetNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<TargetOffsetNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            auto pInputNode = GetConnectedInputNode<Tools_GraphNode>( 0 );
            if ( pInputNode != nullptr )
            {
                NodeIndex const compiledNodeIdx = pInputNode->Compile( context );
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

            pSettings->m_isBoneSpaceOffset = m_isBoneSpaceOffset;
            pSettings->m_rotationOffset = m_rotationOffset;
            pSettings->m_translationOffset = m_translationOffset;
        }
        return pSettings->m_nodeIdx;
    }
}