#include "AnimationToolsNode_AnimationClip.h"
#include "Engine/Animation/Graph/Nodes/AnimationGraphNode_AnimationClip.h"
#include "Tools/Animation/GraphEditor/ToolsGraph/AnimationToolsGraph_Compilation.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void Tools_AnimationClipNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        Tools_DataSlotNode::Initialize( pParent );
        CreateOutputPin( "Pose", ValueType::Pose );
        CreateInputPin( "Play In Reverse", ValueType::Bool );
    }

    NodeIndex Tools_AnimationClipNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        AnimationClipNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<AnimationClipNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            auto pShouldPlayInReverseNodeNode = GetConnectedInputNode<Tools_GraphNode>( 0 );
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