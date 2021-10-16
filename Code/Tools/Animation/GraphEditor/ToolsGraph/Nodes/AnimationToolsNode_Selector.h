#pragma once
#include "../AnimationToolsGraph_FlowNode.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class Tools_SelectorConditionNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_SelectorConditionNode );

        friend class Tools_SelectorNode;

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Conditions"; }
        virtual char const* GetCategory() const override { return "Conditions"; }
        virtual bool IsUserCreatable() const override { return false; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::ValueTree ); }
    };

    //-------------------------------------------------------------------------

    class Tools_SelectorNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_SelectorNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

    private:

        virtual char const* GetTypeName() const override { return "Selector"; }
        virtual char const* GetCategory() const override { return "Anim"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;

        virtual bool SupportsDynamicInputPins() const override { return true; }
        virtual InlineString<100> GetNewDynamicInputPinName() const override;
        virtual uint32 GetDynamicInputPinValueType() const override { return (uint32) ValueType::Pose; }
        virtual void OnDynamicPinCreation( UUID pinID ) override;
        virtual void OnDynamicPinDestruction( UUID pinID ) override;
    };
}