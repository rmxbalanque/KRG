#include "AnimationToolsNode_State.h"
#include "Tools/Animation/GraphEditor/ToolsGraph/Nodes/AnimationToolsNode_Results.h"
#include "Tools/Animation/GraphEditor/ToolsGraph/AnimationToolsGraph_FlowGraph.h"

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

    void Tools_OffStateNode::DrawExtraControls( GraphEditor::DrawingContext const& ctx )
    {
        {
            ImGuiX::ScopedFont font( ImGuiX::Font::Large, Colors::Red );
            ImGui::Text( KRG_ICON_TIMES );
        }

        Tools_StateBaseNode::DrawExtraControls( ctx );
    }
}