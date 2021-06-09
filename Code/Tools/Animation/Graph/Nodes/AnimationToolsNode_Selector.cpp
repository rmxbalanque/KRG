#include "AnimationToolsNode_Selector.h"
#include "Tools/Animation/Graph/AnimationGraphTools_Compilation.h"
#include "Tools/Animation/Graph/AnimationGraphTools_Graph.h"
#include "Engine/Animation/Graph/Nodes/AnimationGraphNode_Selector.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    SelectorConditionNode::SelectorConditionNode()
        : ToolsNode()
    {
        CreateInputPin( "Out", NodeValueType::Bool );
    }

    //-------------------------------------------------------------------------

    SelectorToolsNode::SelectorToolsNode()
        : ToolsNode()
    {
        CreateOutputPin( "Pose", NodeValueType::Pose );
        CreateInputPin( "Option", NodeValueType::Pose );
        CreateInputPin( "Option", NodeValueType::Pose );

        auto pConditionGraph = KRG::New<ToolsGraph>( GraphType::ValueTree );
        pConditionGraph->CreateNode<SelectorConditionNode>();
        m_pSecondaryGraph = pConditionGraph;
    }

    NodeIndex SelectorToolsNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        SelectorNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<SelectorNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            int32 const numOptions = GetNumInputPins();

            auto conditionNodes = m_pSecondaryGraph->FindAllNodesOfType<SelectorConditionNode>( false );
            KRG_ASSERT( conditionNodes.size() == 1 );
            auto pConditionsNode = conditionNodes[0];
            KRG_ASSERT( pConditionsNode->GetNumInputPins() == numOptions );

            for ( auto i = 0; i < numOptions; i++ )
            {
                auto pOptionNode = GetConnectedInputNode<ToolsNode>( i );
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

                auto pConditionNode = pConditionsNode->GetConnectedInputNode<ToolsNode>( i );
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

    void SelectorToolsNode::OnDynamicPinCreated( UUID pinID )
    {
        auto conditionNodes = m_pSecondaryGraph->FindAllNodesOfType<SelectorConditionNode>( false );
        KRG_ASSERT( conditionNodes.size() == 1 );
        auto pConditionsNode = conditionNodes[0];

        int32 const numOptions = GetNumInputPins();
        KRG_ASSERT( pConditionsNode->GetNumInputPins() == ( numOptions - 2 ) );

        // Update pins on condition node
    }

    void SelectorToolsNode::OnDynamicPinDestroyed( UUID pinID )
    {
        auto conditionNodes = m_pSecondaryGraph->FindAllNodesOfType<SelectorConditionNode>( false );
        KRG_ASSERT( conditionNodes.size() == 1 );
        auto pConditionsNode = conditionNodes[0];

        int32 const numOptions = GetNumInputPins();
        KRG_ASSERT( pConditionsNode->GetNumInputPins() == numOptions );

        // Update pins on condition node
    }
}