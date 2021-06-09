#include "AnimationToolsNode_ConstValues.h"
#include "Engine/Animation/Graph/Nodes/AnimationGraphNode_ConstValues.h"
#include "Tools/Animation/Graph/AnimationGraphTools_Compilation.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    ConstBoolToolsNode::ConstBoolToolsNode()
        : ToolsNode()
    {
        CreateOutputPin( "Bool", NodeValueType::Bool, true );
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

    //-------------------------------------------------------------------------

    ConstIDToolsNode::ConstIDToolsNode()
        : ToolsNode()
    {
        CreateOutputPin( "ID", NodeValueType::ID, true );
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

    //-------------------------------------------------------------------------

    ConstIntToolsNode::ConstIntToolsNode()
        : ToolsNode()
    {
        CreateOutputPin( "Int", NodeValueType::Int, true );
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

    //-------------------------------------------------------------------------

    ConstFloatToolsNode::ConstFloatToolsNode()
        : ToolsNode()
    {
        CreateOutputPin( "Float", NodeValueType::Float, true );
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

    //-------------------------------------------------------------------------

    ConstVectorToolsNode::ConstVectorToolsNode()
        : ToolsNode()
    {
        CreateOutputPin( "Vector", NodeValueType::Vector, true );
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

    //-------------------------------------------------------------------------

    ConstTargetToolsNode::ConstTargetToolsNode()
        : ToolsNode()
    {
        CreateOutputPin( "Target", NodeValueType::Target, true );
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
}