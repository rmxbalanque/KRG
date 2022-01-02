#include "Animation_EditorGraphNode_Floats.h"
#include "Tools/Animation/GraphEditor/EditorGraph/Animation_EditorGraph_Compilation.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::GraphNodes
{
    void FloatRemapEditorNode::Initialize( VisualGraph::BaseGraph* pParent )
    {
        EditorGraphNode::Initialize( pParent );
        CreateOutputPin( "Result", GraphValueType::Float, true );
        CreateInputPin( "Float", GraphValueType::Float );
    }

    GraphNodeIndex FloatRemapEditorNode::Compile( EditorGraphCompilationContext& context ) const
    {
        FloatRemapNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<FloatRemapNode>( this, pSettings );
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

            pSettings->m_inputRange = m_inputRange;
            pSettings->m_outputRange = m_outputRange;
        }
        return pSettings->m_nodeIdx;
    }

    void FloatRemapEditorNode::DrawInfoText( VisualGraph::DrawContext const& ctx )
    {
        ImGui::Text( "[%.2f, %.2f] to [%.2f, %.2f]", m_inputRange.m_start, m_inputRange.m_end, m_outputRange.m_start, m_outputRange.m_end );
    }

    //-------------------------------------------------------------------------

    void FloatClampEditorNode::Initialize( VisualGraph::BaseGraph* pParent )
    {
        EditorGraphNode::Initialize( pParent );
        CreateOutputPin( "Result", GraphValueType::Float, true );
        CreateInputPin( "Float", GraphValueType::Float );
    }

    GraphNodeIndex FloatClampEditorNode::Compile( EditorGraphCompilationContext& context ) const
    {
        FloatClampNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<FloatClampNode>( this, pSettings );
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

            pSettings->m_clampRange = m_clampRange;
        }
        return pSettings->m_nodeIdx;
    }

    //-------------------------------------------------------------------------

    void FloatAbsEditorNode::Initialize( VisualGraph::BaseGraph* pParent )
    {
        EditorGraphNode::Initialize( pParent );
        CreateOutputPin( "Result", GraphValueType::Float, true );
        CreateInputPin( "Float", GraphValueType::Float );
    }

    GraphNodeIndex FloatAbsEditorNode::Compile( EditorGraphCompilationContext& context ) const
    {
        FloatAbsNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<FloatAbsNode>( this, pSettings );
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

    void FloatEaseEditorNode::Initialize( VisualGraph::BaseGraph* pParent )
    {
        EditorGraphNode::Initialize( pParent );
        CreateOutputPin( "Result", GraphValueType::Float, true );
        CreateInputPin( "Float", GraphValueType::Float );
    }

    GraphNodeIndex FloatEaseEditorNode::Compile( EditorGraphCompilationContext& context ) const
    {
        FloatEaseNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<FloatEaseNode>( this, pSettings );
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

            pSettings->m_easingType = m_easingType;
            pSettings->m_easeTime = m_easeTime;
        }
        return pSettings->m_nodeIdx;
    }

    //-------------------------------------------------------------------------

    void FloatMathEditorNode::Initialize( VisualGraph::BaseGraph* pParent )
    {
        EditorGraphNode::Initialize( pParent );
        CreateOutputPin( "Result", GraphValueType::Float, true );
        CreateInputPin( "A", GraphValueType::Float );
        CreateInputPin( "B (Optional)", GraphValueType::Float );
    }

    GraphNodeIndex FloatMathEditorNode::Compile( EditorGraphCompilationContext& context ) const
    {
        FloatMathNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<FloatMathNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            auto pInputNodeA = GetConnectedInputNode<EditorGraphNode>( 0 );
            if ( pInputNodeA != nullptr )
            {
                GraphNodeIndex const compiledNodeIdx = pInputNodeA->Compile( context );
                if ( compiledNodeIdx != InvalidIndex )
                {
                    pSettings->m_inputValueNodeIdxA = compiledNodeIdx;
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

            auto pInputNodeB = GetConnectedInputNode<EditorGraphNode>( 1 );
            if ( pInputNodeB != nullptr )
            {
                GraphNodeIndex const compiledNodeIdx = pInputNodeB->Compile( context );
                if ( compiledNodeIdx != InvalidIndex )
                {
                    pSettings->m_inputValueNodeIdxB = compiledNodeIdx;
                }
                else
                {
                    return false;
                }
            }

            //-------------------------------------------------------------------------

            pSettings->m_returnAbsoluteResult = m_returnAbsoluteResult;
            pSettings->m_operator = m_operator;
            pSettings->m_valueB = m_valueB;
        }
        return pSettings->m_nodeIdx;
    }

    //-------------------------------------------------------------------------

    void FloatComparisonEditorNode::Initialize( VisualGraph::BaseGraph* pParent )
    {
        EditorGraphNode::Initialize( pParent );
        CreateOutputPin( "Result", GraphValueType::Bool, true );
        CreateInputPin( "Float", GraphValueType::Float );
    }

    GraphNodeIndex FloatComparisonEditorNode::Compile( EditorGraphCompilationContext& context ) const
    {
        FloatComparisonNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<FloatComparisonNode>( this, pSettings );
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

            pSettings->m_comparison = m_comparison;
            pSettings->m_epsilon = m_epsilon;
            pSettings->m_comparisonValue = m_comparisonValue;
        }
        return pSettings->m_nodeIdx;
    }

    void FloatComparisonEditorNode::DrawInfoText( VisualGraph::DrawContext const& ctx )
    {
        static char const* comparisionStr[] =
        {
            ">=",
            "<=",
            "~=",
            ">",
            "<",
        };

        ImGui::Text( "%s %.2f", comparisionStr[(int32)m_comparison], m_comparisonValue );
    }

    //-------------------------------------------------------------------------

    void FloatRangeComparisonEditorNode::Initialize( VisualGraph::BaseGraph* pParent )
    {
        EditorGraphNode::Initialize( pParent );
        CreateOutputPin( "Result", GraphValueType::Bool, true );
        CreateInputPin( "Float", GraphValueType::Float );
    }

    GraphNodeIndex FloatRangeComparisonEditorNode::Compile( EditorGraphCompilationContext& context ) const
    {
        FloatRangeComparisonNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<FloatRangeComparisonNode>( this, pSettings );
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

            pSettings->m_range = m_range;
            pSettings->m_isInclusiveCheck = m_isInclusiveCheck;
        }
        return pSettings->m_nodeIdx;
    }

    void FloatRangeComparisonEditorNode::DrawInfoText( VisualGraph::DrawContext const& ctx )
    {
        if ( m_isInclusiveCheck )
        {
            ImGui::Text( "%.2f <= X <= %.2f", m_range.m_start, m_range.m_end );
        }
        else
        {
            ImGui::Text( "%.2f < X < %.2f", m_range.m_start, m_range.m_end );
        }
    }

    //-------------------------------------------------------------------------

    void FloatSwitchEditorNode::Initialize( VisualGraph::BaseGraph* pParent )
    {
        EditorGraphNode::Initialize( pParent );
        CreateOutputPin( "Result", GraphValueType::Float, true );
        CreateInputPin( "Bool", GraphValueType::Bool );
        CreateInputPin( "If True", GraphValueType::Float );
        CreateInputPin( "If False", GraphValueType::Float );
    }

    GraphNodeIndex FloatSwitchEditorNode::Compile( EditorGraphCompilationContext& context ) const
    {
        FloatSwitchNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<FloatSwitchNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            auto pInputNode = GetConnectedInputNode<EditorGraphNode>( 0 );
            if ( pInputNode != nullptr )
            {
                GraphNodeIndex const compiledNodeIdx = pInputNode->Compile( context );
                if ( compiledNodeIdx != InvalidIndex )
                {
                    pSettings->m_switchValueNodeIdx = compiledNodeIdx;
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
                    pSettings->m_trueValueNodeIdx = compiledNodeIdx;
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

            pInputNode = GetConnectedInputNode<EditorGraphNode>( 2 );
            if ( pInputNode != nullptr )
            {
                GraphNodeIndex const compiledNodeIdx = pInputNode->Compile( context );
                if ( compiledNodeIdx != InvalidIndex )
                {
                    pSettings->m_falseValueNodeIdx = compiledNodeIdx;
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
}