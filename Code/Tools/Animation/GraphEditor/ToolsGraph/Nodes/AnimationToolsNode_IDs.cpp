#include "AnimationToolsNode_IDs.h"
#include "Tools/Animation/GraphEditor/ToolsGraph/AnimationToolsGraph_Compilation.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void Tools_IDComparisonNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        Tools_GraphNode::Initialize( pParent );
        CreateOutputPin( "Result", ValueType::Bool, true );
        CreateInputPin( "ID", ValueType::ID );
    }

    NodeIndex Tools_IDComparisonNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        IDComparisonNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<IDComparisonNode>( this, pSettings );
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

            pSettings->m_comparison = m_comparison;
            pSettings->m_comparisionIDs.clear();
            pSettings->m_comparisionIDs.insert( pSettings->m_comparisionIDs.begin(), m_IDs.begin(), m_IDs.end() );
        }
        return pSettings->m_nodeIdx;
    }

    void Tools_IDComparisonNode::DrawInfoText( GraphEditor::DrawContext const& ctx )
    {
        InlineString<255> infoText;

        if ( m_comparison == IDComparisonNode::Comparison::Matches )
        {
            infoText = "is: ";
        }
        else
        {
            infoText = "is not: ";
        }

        for ( auto i = 0; i < m_IDs.size(); i++ )
        {
            infoText.append( m_IDs[i].c_str() );
            if ( i != m_IDs.size() - 1 )
            {
                infoText.append( ", " );
            }
        }

        ImGui::Text( infoText.c_str() );
    }

    //-------------------------------------------------------------------------

    void Tools_IDToFloatNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        Tools_GraphNode::Initialize( pParent );
        CreateOutputPin( "Result", ValueType::Float, true );
        CreateInputPin( "ID", ValueType::ID );
    }

    NodeIndex Tools_IDToFloatNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        IDToFloatNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<IDToFloatNode>( this, pSettings );
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

            pSettings->m_defaultValue = m_defaultValue;
            
            for ( auto const& mapping : m_mappings )
            {
                pSettings->m_IDs.emplace_back( mapping.m_ID );
                pSettings->m_values.emplace_back( mapping.m_value );
            }

            KRG_ASSERT( pSettings->m_IDs.size() == pSettings->m_values.size() );
        }
        return pSettings->m_nodeIdx;
    }
}