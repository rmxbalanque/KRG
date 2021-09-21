#pragma once
#include "../AnimationToolsGraph_FlowNode.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class Tools_AndNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_AndNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "And"; }
        virtual char const* GetCategory() const override { return "Conditions"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual bool SupportsDynamicInputPins() const override { return true; }
        virtual InlineString<100> GetNewDynamicInputPinName() const override { return "And"; }
        virtual uint32 GetDynamicInputPinValueType() const override { return (uint32) ValueType::Bool; }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;
    };

    //-------------------------------------------------------------------------

    class Tools_OrNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_OrNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Or"; }
        virtual char const* GetCategory() const override { return "Conditions"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual bool SupportsDynamicInputPins() const override { return true; }
        virtual InlineString<100> GetNewDynamicInputPinName() const override{ return "Or"; }
        virtual uint32 GetDynamicInputPinValueType() const override{ return (uint32) ValueType::Bool; }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;
    };

    //-------------------------------------------------------------------------

    class Tools_NotNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_NotNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Not"; }
        virtual char const* GetCategory() const override { return "Conditions"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;
    };
}