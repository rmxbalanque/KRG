#include "AnimationToolsNode_SpeedScale.h"
#include "Tools/Animation/ToolsGraph/AnimationToolsGraph_Compilation.h"
#include "Engine/Animation/Graph/Nodes/AnimationGraphNode_SpeedScale.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void Tools_SpeedScaleNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        Tools_GraphNode::Initialize( pParent );
        CreateOutputPin( "Result", ValueType::Bool, true );
        CreateInputPin( "Input", ValueType::Pose );
        CreateInputPin( "Scale", ValueType::Float );
    }

    NodeIndex Tools_SpeedScaleNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        SpeedScaleNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<SpeedScaleNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            auto pInputNode = GetConnectedInputNode<Tools_GraphNode>( 0 );
            if ( pInputNode != nullptr )
            {
                NodeIndex const compiledNodeIdx = pInputNode->Compile( context );
                if ( compiledNodeIdx != InvalidIndex )
                {
                    pSettings->m_childNodeIdx = compiledNodeIdx;
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

            pInputNode = GetConnectedInputNode<Tools_GraphNode>( 1 );
            if ( pInputNode != nullptr )
            {
                NodeIndex const compiledNodeIdx = pInputNode->Compile( context );
                if ( compiledNodeIdx != InvalidIndex )
                {
                    pSettings->m_scaleValueNodeIdx = compiledNodeIdx;
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

            pSettings->m_scaleLimits = m_scaleLimits;
            pSettings->m_blendTime = m_blendTime;
        }
        return pSettings->m_nodeIdx;
    }

    //-------------------------------------------------------------------------

    void Tools_VelocityBasedSpeedScaleNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        Tools_GraphNode::Initialize( pParent );
        CreateOutputPin( "Result", ValueType::Float, true );
        CreateInputPin( "Input", ValueType::Pose );
        CreateInputPin( "Scale", ValueType::Float );
    }

    NodeIndex Tools_VelocityBasedSpeedScaleNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        VelocityBasedSpeedScaleNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<VelocityBasedSpeedScaleNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            auto pInputNode = GetConnectedInputNode<Tools_GraphNode>( 0 );
            if ( pInputNode != nullptr )
            {
                NodeIndex const compiledNodeIdx = pInputNode->Compile( context );
                if ( compiledNodeIdx != InvalidIndex )
                {
                    pSettings->m_childNodeIdx = compiledNodeIdx;
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

            pInputNode = GetConnectedInputNode<Tools_GraphNode>( 1 );
            if ( pInputNode != nullptr )
            {
                NodeIndex const compiledNodeIdx = pInputNode->Compile( context );
                if ( compiledNodeIdx != InvalidIndex )
                {
                    pSettings->m_desiredVelocityValueNodeIdx = compiledNodeIdx;
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

            pSettings->m_blendTime = m_blendTime;
        }
        return pSettings->m_nodeIdx;
    }
}