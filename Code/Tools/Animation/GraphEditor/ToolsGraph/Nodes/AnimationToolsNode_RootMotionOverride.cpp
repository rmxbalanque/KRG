#include "AnimationToolsNode_RootMotionOverride.h"
#include "Tools/Animation/GraphEditor/ToolsGraph/AnimationToolsGraph_Compilation.h"
#include "Engine/Animation/Graph/Nodes/AnimationGraphNode_RootMotionOverride.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void Tools_RootMotionOverrideNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        Tools_GraphNode::Initialize( pParent );
        CreateOutputPin( "Result", ValueType::Pose, true );
        CreateInputPin( "Input", ValueType::Pose );
        CreateInputPin( "Desired Heading Velocity (Character)", ValueType::Vector );
        CreateInputPin( "Desired Facing Direction (Character)", ValueType::Vector );
        CreateInputPin( "Linear Velocity Limit (Optional)", ValueType::Float );
        CreateInputPin( "Angular Velocity Limit (Optional)", ValueType::Float );
    }

    NodeIndex Tools_RootMotionOverrideNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        RootMotionOverrideNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<RootMotionOverrideNode>( this, pSettings );
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
                    return InvalidIndex;
                }
            }
            else
            {
                context.LogError( this, "Disconnected input pin!" );
                return InvalidIndex;
            }

            //-------------------------------------------------------------------------

            pInputNode = GetConnectedInputNode<Tools_GraphNode>( 1 );
            if ( pInputNode != nullptr )
            {
                NodeIndex const compiledNodeIdx = pInputNode->Compile( context );
                if ( compiledNodeIdx != InvalidIndex )
                {
                    pSettings->m_desiredHeadingVelocityNodeIdx = compiledNodeIdx;
                }
                else
                {
                    return InvalidIndex;
                }
            }
            else
            {
                context.LogError( this, "Disconnected Heading Velocity pin!" );
                return InvalidIndex;
            }

            //-------------------------------------------------------------------------

            pInputNode = GetConnectedInputNode<Tools_GraphNode>( 2 );
            if ( pInputNode != nullptr )
            {
                NodeIndex const compiledNodeIdx = pInputNode->Compile( context );
                if ( compiledNodeIdx != InvalidIndex )
                {
                    pSettings->m_desiredFacingDirectionNodeIdx = compiledNodeIdx;
                }
                else
                {
                    return InvalidIndex;
                }
            }
            else
            {
                context.LogError( this, "Disconnected Facing Direction pin!" );
                return InvalidIndex;
            }

            //-------------------------------------------------------------------------

            pInputNode = GetConnectedInputNode<Tools_GraphNode>( 3 );
            if ( pInputNode != nullptr )
            {
                NodeIndex const compiledNodeIdx = pInputNode->Compile( context );
                if ( compiledNodeIdx != InvalidIndex )
                {
                    pSettings->m_linearVelocityLimitNodeIdx = compiledNodeIdx;
                }
                else
                {
                    return InvalidIndex;
                }
            }

            //-------------------------------------------------------------------------

            pInputNode = GetConnectedInputNode<Tools_GraphNode>( 4 );
            if ( pInputNode != nullptr )
            {
                NodeIndex const compiledNodeIdx = pInputNode->Compile( context );
                if ( compiledNodeIdx != InvalidIndex )
                {
                    pSettings->m_angularVelocityLimitNodeIdx = compiledNodeIdx;
                }
                else
                {
                    return InvalidIndex;
                }
            }

            //-------------------------------------------------------------------------

            pSettings->m_maxLinearVelocity = m_maxLinearVelocity;
            pSettings->m_maxAngularVelocity = m_maxAngularVelocity;

            pSettings->m_overrideFlags.SetFlag( RootMotionOverrideNode::OverrideFlags::HeadingX, m_overrideHeadingX );
            pSettings->m_overrideFlags.SetFlag( RootMotionOverrideNode::OverrideFlags::HeadingY, m_overrideHeadingY );
            pSettings->m_overrideFlags.SetFlag( RootMotionOverrideNode::OverrideFlags::HeadingZ, m_overrideHeadingZ );
            pSettings->m_overrideFlags.SetFlag( RootMotionOverrideNode::OverrideFlags::FacingX, m_overrideFacingX );
            pSettings->m_overrideFlags.SetFlag( RootMotionOverrideNode::OverrideFlags::FacingY, m_overrideFacingY );
            pSettings->m_overrideFlags.SetFlag( RootMotionOverrideNode::OverrideFlags::FacingZ, m_overrideFacingZ );
        }
        return pSettings->m_nodeIdx;
    }
}