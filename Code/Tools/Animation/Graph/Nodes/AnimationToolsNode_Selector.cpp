#include "AnimationToolsNode_Selector.h"
#include "Tools/Animation/Graph/AnimationGraphTools_Compilation.h"
#include "Tools/Animation/Graph/AnimationGraphTools_FlowGraph.h"
#include "Engine/Animation/Graph/Nodes/AnimationGraphNode_Selector.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void SelectorConditionNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        FlowToolsNode::Initialize( pParent );

        CreateInputPin( "Option 0", NodeValueType::Bool );
        CreateInputPin( "Option 1", NodeValueType::Bool );
    }

    //-------------------------------------------------------------------------

    void SelectorToolsNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        FlowToolsNode::Initialize( pParent );

        CreateOutputPin( "Pose", NodeValueType::Pose );
        CreateInputPin( "Option 0", NodeValueType::Pose );
        CreateInputPin( "Option 1", NodeValueType::Pose );

        auto pConditionGraph = KRG::New<FlowToolGraph>( GraphType::ValueTree );
        pConditionGraph->CreateNode<SelectorConditionNode>();

        SetSecondaryGraph( pConditionGraph );
    }

    NodeIndex SelectorToolsNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        SelectorNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<SelectorNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            int32 const numOptions = GetNumInputPins();

            auto conditionNodes = GetSecondaryGraph()->FindAllNodesOfType<SelectorConditionNode>();
            KRG_ASSERT( conditionNodes.size() == 1 );
            auto pConditionsNode = conditionNodes[0];
            KRG_ASSERT( pConditionsNode->GetNumInputPins() == numOptions );

            for ( auto i = 0; i < numOptions; i++ )
            {
                auto pOptionNode = GetConnectedInputNode<FlowToolsNode>( i );
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

                auto pConditionNode = pConditionsNode->GetConnectedInputNode<FlowToolsNode>( i );
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

    InlineString<100> SelectorToolsNode::GetNewDynamicInputPinName() const
    {
        int32 const numOptions = GetNumInputPins();
        InlineString<100> pinName;
        pinName.sprintf( "Option %d", numOptions - 1 );
        return pinName;
    }

    void SelectorToolsNode::OnDynamicPinCreation( UUID pinID )
    {
        auto conditionNodes = GetSecondaryGraph()->FindAllNodesOfType<SelectorConditionNode>();
        KRG_ASSERT( conditionNodes.size() == 1 );
        auto pConditionsNode = conditionNodes[0];

        int32 const numOptions = GetNumInputPins();
        KRG_ASSERT( pConditionsNode->GetNumInputPins() == numOptions - 1 );

        InlineString<100> pinName;
        pinName.sprintf( "Option %d", numOptions - 1 );
        pConditionsNode->CreateInputPin( pinName.c_str(), NodeValueType::Bool );
    }

    void SelectorToolsNode::OnDynamicPinDestruction( UUID pinID )
    {
        auto conditionNodes = GetSecondaryGraph()->FindAllNodesOfType<SelectorConditionNode>();
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