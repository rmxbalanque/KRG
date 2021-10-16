#pragma once
#include "Tools/Animation/GraphEditor/ToolsGraph/AnimationToolsGraph_FlowNode.h"
#include "Tools/Core/GraphEditor/StateMachine/StateMachineGraph.h"
#include "Tools/Animation/GraphEditor/ToolsGraph/AnimationToolsGraph_FlowGraph.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class Tools_StateBaseNode;

    // The result node for the entry state overrides
    //-------------------------------------------------------------------------

    class Tools_EntryStateOverrideConditionsNode final : public Tools_GraphNode
    {
        friend class Tools_EntryStateOverrideNode;
        KRG_REGISTER_TYPE( Tools_EntryStateOverrideConditionsNode );

    public:

        virtual char const* GetTypeName() const override { return "Entry State Conditions"; }
        virtual char const* GetCategory() const override { return "State Machine"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::ValueTree ); }
        virtual bool IsUserCreatable() const override { return false; }

        void UpdateInputPins( TInlineVector<Tools_StateBaseNode*, 20> const& states );

    private:

        // For each pin, what state does it represent
        KRG_REGISTER TVector<UUID>  m_pinToStateMapping;
    };

    // The Flow graph for the state machine node
    //-------------------------------------------------------------------------

    class Tools_EntryStateOverrideGraph final : public FlowGraph
    {
        friend class Tools_EntryStateOverrideNode;
        KRG_REGISTER_TYPE( Tools_EntryStateOverrideGraph );

    public:

        Tools_EntryStateOverrideGraph();

        virtual void Initialize( GraphEditor::BaseNode* pParentNode ) override;

    private:

        virtual void OnShowGraph() override;
    };

    // State machine node
    //-------------------------------------------------------------------------

    class Tools_EntryStateOverrideNode final : public GraphEditor::SM::Node
    {
        KRG_REGISTER_TYPE( Tools_EntryStateOverrideNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        Tools_GraphNode const* GetEntryConditionNodeForState( UUID const& stateID ) const;
        void UpdateEntryStateNodeInputPins();

        virtual ImColor GetNodeColor() const override { return ImGuiX::ConvertColor( Colors::SlateBlue ); }
        virtual char const* GetTypeName() const override { return "Entry Overrides"; }
        virtual bool IsUserCreatable() const override { return false; }
    };
}