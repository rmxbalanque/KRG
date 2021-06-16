#pragma once
#include "Tools/Animation/Graph/AnimationGraphTools_FlowNode.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class StateToolsNode;
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
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;

    private:

        NodeIndex CompileState( ToolsGraphCompilationContext& context, StateToolsNode const* pState ) const;
        NodeIndex CompileTransition( ToolsGraphCompilationContext& context, TransitionToolsNode const* pTransition, NodeIndex targetStateNodeIdx ) const;

    private:

        EXPOSE String m_name = "State Machine";
    };
}