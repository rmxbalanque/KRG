#include "AnimationToolsNode_AnimationClip.h"
#include "Engine/Animation/Graph/Nodes/AnimationGraphNode_AnimationClip.h"
#include "Tools/Animation/Graph/AnimationGraphTools_Compilation.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void AnimationClipToolsNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        DataSlotToolsNode::Initialize( pParent );
        CreateOutputPin( "Pose", NodeValueType::Pose );
        CreateInputPin( "Play In Reverse", NodeValueType::Bool );
    }

    NodeIndex AnimationClipToolsNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        AnimationClipNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<AnimationClipNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            auto pShouldPlayInReverseNodeNode = GetConnectedInputNode<FlowToolsNode>( 0 );
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

            pSettings->m_dataSlotIdx = context.RegisterSlotNode( GetID() );
            pSettings->m_sampleRootMotion = m_sampleRootMotion;
            pSettings->m_allowLooping = m_allowLooping;
        }
        return pSettings->m_nodeIdx;
    }

}