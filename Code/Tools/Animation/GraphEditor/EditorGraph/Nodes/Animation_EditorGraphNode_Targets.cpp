#include "Animation_EditorGraphNode_Targets.h"
#include "Tools/Animation/GraphEditor/EditorGraph/Animation_EditorGraph_Compilation.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::GraphNodes
{
    void IsTargetSetEditorNode::Initialize( VisualGraph::BaseGraph* pParent )
    {
        EditorGraphNode::Initialize( pParent );
        CreateOutputPin( "Result", GraphValueType::Bool, true );
        CreateInputPin( "Target", GraphValueType::Target );
    }

    GraphNodeIndex IsTargetSetEditorNode::Compile( EditorGraphCompilationContext& context ) const
    {
        IsTargetSetNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<IsTargetSetNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            auto pInputNode = GetConnectedInputNode<EditorGraphNode>( 0 );
            if ( pInputNode != nullptr )
            {
                GraphNodeIndex const compiledNodeIdx = pInputNode->Compile( context );
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

    void TargetInfoEditorNode::Initialize( VisualGraph::BaseGraph* pParent )
    {
        EditorGraphNode::Initialize( pParent );
        CreateOutputPin( "Result", GraphValueType::Float, true );
        CreateInputPin( "Target", GraphValueType::Target );
    }

    GraphNodeIndex TargetInfoEditorNode::Compile( EditorGraphCompilationContext& context ) const
    {
        TargetInfoNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<TargetInfoNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            auto pInputNode = GetConnectedInputNode<EditorGraphNode>( 0 );
            if ( pInputNode != nullptr )
            {
                GraphNodeIndex const compiledNodeIdx = pInputNode->Compile( context );
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

    void TargetInfoEditorNode::DrawInfoText( VisualGraph::DrawContext const& ctx )
    {
        InlineString infoText;
        
        if ( m_isWorldSpaceTarget )
        {
            infoText += "World Space - ";
        }
        else
        {
            infoText += "Character Space - ";
        }

        //-------------------------------------------------------------------------

        switch ( m_infoType )
        {
            case TargetInfoNode::Info::AngleHorizontal:
            {
                infoText += "Horizontal Angle";
            }
            break;

            case TargetInfoNode::Info::AngleVertical:
            {
                infoText += "Vertical Angle";
            }
            break;

            case TargetInfoNode::Info::Distance:
            {
                infoText += "Distance";
            }
            break;

            case TargetInfoNode::Info::DistanceHorizontalOnly:
            {
                infoText += "Horizontal Distance";
            }
            break;

            case TargetInfoNode::Info::DistanceVerticalOnly:
            {
                infoText += "Vertical Distance";
            }
            break;

            case TargetInfoNode::Info::DeltaOrientationX:
            {
                infoText += "Rotation Angle X";
            }
            break;

            case TargetInfoNode::Info::DeltaOrientationY:
            {
                infoText += "Rotation Angle Y";
            }
            break;

            case TargetInfoNode::Info::DeltaOrientationZ:
            {
                infoText += "Rotation Angle Z";
            }
            break;
        }

        ImGui::Text( infoText.c_str() );
    }

    //-------------------------------------------------------------------------

    void TargetOffsetEditorNode::Initialize( VisualGraph::BaseGraph* pParent )
    {
        EditorGraphNode::Initialize( pParent );
        CreateOutputPin( "Result", GraphValueType::Target, true );
        CreateInputPin( "Target", GraphValueType::Target );
    }

    GraphNodeIndex TargetOffsetEditorNode::Compile( EditorGraphCompilationContext& context ) const
    {
        TargetOffsetNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<TargetOffsetNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            auto pInputNode = GetConnectedInputNode<EditorGraphNode>( 0 );
            if ( pInputNode != nullptr )
            {
                GraphNodeIndex const compiledNodeIdx = pInputNode->Compile( context );
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