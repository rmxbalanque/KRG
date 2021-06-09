#include "AnimationToolsNode_AnimationClip.h"
#include "Engine/Animation/Graph/Nodes/AnimationGraphNode_AnimationClip.h"
#include "Tools/Animation/Graph/AnimationGraphTools_Compilation.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    AnimationClipToolsNode::AnimationClipToolsNode()
        : ToolsNode()
    {
        CreateOutputPin( "Pose", NodeValueType::Pose );
        CreateInputPin( "Play In Reverse", NodeValueType::Bool );
    }

    NodeIndex AnimationClipToolsNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        AnimationClipNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<AnimationClipNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            auto pShouldPlayInReverseNodeNode = GetConnectedInputNode<ToolsNode>( 0 );
            if ( pShouldPlayInReverseNodeNode != nullptr )
            {
                auto compiledNodeIdx = pShouldPlayInReverseNodeNode->Compile( context );
                if ( compiledNodeIdx != InvalidIndex )
                {
                    pSettings->m_playInReverseValueNodeIdx = compiledNodeIdx;
                }
                else
                {
                    return InvalidIndex;
                }
            }

            //-------------------------------------------------------------------------

            pSettings->m_sourceIndex = context.GetAnimationClipSourceIndex( GetID() );
            pSettings->m_shouldSampleRootMotion = m_shouldSampleRootMotion;
            pSettings->m_allowLooping = m_allowLooping;
        }
        return pSettings->m_nodeIdx;
    }

}