#include "Animation_EditorGraphNode_IDs.h"
#include "Tools/Animation/GraphEditor/EditorGraph/Animation_EditorGraph_Compilation.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void IDComparisonEditorNode::Initialize( VisualGraph::BaseGraph* pParent )
    {
        EditorGraphNode::Initialize( pParent );
        CreateOutputPin( "Result", ValueType::Bool, true );
        CreateInputPin( "ID", ValueType::ID );
    }

    NodeIndex IDComparisonEditorNode::Compile( EditorGraphCompilationContext& context ) const
    {
        IDComparisonNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<IDComparisonNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            auto pInputNode = GetConnectedInputNode<EditorGraphNode>( 0 );
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

    void IDComparisonEditorNode::DrawInfoText( VisualGraph::DrawContext const& ctx )
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
            if ( m_IDs[i].IsValid() )
            {
                infoText.append( m_IDs[i].c_str() );
            }
            else
            {
                infoText.append( "INVALID ID" );
            }

            if ( i != m_IDs.size() - 1 )
            {
                infoText.append( ", " );
            }
        }

        ImGui::Text( infoText.c_str() );
    }

    //-------------------------------------------------------------------------

    void IDToFloatEditorNode::Initialize( VisualGraph::BaseGraph* pParent )
    {
        EditorGraphNode::Initialize( pParent );
        CreateOutputPin( "Result", ValueType::Float, true );
        CreateInputPin( "ID", ValueType::ID );
    }

    NodeIndex IDToFloatEditorNode::Compile( EditorGraphCompilationContext& context ) const
    {
        IDToFloatNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<IDToFloatNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            auto pInputNode = GetConnectedInputNode<EditorGraphNode>( 0 );
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