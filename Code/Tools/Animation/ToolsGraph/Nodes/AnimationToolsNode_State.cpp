#include "AnimationToolsNode_State.h"
#include "Tools/Animation/ToolsGraph/Nodes/AnimationToolsNode_Results.h"
#include "Tools/Animation/ToolsGraph/AnimationToolsGraph_FlowGraph.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void Tools_StateLayerDataNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        Tools_GraphNode::Initialize( pParent );
        CreateInputPin( "Layer Weight", ValueType::Float );
        CreateInputPin( "Layer Mask", ValueType::BoneMask );
    }

    //-------------------------------------------------------------------------

    void Tools_StateBaseNode::DrawExtraControls( GraphEditor::DrawingContext const& ctx )
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

    void Tools_StateNode::Initialize( GraphEditor::BaseGraph* pParent )
    {
        GraphEditor::SM::State::Initialize( pParent );

        auto pBlendTree = KRG::New<FlowGraph>( GraphType::BlendTree );
        pBlendTree->CreateNode<Tools_ResultNode>( ValueType::Pose );
        SetChildGraph( pBlendTree );

        auto pValueTree = KRG::New<FlowGraph>( GraphType::ValueTree );
        pValueTree->CreateNode<Tools_StateLayerDataNode>();
        SetSecondaryGraph( pValueTree );
    }

    Tools_ResultNode const* Tools_StateNode::GetBlendTreeRootNode() const
    {
        auto resultNodes = GetChildGraph()->FindAllNodesOfType<Tools_ResultNode>();
        KRG_ASSERT( resultNodes.size() == 1 );
        return resultNodes[0];
    }

    Tools_StateLayerDataNode const* Tools_StateNode::GetLayerDataNode() const
    {
        auto dataNodes = GetSecondaryGraph()->FindAllNodesOfType<Tools_StateLayerDataNode>();
        KRG_ASSERT( dataNodes.size() == 1 );
        return dataNodes[0];
    }

    void Tools_StateNode::DrawExtraControls( GraphEditor::DrawingContext const& ctx )
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

        Tools_StateBaseNode::DrawExtraControls( ctx );

        ImGui::NewLine();

        CreateTimedEventString( m_timeRemainingEvents );
        ImGui::Text( "Time Left: %s", string.c_str() );

        CreateTimedEventString( m_timeElapsedEvents );
        ImGui::Text( "Time Elapsed: %s", string.c_str() );
    }

    void Tools_OffStateNode::DrawExtraControls( GraphEditor::DrawingContext const& ctx )
    {
        {
            ImGuiX::ScopedFont font( ImGuiX::Font::Large, Colors::Red );
            ImGui::Text( KRG_ICON_TIMES );
        }

        Tools_StateBaseNode::DrawExtraControls( ctx );
    }
}