#include "AnimationToolsNode_StateConditions.h"
#include "Tools/Animation/GraphEditor/ToolsGraph/AnimationToolsGraph_Compilation.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void Tools_StateCompletedConditionNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        Tools_GraphNode::Initialize( pParent );
        CreateOutputPin( "Result", ValueType::Bool, true );
    }

    NodeIndex Tools_StateCompletedConditionNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        StateCompletedConditionNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<StateCompletedConditionNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            pSettings->m_sourceStateNodeIdx = context.GetConduitSourceStateIndex();
            pSettings->m_transitionDuration = context.GetCompiledTransitionDuration();
            pSettings->m_transitionDurationOverrideNodeIdx = context.GetCompiledTransitionDurationOverrideIdx();
        }
        else // Encountered this node twice during compilation
        {
            context.LogError( this, "State Completed Conditions are only allowed to be connected to a single transition" );
            return InvalidIndex;
        }

        return pSettings->m_nodeIdx;
    }

    //-------------------------------------------------------------------------

    void Tools_TimeConditionNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        Tools_GraphNode::Initialize( pParent );
        CreateOutputPin( "Result", ValueType::Bool, true );
        CreateOutputPin( "Time Value (optional)", ValueType::Float, true );
    }

    NodeIndex Tools_TimeConditionNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        TimeConditionNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<TimeConditionNode>( this, pSettings );
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

            pSettings->m_sourceStateNodeIdx = context.GetConduitSourceStateIndex();
            pSettings->m_comparand = m_comparand;
            pSettings->m_type = m_type;
            pSettings->m_operator = m_operator;
        }
        return pSettings->m_nodeIdx;
    }
}