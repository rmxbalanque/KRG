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

            pSettings->m_isWorldSpaceTarget = m_isWorldSpaceTarget;
            pSettings->m_infoType = m_infoType;
        }
        return pSettings->m_nodeIdx;
    }

    void Tools_TargetInfoNode::DrawInfoText( GraphEditor::DrawingContext const& ctx )
    {
        InlineString<150> infoText = "Expected Space: ";
        
        if ( m_isWorldSpaceTarget )
        {
            infoText += "World";
        }
        else
        {
            infoText += "Character";
        }

        ImGui::Text( infoText.c_str() );
        infoText.clear();

        //-------------------------------------------------------------------------

        switch ( m_infoType )
        {
            case TargetInfoNode::Info::AngleHorizontal:
            {
                infoText = "Horizontal Angle";
            }
            break;

            case TargetInfoNode::Info::AngleVertical:
            {
                infoText = "Vertical Angle";
            }
            break;

            case TargetInfoNode::Info::Distance:
            {
                infoText = "Distance";
            }
            break;

            case TargetInfoNode::Info::DistanceHorizontalOnly:
            {
                infoText = "Horizontal Distance";
            }
            break;

            case TargetInfoNode::Info::DistanceVerticalOnly:
            {
                infoText = "Vertical Distance";
            }
            break;

            case TargetInfoNode::Info::DeltaOrientationX:
            {
                infoText = "Rotation Angle X";
            }
            break;

            case TargetInfoNode::Info::DeltaOrientationY:
            {
                infoText = "Rotation Angle Y";
            }
            break;

            case TargetInfoNode::Info::DeltaOrientationZ:
            {
                infoText = "Rotation Angle Z";
            }
            break;
        }

        ImGui::Text( infoText.c_str() );
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