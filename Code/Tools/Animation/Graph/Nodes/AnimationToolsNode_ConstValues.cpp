#include "AnimationToolsNode_ConstValues.h"
#include "Engine/Animation/Graph/Nodes/AnimationGraphNode_ConstValues.h"
#include "Tools/Animation/Graph/AnimationGraphTools_Compilation.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    ConstBoolToolsNode::ConstBoolToolsNode()
        : ToolsNode()
    {
        CreateOutputPin( "Value", NodeValueType::Bool, true );
    }

    NodeIndex ConstBoolToolsNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        ConstBoolNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<ConstBoolNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            pSettings->m_value = m_value;
        }
        return pSettings->m_nodeIdx;
    }

    void ConstBoolToolsNode::DrawExtraControls( GraphEditor::DrawingContext const& ctx )
    {
        ImGui::Text( m_value ? "True" : "False" );
    }

    //-------------------------------------------------------------------------

    ConstIDToolsNode::ConstIDToolsNode()
        : ToolsNode()
    {
        CreateOutputPin( "Value", NodeValueType::ID, true );
    }

    NodeIndex ConstIDToolsNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        ConstIDNode::Settings* pSettings = nullptr;
        if ( context.GetSettings<ConstIDNode>( this, pSettings ) == NodeCompilationState::NeedCompilation )
        {
            pSettings->m_value = m_value;
        }
        return pSettings->m_nodeIdx;
    }

    void ConstIDToolsNode::DrawExtraControls( GraphEditor::DrawingContext const& ctx )
    {
        ImGui::Text( m_value.c_str() );
    }

    //-------------------------------------------------------------------------

    ConstIntToolsNode::ConstIntToolsNode()
        : ToolsNode()
    {
        CreateOutputPin( "Value", NodeValueType::Int, true );
    }

    NodeIndex ConstIntToolsNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        ConstIntNode::Settings* pSettings = nullptr;
        if ( context.GetSettings<ConstIntNode>( this, pSettings ) == NodeCompilationState::NeedCompilation )
        {
            pSettings->m_value = m_value;
        }
        return pSettings->m_nodeIdx;
    }

    void ConstIntToolsNode::DrawExtraControls( GraphEditor::DrawingContext const& ctx )
    {
        ImGui::Text( "%d", m_value );
    }

    //-------------------------------------------------------------------------

    ConstFloatToolsNode::ConstFloatToolsNode()
        : ToolsNode()
    {
        CreateOutputPin( "Value", NodeValueType::Float, true );
    }

    NodeIndex ConstFloatToolsNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        ConstFloatNode::Settings* pSettings = nullptr;
        if ( context.GetSettings<ConstFloatNode>( this, pSettings ) == NodeCompilationState::NeedCompilation )
        {
            pSettings->m_value = m_value;
        }
        return pSettings->m_nodeIdx;
    }

    void ConstFloatToolsNode::DrawExtraControls( GraphEditor::DrawingContext const& ctx )
    {
        ImGui::Text( "%.3f", m_value );
    }

    //-------------------------------------------------------------------------

    ConstVectorToolsNode::ConstVectorToolsNode()
        : ToolsNode()
    {
        CreateOutputPin( "Value", NodeValueType::Vector, true );
    }

    NodeIndex ConstVectorToolsNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        ConstVectorNode::Settings* pSettings = nullptr;
        if ( context.GetSettings<ConstVectorNode>( this, pSettings ) == NodeCompilationState::NeedCompilation )
        {
            pSettings->m_value = m_value;
        }
        return pSettings->m_nodeIdx;
    }

    void ConstVectorToolsNode::DrawExtraControls( GraphEditor::DrawingContext const& ctx )
    {
        ImGui::Text( "X: %.2f, Y: %.2f, Z: %.2f, W: %.2f", m_value.m_x, m_value.m_y, m_value.m_z, m_value.m_w );
    }

    //-------------------------------------------------------------------------

    ConstTargetToolsNode::ConstTargetToolsNode()
        : ToolsNode()
    {
        CreateOutputPin( "Value", NodeValueType::Target, true );
    }

    NodeIndex ConstTargetToolsNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        ConstTargetNode::Settings* pSettings = nullptr;
        if ( context.GetSettings<ConstTargetNode>( this, pSettings ) == NodeCompilationState::NeedCompilation )
        {
            pSettings->m_value = Target( m_value );
        }
        return pSettings->m_nodeIdx;
    }

    void ConstTargetToolsNode::DrawExtraControls( GraphEditor::DrawingContext const& ctx )
    {
        ImGui::Text( "Transform - TODO" );
    }
}