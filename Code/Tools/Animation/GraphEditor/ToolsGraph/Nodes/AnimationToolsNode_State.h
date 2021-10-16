#pragma once
#include "Tools/Animation/GraphEditor/ToolsGraph/AnimationToolsGraph_FlowNode.h"
#include "Tools/Animation/GraphEditor/ToolsGraph/AnimationToolsGraph_StateMachineNode.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class Tools_ResultNode;

    //-------------------------------------------------------------------------

    class Tools_StateLayerDataNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_StateLayerDataNode );

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "State Layer Data"; }
        virtual char const* GetCategory() const override { return "State Machine"; }
        virtual bool IsUserCreatable() const override { return false; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::ValueTree ); }
    };

    //-------------------------------------------------------------------------

    class Tools_StateNode final : public Tools_StateBaseNode
    {
        friend class Tools_StateMachineNode;
        KRG_REGISTER_TYPE( Tools_StateNode );

    public:

        Tools_ResultNode const* GetBlendTreeRootNode() const;
        Tools_StateLayerDataNode const* GetLayerDataNode() const;
        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

    private:

        virtual char const* GetTypeName() const override { return "State"; }
    };

    //-------------------------------------------------------------------------

    class Tools_OffStateNode final : public Tools_StateBaseNode
    {
        friend class Tools_StateMachineNode;
        KRG_REGISTER_TYPE( Tools_OffStateNode );

    private:

        virtual char const* GetTypeName() const override { return "Off State"; }
        virtual ImColor GetNodeColor() const override { return ImGuiX::ConvertColor( Colors::PaleVioletRed ); }
        virtual void DrawExtraControls( GraphEditor::DrawingContext const& ctx ) override;
    };
}