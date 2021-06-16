#pragma once
#include "Tools/Animation/Graph/AnimationGraphTools_FlowNode.h"
#include "Tools/Core/GraphEditor/StateMachine/StateMachineGraph.h"
#include "Tools/Animation/Graph/AnimationGraphTools_FlowGraph.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class StateToolsNode;

    // The result node for the entry state overrides
    //-------------------------------------------------------------------------

    class EntryStateOverrideConditionsNode final : public FlowToolsNode
    {
        friend class EntryStateOverrideNode;
        KRG_REGISTER_TYPE( EntryStateOverrideConditionsNode );

    public:

        virtual char const* GetTypeName() const override { return "Entry State Conditions"; }
        virtual char const* GetCategory() const override { return "State Machine"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::ValueTree ); }
        virtual bool IsUserCreatable() const override { return false; }

        void Update( TInlineVector<StateToolsNode*, 20> const& states );

    private:

        // For each pin, what state does it represent
        REGISTER TVector<UUID>  m_pinToStateMapping;
    };

    // The Flow graph for the state machine node
    //-------------------------------------------------------------------------

    class EntryStateOverrideGraph final : public FlowToolGraph
    {
        KRG_REGISTER_TYPE( EntryStateOverrideGraph );

    public:

        EntryStateOverrideGraph();

        virtual void Initialize( GraphEditor::BaseNode* pParentNode ) override;

        void Update( TInlineVector<StateToolsNode*, 20> const& states );
    };

    // State machine node
    //-------------------------------------------------------------------------

    class EntryStateOverrideNode final : public GraphEditor::SM::Node
    {
        KRG_REGISTER_TYPE( EntryStateOverrideNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        inline void Update( TInlineVector<StateToolsNode*, 20> const& states )
        {
            Cast<EntryStateOverrideGraph>( GetSecondaryGraph() )->Update( states );
        }

        FlowToolsNode const* GetEntryConditionNodeForState( UUID const& stateID ) const;

        virtual ImColor GetHighlightColor() const override { return ImGuiX::ConvertColor( Colors::SlateBlue ); }
        virtual char const* GetTypeName() const override { return "Entry Overrides"; }
        virtual bool IsUserCreatable() const override { return false; }
    };
}