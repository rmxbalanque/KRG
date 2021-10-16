#pragma once
#include "Tools/Animation/GraphEditor/ToolsGraph/AnimationToolsGraph_FlowNode.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class Tools_StateBaseNode;
    class Tools_TransitionNode;

    //-------------------------------------------------------------------------

    class Tools_StateMachineNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_StateMachineNode );

        struct TransitionCompileResult
        {

        };

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetDisplayName() const override { return m_name.c_str(); }
        virtual char const* GetTypeName() const override { return "State Machine"; }
        virtual char const* GetCategory() const override { return "State Machine"; }
        virtual ImColor GetNodeColor() const override { return ImGuiX::ConvertColor( Colors::CornflowerBlue ); }

        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;

    private:

        NodeIndex CompileState( ToolsGraphCompilationContext& context, Tools_StateBaseNode const* pBaseStateNode ) const;
        NodeIndex CompileTransition( ToolsGraphCompilationContext& context, Tools_TransitionNode const* pTransitionNode, NodeIndex targetStateNodeIdx ) const;

    private:

        KRG_EXPOSE String m_name = "State Machine";
    };
}