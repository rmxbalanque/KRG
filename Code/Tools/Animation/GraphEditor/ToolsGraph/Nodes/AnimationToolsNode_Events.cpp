#include "AnimationToolsNode_Events.h"
#include "Tools/Animation/GraphEditor/ToolsGraph/AnimationToolsGraph_Compilation.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void Tools_GenericEventConditionNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        Tools_GraphNode::Initialize( pParent );
        CreateOutputPin( "Result", ValueType::Bool, true );
    }

    NodeIndex Tools_GenericEventConditionNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        GenericEventConditionNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<GenericEventConditionNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            pSettings->m_sourceStateNodeIdx = context.GetConduitSourceStateIndex();
            pSettings->m_operator = m_operator;
            pSettings->m_searchMode = m_searchMode;

            pSettings->m_eventIDs.clear();
            pSettings->m_eventIDs.insert( pSettings->m_eventIDs.begin(), m_eventIDs.begin(), m_eventIDs.end() );
        }
        return pSettings->m_nodeIdx;
    }

    void Tools_GenericEventConditionNode::DrawInfoText( GraphEditor::DrawingContext const& ctx )
    {
        switch ( m_searchMode )
        {
            case EventSearchMode::OnlySearchStateEvents:
            {
                ImGui::Text( "Search: State Events" );
            }
            break;

            case EventSearchMode::OnlySearchAnimEvents:
            {
                ImGui::Text( "Search: Anim Events" );
            }
            break;

            case EventSearchMode::SearchAll:
            {
                ImGui::Text( "Search: All Events" );
            }
            break;
        }

        //-------------------------------------------------------------------------

        InlineString<255> infoText;

        if ( m_operator == GenericEventConditionNode::Operator::Or )
        {
            infoText = "Any: ";
        }
        else
        {
            infoText = "All: ";
        }

        for ( auto i = 0; i < m_eventIDs.size(); i++ )
        {
            infoText.append( m_eventIDs[i].c_str() );
            if ( i != m_eventIDs.size() - 1 )
            {
                infoText.append( ", " );
            }
        }

        ImGui::Text( infoText.c_str() );
    }

    //-------------------------------------------------------------------------

    void Tools_GenericEventPercentageThroughNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        Tools_GraphNode::Initialize( pParent );
        CreateOutputPin( "Result", ValueType::Float, true );
    }

    NodeIndex Tools_GenericEventPercentageThroughNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        GenericEventPercentageThroughNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<GenericEventPercentageThroughNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            pSettings->m_sourceStateNodeIdx = context.GetConduitSourceStateIndex();
            pSettings->m_searchMode = m_searchMode;
            pSettings->m_preferHighestPercentageThrough = m_preferHighestPercentageThrough;
            pSettings->m_eventID = m_eventID;
        }
        return pSettings->m_nodeIdx;
    }

    void Tools_GenericEventPercentageThroughNode::DrawInfoText( GraphEditor::DrawingContext const& ctx )
    {

    }

    //-------------------------------------------------------------------------

    void Tools_FootEventConditionNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        Tools_GraphNode::Initialize( pParent );
        CreateOutputPin( "Result", ValueType::Bool, true );
    }

    NodeIndex Tools_FootEventConditionNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        FootEventConditionNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<FootEventConditionNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            pSettings->m_sourceStateNodeIdx = context.GetConduitSourceStateIndex();
            pSettings->m_phaseCondition = m_phaseCondition;
            pSettings->m_preferHighestPercentageThrough = m_preferHighestPercentageThrough;
        }
        return pSettings->m_nodeIdx;
    }

    //-------------------------------------------------------------------------

    void Tools_FootstepEventPercentageThroughNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        Tools_GraphNode::Initialize( pParent );
        CreateOutputPin( "Result", ValueType::Float, true );
    }

    NodeIndex Tools_FootstepEventPercentageThroughNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        FootstepEventPercentageThroughNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<FootstepEventPercentageThroughNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            pSettings->m_sourceStateNodeIdx = context.GetConduitSourceStateIndex();
            pSettings->m_phaseCondition = m_phaseCondition;
            pSettings->m_preferHighestPercentageThrough = m_preferHighestPercentageThrough;
        }
        return pSettings->m_nodeIdx;
    }

    //-------------------------------------------------------------------------

    void Tools_SyncEventConditionNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        Tools_GraphNode::Initialize( pParent );
        CreateOutputPin( "Result", ValueType::Float, true );
    }

    NodeIndex Tools_SyncEventConditionNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        SyncEventConditionNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<SyncEventConditionNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            pSettings->m_sourceStateNodeIdx = context.GetConduitSourceStateIndex();
            pSettings->m_triggerMode = m_triggerMode;
            pSettings->m_syncEventIdx = m_syncEventIdx;
        }
        return pSettings->m_nodeIdx;
    }

    //-------------------------------------------------------------------------

    void Tools_CurrentSyncEventNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        Tools_GraphNode::Initialize( pParent );
        CreateOutputPin( "Result", ValueType::Float, true );
    }

    NodeIndex Tools_CurrentSyncEventNode::Compile( ToolsGraphCompilationContext& context ) const
    {
        CurrentSyncEventNode::Settings* pSettings = nullptr;
        NodeCompilationState const state = context.GetSettings<CurrentSyncEventNode>( this, pSettings );
        if ( state == NodeCompilationState::NeedCompilation )
        {
            pSettings->m_sourceStateNodeIdx = context.GetConduitSourceStateIndex();
        }
        return pSettings->m_nodeIdx;
    }
}