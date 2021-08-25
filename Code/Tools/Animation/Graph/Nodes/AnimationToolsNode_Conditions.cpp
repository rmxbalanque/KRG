#include "AnimationToolsNode_Conditions.h"
#include "Engine/Animation/Graph/Nodes/AnimationGraphNode_BooleanLogic.h"
#include "Tools/Animation/Graph/AnimationGraphTools_Compilation.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void AndToolsNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        FlowToolsNode::Initialize( pParent );
        CreateOutputPin( "Result", NodeValueType::Bool, true );
        CreateInputPin( "And", NodeValueType::Bool );
        CreateInputPin( "And", NodeValueType::Bool );
    }

    NodeIndex AndToolsNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        AndNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<AndNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            int32 const numInputs = GetNumInputPins();
            for ( auto i = 0; i < numInputs; i++ )
            {
                // We allow some disconnected pins
                auto pConnectedNode = GetConnectedInputNode<FlowToolsNode>( i );
                if ( pConnectedNode != nullptr )
                {
                    auto compiledNodeIdx = pConnectedNode->Compile( context );
                    if ( compiledNodeIdx != InvalidIndex )
                    {
                        pSettings->m_conditionNodeIndices.emplace_back( compiledNodeIdx );
                    }
                    else
                    {
                        return InvalidIndex;
                    }
                }
            }

            //-------------------------------------------------------------------------

            if ( pSettings->m_conditionNodeIndices.empty() )
            {
                context.LogError( this, "All inputs on 'And' node disconnected" );
                return InvalidIndex;
            }
        }
        return pSettings->m_nodeIdx;
    }

    //-------------------------------------------------------------------------

    void OrToolsNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        FlowToolsNode::Initialize( pParent );
        CreateOutputPin( "Result", NodeValueType::Bool, true );
        CreateInputPin( "Or", NodeValueType::Bool );
        CreateInputPin( "Or", NodeValueType::Bool );
    }

    NodeIndex OrToolsNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        OrNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<OrNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            int32 const numInputs = GetNumInputPins();
            for ( auto i = 0; i < numInputs; i++ )
            {
                // We allow some disconnected pins
                auto pConnectedNode = GetConnectedInputNode<FlowToolsNode>( i );
                if ( pConnectedNode != nullptr )
                {
                    auto compiledNodeIdx = pConnectedNode->Compile( context );
                    if ( compiledNodeIdx != InvalidIndex )
                    {
                        pSettings->m_conditionNodeIndices.emplace_back( compiledNodeIdx );
                    }
                    else
                    {
                        return InvalidIndex;
                    }
                }
            }

            //-------------------------------------------------------------------------

            if ( pSettings->m_conditionNodeIndices.empty() )
            {
                context.LogError( this, "All inputs on 'Or' node disconnected" );
                return InvalidIndex;
            }
        }
        return pSettings->m_nodeIdx;
    }

    //-------------------------------------------------------------------------

    void NotToolsNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        FlowToolsNode::Initialize( pParent );
        CreateOutputPin( "Result", NodeValueType::Bool, true );
        CreateInputPin( "Not", NodeValueType::Bool );
    }

    NodeIndex NotToolsNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        NotNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<NotNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            // Set input node
            auto pInputNode = GetConnectedInputNode<FlowToolsNode>( 0 );
            if ( pInputNode != nullptr )
            {
                pSettings->m_inputValueNodeIdx = pInputNode->Compile( context );
                if ( pSettings->m_inputValueNodeIdx == InvalidIndex )
                {
                    return InvalidIndex;
                }
            }
            else
            {
                context.LogError( this, "Input not connected!" );
                return InvalidIndex;
            }
        }
        return pSettings->m_nodeIdx;
    }
}