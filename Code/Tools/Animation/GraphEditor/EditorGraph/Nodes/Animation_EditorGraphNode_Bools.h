#pragma once
#include "Tools/Animation/GraphEditor/EditorGraph/Animation_EditorGraph_FlowGraph.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class AndEditorNode final : public EditorGraphNode
    {
        KRG_REGISTER_TYPE( AndEditorNode );

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "And"; }
        virtual char const* GetCategory() const override { return "Values/Bool"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual bool SupportsDynamicInputPins() const override { return true; }
        virtual InlineString<100> GetNewDynamicInputPinName() const override { return "And"; }
        virtual uint32 GetDynamicInputPinValueType() const override { return (uint32) ValueType::Bool; }
        virtual NodeIndex Compile( EditorGraphCompilationContext& context ) const override;
    };

    //-------------------------------------------------------------------------

    class OrEditorNode final : public EditorGraphNode
    {
        KRG_REGISTER_TYPE( OrEditorNode );

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Or"; }
        virtual char const* GetCategory() const override { return "Values/Bool"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual bool SupportsDynamicInputPins() const override { return true; }
        virtual InlineString<100> GetNewDynamicInputPinName() const override{ return "Or"; }
        virtual uint32 GetDynamicInputPinValueType() const override{ return (uint32) ValueType::Bool; }
        virtual NodeIndex Compile( EditorGraphCompilationContext& context ) const override;
    };

    //-------------------------------------------------------------------------

    class NotEditorNode final : public EditorGraphNode
    {
        KRG_REGISTER_TYPE( NotEditorNode );

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Not"; }
        virtual char const* GetCategory() const override { return "Values/Bool"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( EditorGraphCompilationContext& context ) const override;
    };
}