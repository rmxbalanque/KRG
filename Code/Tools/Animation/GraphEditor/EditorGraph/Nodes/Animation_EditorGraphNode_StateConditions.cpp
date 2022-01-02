#include "Animation_EditorGraphNode_StateConditions.h"
#include "Tools/Animation/GraphEditor/EditorGraph/Animation_EditorGraph_Compilation.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::GraphNodes
{
    void StateCompletedConditionEditorNode::Initialize( VisualGraph::BaseGraph* pParent )
    {
        EditorGraphNode::Initialize( pParent );
        CreateOutputPin( "Result", GraphValueType::Bool, true );
    }

    GraphNodeIndex StateCompletedConditionEditorNode::Compile( EditorGraphCompilationContext& context ) const
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

    void TimeConditionEditorNode::Initialize( VisualGraph::BaseGraph* pParent )
    {
        EditorGraphNode::Initialize( pParent );
        CreateOutputPin( "Result", GraphValueType::Bool, true );
        CreateOutputPin( "Time Value (optional)", GraphValueType::Float, true );
    }

    GraphNodeIndex TimeConditionEditorNode::Compile( EditorGraphCompilationContext& context ) const
    {
        TimeConditionNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<TimeConditionNode>( this, pSettings );
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

            pSettings->m_sourceStateNodeIdx = context.GetConduitSourceStateIndex();
            pSettings->m_comparand = m_comparand;
            pSettings->m_type = m_type;
            pSettings->m_operator = m_operator;
        }
        return pSettings->m_nodeIdx;
    }
}