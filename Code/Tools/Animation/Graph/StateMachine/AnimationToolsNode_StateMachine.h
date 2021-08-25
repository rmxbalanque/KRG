#pragma once
#include "Tools/Animation/Graph/AnimationGraphTools_FlowNode.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class StateBaseToolsNode;
    class TransitionToolsNode;

    //-------------------------------------------------------------------------

    class StateMachineToolsNode final : public FlowToolsNode
    {
        KRG_REGISTER_TYPE( StateMachineToolsNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetDisplayName() const override { return m_name.c_str(); }
        virtual char const* GetTypeName() const override { return "State Machine"; }
        virtual char const* GetCategory() const override { return "State Machine"; }
        virtual ImColor GetHighlightColor() const override { return ImGuiX::ConvertColor( Colors::CornflowerBlue ); }

        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;

    private:

        NodeIndex CompileState( ToolsGraphCompilationContext& context, StateBaseToolsNode const* pBaseStateNode ) const;
        NodeIndex CompileTransition( ToolsGraphCompilationContext& context, TransitionToolsNode const* pTransitionNode, NodeIndex targetStateNodeIdx ) const;

    private:

        EXPOSE String m_name = "State Machine";
    };
}