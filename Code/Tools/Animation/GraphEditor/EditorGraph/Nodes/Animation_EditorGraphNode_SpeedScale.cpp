#include "Animation_EditorGraphNode_SpeedScale.h"
#include "Tools/Animation/GraphEditor/EditorGraph/Animation_EditorGraph_Compilation.h"
#include "Engine/Animation/Graph/Nodes/Animation_RuntimeGraphNode_SpeedScale.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void SpeedScaleEditorNode::Initialize( VisualGraph::BaseGraph* pParent )
    {
        EditorGraphNode::Initialize( pParent );
        CreateOutputPin( "Result", ValueType::Pose, true );
        CreateInputPin( "Input", ValueType::Pose );
        CreateInputPin( "Scale", ValueType::Float );
    }

    NodeIndex SpeedScaleEditorNode::Compile( EditorGraphCompilationContext& context ) const
    {
        SpeedScaleNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<SpeedScaleNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            auto pInputNode = GetConnectedInputNode<EditorGraphNode>( 0 );
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

            pInputNode = GetConnectedInputNode<EditorGraphNode>( 1 );
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

    void VelocityBasedSpeedScaleEditorNode::Initialize( VisualGraph::BaseGraph* pParent )
    {
        EditorGraphNode::Initialize( pParent );
        CreateOutputPin( "Result", ValueType::Float, true );
        CreateInputPin( "Input", ValueType::Pose );
        CreateInputPin( "Scale", ValueType::Float );
    }

    NodeIndex VelocityBasedSpeedScaleEditorNode::Compile( EditorGraphCompilationContext& context ) const
    {
        VelocityBasedSpeedScaleNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<VelocityBasedSpeedScaleNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            auto pInputNode = GetConnectedInputNode<EditorGraphNode>( 0 );
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

            pInputNode = GetConnectedInputNode<EditorGraphNode>( 1 );
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