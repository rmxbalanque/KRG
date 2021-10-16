#include "AnimationToolsNode_Selector.h"
#include "Tools/Animation/GraphEditor/ToolsGraph/AnimationToolsGraph_Compilation.h"
#include "Tools/Animation/GraphEditor/ToolsGraph/AnimationToolsGraph_FlowGraph.h"
#include "Engine/Animation/Graph/Nodes/AnimationGraphNode_Selector.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void Tools_SelectorConditionNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        Tools_GraphNode::Initialize( pParent );

        CreateInputPin( "Option 0", ValueType::Bool );
        CreateInputPin( "Option 1", ValueType::Bool );
    }

    //-------------------------------------------------------------------------

    void Tools_SelectorNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        Tools_GraphNode::Initialize( pParent );

        CreateOutputPin( "Pose", ValueType::Pose );
        CreateInputPin( "Option 0", ValueType::Pose );
        CreateInputPin( "Option 1", ValueType::Pose );

        auto pConditionGraph = KRG::New<FlowGraph>( GraphType::ValueTree );
        pConditionGraph->CreateNode<Tools_SelectorConditionNode>();

        SetSecondaryGraph( pConditionGraph );
    }

    NodeIndex Tools_SelectorNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        SelectorNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<SelectorNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            int32 const numOptions = GetNumInputPins();

            auto conditionNodes = GetSecondaryGraph()->FindAllNodesOfType<Tools_SelectorConditionNode>();
            KRG_ASSERT( conditionNodes.size() == 1 );
            auto pConditionsNode = conditionNodes[0];
            KRG_ASSERT( pConditionsNode->GetNumInputPins() == numOptions );

            for ( auto i = 0; i < numOptions; i++ )
            {
                auto pOptionNode = GetConnectedInputNode<Tools_GraphNode>( i );
                if ( pOptionNode != nullptr )
                {
                    auto compiledNodeIdx = pOptionNode->Compile( context );
                    if ( compiledNodeIdx != InvalidIndex )
                    {
                        pSettings->m_optionNodeIndices.emplace_back( compiledNodeIdx );
                    }
                    else
                    {
                        return InvalidIndex;
                    }
                }
                else
                {
                    context.LogError( this, "Disconnected option pin on selector node!" );
                    return InvalidIndex;
                }

                auto pConditionNode = pConditionsNode->GetConnectedInputNode<Tools_GraphNode>( i );
                if ( pConditionNode != nullptr )
                {
                    auto compiledNodeIdx = pConditionNode->Compile( context );
                    if ( compiledNodeIdx != InvalidIndex )
                    {
                        pSettings->m_conditionNodeIndices.emplace_back( compiledNodeIdx );
                    }
                    else
                    {
                        return InvalidIndex;
                    }
                }
                else
                {
                    context.LogError( this, "Disconnected condition pin on selector node!" );
                    return InvalidIndex;
                }
            }

            KRG_ASSERT( pSettings->m_optionNodeIndices.size() == pSettings->m_conditionNodeIndices.size() );

            //-------------------------------------------------------------------------

            if ( pSettings->m_optionNodeIndices.empty() )
            {
                context.LogError( this, "No inputs on selector" );
                return InvalidIndex;
            }
        }
        return pSettings->m_nodeIdx;
    }

    InlineString<100> Tools_SelectorNode::GetNewDynamicInputPinName() const
    {
        int32 const numOptions = GetNumInputPins();
        InlineString<100> pinName;
        pinName.sprintf( "Option %d", numOptions - 1 );
        return pinName;
    }

    void Tools_SelectorNode::OnDynamicPinCreation( UUID pinID )
    {
        auto conditionNodes = GetSecondaryGraph()->FindAllNodesOfType<Tools_SelectorConditionNode>();
        KRG_ASSERT( conditionNodes.size() == 1 );
        auto pConditionsNode = conditionNodes[0];

        int32 const numOptions = GetNumInputPins();
        KRG_ASSERT( pConditionsNode->GetNumInputPins() == numOptions - 1 );

        InlineString<100> pinName;
        pinName.sprintf( "Option %d", numOptions - 1 );
        pConditionsNode->CreateInputPin( pinName.c_str(), ValueType::Bool );
    }

    void Tools_SelectorNode::OnDynamicPinDestruction( UUID pinID )
    {
        auto conditionNodes = GetSecondaryGraph()->FindAllNodesOfType<Tools_SelectorConditionNode>();
        KRG_ASSERT( conditionNodes.size() == 1 );
        auto pConditionsNode = conditionNodes[0];

        int32 const numOptions = GetNumInputPins();
        KRG_ASSERT( pConditionsNode->GetNumInputPins() == numOptions );

        int32 const pintoBeRemovedIdx = GetInputPinIndex( pinID );
        KRG_ASSERT( pintoBeRemovedIdx != InvalidIndex );

        // Rename all pins
        //-------------------------------------------------------------------------

        int32 newPinIdx = 2;
        for ( auto i = 2; i < numOptions; i++ )
        {
            if ( i == pintoBeRemovedIdx )
            {
                continue;
            }

            InlineString<100> newPinName; 
            newPinName.sprintf( "Option %d", newPinIdx );

            GetInputPin( i )->m_name = newPinName;
            pConditionsNode->GetInputPin( i )->m_name = newPinName;
            newPinIdx++;
        }

        // Destroy condition node pin
        //-------------------------------------------------------------------------

        pConditionsNode->DestroyInputPin( pintoBeRemovedIdx );
    }
}