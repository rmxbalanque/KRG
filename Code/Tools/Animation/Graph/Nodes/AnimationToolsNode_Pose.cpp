#include "AnimationToolsNode_Pose.h"
#include "Engine/Animation/Graph/Nodes/AnimationGraphNode_Pose.h"
#include "Tools/Animation/Graph/AnimationGraphTools_Compilation.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    ZeroPoseToolsNode::ZeroPoseToolsNode()
        : ToolsNode()
    {
        CreateOutputPin( "Pose", NodeValueType::Pose );
    }

    NodeIndex ZeroPoseToolsNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        ZeroPoseNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<ZeroPoseNode>( this, pSettings );
        return pSettings->m_nodeIdx;
    }

    //-------------------------------------------------------------------------

    ReferencePoseToolsNode::ReferencePoseToolsNode()
        : ToolsNode()
    {
        CreateOutputPin( "Pose", NodeValueType::Pose );
    }

    NodeIndex ReferencePoseToolsNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        ReferencePoseNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<ReferencePoseNode>( this, pSettings );
        return pSettings->m_nodeIdx;
    }

    //-------------------------------------------------------------------------

    AnimationPoseToolsNode::AnimationPoseToolsNode()
        : ToolsNode()
    {
        CreateOutputPin( "Pose", NodeValueType::Pose );
        CreateInputPin( "Time", NodeValueType::Float );
    }

    NodeIndex AnimationPoseToolsNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        AnimationPoseNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<AnimationPoseNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            auto pTimeParameterNode = GetConnectedInputNode<ToolsNode>( 0 );
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

            pSettings->m_sourceIndex = context.GetAnimationClipSourceIndex( GetID() );
            pSettings->m_inputTimeRemapRange = m_inputTimeRemapRange;
        }
        return pSettings->m_nodeIdx;
    }
}