#include "AnimationToolsNode_Bools.h"
#include "Engine/Animation/Graph/Nodes/AnimationGraphNode_Bools.h"
#include "Tools/Animation/GraphEditor/ToolsGraph/AnimationToolsGraph_Compilation.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void Tools_AndNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        Tools_GraphNode::Initialize( pParent );
        CreateOutputPin( "Result", ValueType::Bool, true );
        CreateInputPin( "And", ValueType::Bool );
        CreateInputPin( "And", ValueType::Bool );
    }

    NodeIndex Tools_AndNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        AndNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<AndNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            int32 const numInputs = GetNumInputPins();
            for ( auto i = 0; i < numInputs; i++ )
            {
                // We allow some disconnected pins
                auto pConnectedNode = GetConnectedInputNode<Tools_GraphNode>( i );
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

    void Tools_OrNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        Tools_GraphNode::Initialize( pParent );
        CreateOutputPin( "Result", ValueType::Bool, true );
        CreateInputPin( "Or", ValueType::Bool );
        CreateInputPin( "Or", ValueType::Bool );
    }

    NodeIndex Tools_OrNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        OrNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<OrNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            int32 const numInputs = GetNumInputPins();
            for ( auto i = 0; i < numInputs; i++ )
            {
                // We allow some disconnected pins
                auto pConnectedNode = GetConnectedInputNode<Tools_GraphNode>( i );
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

    void Tools_NotNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        Tools_GraphNode::Initialize( pParent );
        CreateOutputPin( "Result", ValueType::Bool, true );
        CreateInputPin( "Not", ValueType::Bool );
    }

    NodeIndex Tools_NotNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        NotNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<NotNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            // Set input node
            auto pInputNode = GetConnectedInputNode<Tools_GraphNode>( 0 );
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