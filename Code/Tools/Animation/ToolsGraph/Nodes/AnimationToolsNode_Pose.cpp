#include "AnimationToolsNode_Pose.h"
#include "Engine/Animation/Graph/Nodes/AnimationGraphNode_Pose.h"
#include "Tools/Animation/ToolsGraph/AnimationToolsGraph_Compilation.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void Tools_ZeroPoseNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        Tools_GraphNode::Initialize( pParent );
        CreateOutputPin( "Pose", ValueType::Pose );
    }

    NodeIndex Tools_ZeroPoseNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        ZeroPoseNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<ZeroPoseNode>( this, pSettings );
        return pSettings->m_nodeIdx;
    }

    //-------------------------------------------------------------------------

    void Tools_ReferencePoseNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        Tools_GraphNode::Initialize( pParent );
        CreateOutputPin( "Pose", ValueType::Pose );
    }

    NodeIndex Tools_ReferencePoseNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        ReferencePoseNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<ReferencePoseNode>( this, pSettings );
        return pSettings->m_nodeIdx;
    }

    //-------------------------------------------------------------------------

    void Tools_AnimationPoseNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        Tools_GraphNode::Initialize( pParent );
        CreateOutputPin( "Pose", ValueType::Pose );
        CreateInputPin( "Time", ValueType::Float );
    }

    NodeIndex Tools_AnimationPoseNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        AnimationPoseNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<AnimationPoseNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            auto pTimeParameterNode = GetConnectedInputNode<Tools_GraphNode>( 0 );
            if ( pTimeParameterNode != nullptr )
            {
                auto compiledNodeIdx = pTimeParameterNode->Compile( context );
                if ( compiledNodeIdx != InvalidIndex )
                {
                    pSettings->m_poseTimeValueNodeIdx = compiledNodeIdx;
                }
                else
                {
                    return InvalidIndex;
                }
            }
            else
            {
                context.LogError( this, "Disconnected time parameter pin on animation pose node!" );
                return InvalidIndex;
            }

            //-------------------------------------------------------------------------

            pSettings->m_dataSlotIndex = context.RegisterSlotNode( GetID() );
            pSettings->m_inputTimeRemapRange = m_inputTimeRemapRange;
        }
        return pSettings->m_nodeIdx;
    }
}