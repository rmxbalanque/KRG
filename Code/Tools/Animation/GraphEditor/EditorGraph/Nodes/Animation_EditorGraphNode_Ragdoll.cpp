#include "Animation_EditorGraphNode_Ragdoll.h"
#include "Tools/Animation/GraphEditor/EditorGraph/Animation_EditorGraph_Compilation.h"
#include "Engine/Animation/Graph/Nodes/Animation_RuntimeGraphNode_Ragdoll.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::GraphNodes
{
    void PoweredRagdollEditorNode::Initialize( VisualGraph::BaseGraph* pParent )
    {
        EditorGraphNode::Initialize( pParent );
        CreateOutputPin( "Result", GraphValueType::Pose, true );
        CreateInputPin( "Input", GraphValueType::Pose );
        CreateInputPin( "Physics Blend Weight", GraphValueType::Float );
    }

    GraphNodeIndex PoweredRagdollEditorNode::Compile( EditorGraphCompilationContext& context ) const
    {
        PoweredRagdollNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<PoweredRagdollNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            auto pInputNode = GetConnectedInputNode<EditorGraphNode>( 0 );
            if ( pInputNode != nullptr )
            {
                GraphNodeIndex const compiledNodeIdx = pInputNode->Compile( context );
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
                GraphNodeIndex const compiledNodeIdx = pInputNode->Compile( context );
                if ( compiledNodeIdx != InvalidIndex )
                {
                    pSettings->m_physicsBlendWeightNodeIdx = compiledNodeIdx;
                }
                else
                {
                    context.LogError( this, "Failed to compile physics blend weight node!" );
                    return false;
                }
            }

            //-------------------------------------------------------------------------

            pSettings->m_dataSlotIdx = context.RegisterSlotNode( GetID() );
            pSettings->m_profileID = m_profileID;
            pSettings->m_physicsBlendWeight = m_physicsBlendWeight;
            pSettings->m_isGravityEnabled = m_isGravityEnabled;
        }
        return pSettings->m_nodeIdx;
    }
}