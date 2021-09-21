#include "AnimationToolsNode_ConstValues.h"
#include "Engine/Animation/Graph/Nodes/AnimationGraphNode_ConstValues.h"
#include "Tools/Animation/ToolsGraph/AnimationToolsGraph_Compilation.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void Tools_ConstBoolNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        Tools_GraphNode::Initialize( pParent );
        CreateOutputPin( "Value", ValueType::Bool, true );
    }

    NodeIndex Tools_ConstBoolNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        ConstBoolNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<ConstBoolNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            pSettings->m_value = m_value;
        }
        return pSettings->m_nodeIdx;
    }

    void Tools_ConstBoolNode::DrawExtraControls( GraphEditor::DrawingContext const& ctx )
    {
        ImGui::Text( m_value ? "True" : "False" );
    }

    //-------------------------------------------------------------------------

    void Tools_ConstIDNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        Tools_GraphNode::Initialize( pParent );
        CreateOutputPin( "Value", ValueType::ID, true );
    }

    NodeIndex Tools_ConstIDNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        ConstIDNode::Settings* pSettings = nullptr;
        if ( context.GetSettings<ConstIDNode>( this, pSettings ) == NodeCompilationState::NeedCompilation )
        {
            pSettings->m_value = m_value;
        }
        return pSettings->m_nodeIdx;
    }

    void Tools_ConstIDNode::DrawExtraControls( GraphEditor::DrawingContext const& ctx )
    {
        ImGui::Text( m_value.c_str() );
    }

    //-------------------------------------------------------------------------

    void Tools_ConstIntNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        Tools_GraphNode::Initialize( pParent );
        CreateOutputPin( "Value", ValueType::Int, true );
    }

    NodeIndex Tools_ConstIntNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        ConstIntNode::Settings* pSettings = nullptr;
        if ( context.GetSettings<ConstIntNode>( this, pSettings ) == NodeCompilationState::NeedCompilation )
        {
            pSettings->m_value = m_value;
        }
        return pSettings->m_nodeIdx;
    }

    void Tools_ConstIntNode::DrawExtraControls( GraphEditor::DrawingContext const& ctx )
    {
        ImGui::Text( "%d", m_value );
    }

    //-------------------------------------------------------------------------

    void Tools_ConstFloatNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        Tools_GraphNode::Initialize( pParent );
        CreateOutputPin( "Value", ValueType::Float, true );
    }

    NodeIndex Tools_ConstFloatNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        ConstFloatNode::Settings* pSettings = nullptr;
        if ( context.GetSettings<ConstFloatNode>( this, pSettings ) == NodeCompilationState::NeedCompilation )
        {
            pSettings->m_value = m_value;
        }
        return pSettings->m_nodeIdx;
    }

    void Tools_ConstFloatNode::DrawExtraControls( GraphEditor::DrawingContext const& ctx )
    {
        ImGui::Text( "%.3f", m_value );
    }

    //-------------------------------------------------------------------------

    void Tools_ConstVectorNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        Tools_GraphNode::Initialize( pParent );
        CreateOutputPin( "Value", ValueType::Vector, true );
    }

    NodeIndex Tools_ConstVectorNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        ConstVectorNode::Settings* pSettings = nullptr;
        if ( context.GetSettings<ConstVectorNode>( this, pSettings ) == NodeCompilationState::NeedCompilation )
        {
            pSettings->m_value = m_value;
        }
        return pSettings->m_nodeIdx;
    }

    void Tools_ConstVectorNode::DrawExtraControls( GraphEditor::DrawingContext const& ctx )
    {
        ImGui::Text( "X: %.2f, Y: %.2f, Z: %.2f, W: %.2f", m_value.m_x, m_value.m_y, m_value.m_z, m_value.m_w );
    }

    //-------------------------------------------------------------------------

    void Tools_ConstTargetNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        Tools_GraphNode::Initialize( pParent );
        CreateOutputPin( "Value", ValueType::Target, true );
    }

    NodeIndex Tools_ConstTargetNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        ConstTargetNode::Settings* pSettings = nullptr;
        if ( context.GetSettings<ConstTargetNode>( this, pSettings ) == NodeCompilationState::NeedCompilation )
        {
            pSettings->m_value = Target( m_value );
        }
        return pSettings->m_nodeIdx;
    }

    void Tools_ConstTargetNode::DrawExtraControls( GraphEditor::DrawingContext const& ctx )
    {
        ImGui::Text( "Transform - TODO" );
    }
}