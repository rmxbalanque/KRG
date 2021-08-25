#include "AnimationToolsNode_State.h"
#include "Tools/Animation/Graph/Nodes/AnimationToolsNode_Results.h"
#include "Tools/Animation/Graph/AnimationGraphTools_FlowGraph.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void StateLayerDataToolsNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        FlowToolsNode::Initialize( pParent );
        CreateInputPin( "Layer Weight", NodeValueType::Float );
        CreateInputPin( "Layer Mask", NodeValueType::BoneMask );
    }

    //-------------------------------------------------------------------------

    void StateBaseToolsNode::DrawExtraControls( GraphEditor::DrawingContext const& ctx )
    {
        InlineString<255> string;
        auto CreateEventString = [&] ( TVector<StringID> const& IDs )
        {
            string.clear();
            for ( int32 i = 0; i < (int32) IDs.size(); i++ )
            {
                string += IDs[i].c_str();

                if ( i != IDs.size() - 1 )
                {
                    string += ", ";
                }
            }
        };

        CreateEventString( m_entryEvents );
        ImGui::Text( "Entry: %s", string.c_str() );

        CreateEventString( m_executeEvents );
        ImGui::Text( "Execute: %s", string.c_str() );

        CreateEventString( m_exitEvents );
        ImGui::Text( "Exit: %s", string.c_str() );
    }

    //-------------------------------------------------------------------------

    void StateToolsNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        GraphEditor::SM::State::Initialize( pParent );

        auto pBlendTree = KRG::New<FlowToolGraph>( GraphType::BlendTree );
        pBlendTree->CreateNode<ResultToolsNode>( NodeValueType::Pose );
        SetChildGraph( pBlendTree );

        auto pValueTree = KRG::New<FlowToolGraph>( GraphType::ValueTree );
        pValueTree->CreateNode<StateLayerDataToolsNode>();
        SetSecondaryGraph( pValueTree );
    }

    ResultToolsNode const* StateToolsNode::GetBlendTreeRootNode() const
    {
        auto resultNodes = GetChildGraph()->FindAllNodesOfType<ResultToolsNode>();
        KRG_ASSERT( resultNodes.size() == 1 );
        return resultNodes[0];
    }

    StateLayerDataToolsNode const* StateToolsNode::GetLayerDataNode() const
    {
        auto dataNodes = GetSecondaryGraph()->FindAllNodesOfType<StateLayerDataToolsNode>();
        KRG_ASSERT( dataNodes.size() == 1 );
        return dataNodes[0];
    }

    void StateToolsNode::DrawExtraControls( GraphEditor::DrawingContext const& ctx )
    {
        InlineString<255> string;
        auto CreateTimedEventString = [&] ( TVector<TimedStateEvent> const& events )
        {
            string.clear();
            for ( int32 i = 0; i < (int32) events.size(); i++ )
            {
                string += events[i].m_ID.c_str();

                if ( i != events.size() - 1 )
                {
                    string += ", ";
                }
            }
        };

        //-------------------------------------------------------------------------

        StateBaseToolsNode::DrawExtraControls( ctx );

        ImGui::NewLine();

        CreateTimedEventString( m_timeRemainingEvents );
        ImGui::Text( "Time Left: %s", string.c_str() );

        CreateTimedEventString( m_timeElapsedEvents );
        ImGui::Text( "Time Elapsed: %s", string.c_str() );
    }

    void OffStateToolsNode::DrawExtraControls( GraphEditor::DrawingContext const& ctx )
    {
        {
            ImGuiX::ScopedFont font( ImGuiX::Font::Large, Colors::Red );
            ImGui::Text( KRG_ICON_TIMES );
        }

        StateBaseToolsNode::DrawExtraControls( ctx );
    }
}