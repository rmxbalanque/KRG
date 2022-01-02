#pragma once
#include "Tools/Animation/GraphEditor/EditorGraph/Animation_EditorGraph_FlowGraph.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::GraphNodes
{
    class SelectorConditionEditorNode final : public EditorGraphNode
    {
        KRG_REGISTER_TYPE( SelectorConditionEditorNode );

        friend class SelectorEditorNode;

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Conditions"; }
        virtual char const* GetCategory() const override { return "Conditions"; }
        virtual bool IsUserCreatable() const override { return false; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::ValueTree ); }
    };

    //-------------------------------------------------------------------------

    class SelectorEditorNode final : public EditorGraphNode
    {
        KRG_REGISTER_TYPE( SelectorEditorNode );

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

    private:

        virtual char const* GetTypeName() const override { return "Selector"; }
        virtual char const* GetCategory() const override { return "Utility"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree ); }
        virtual GraphNodeIndex Compile( EditorGraphCompilationContext& context ) const override;

        virtual bool SupportsDynamicInputPins() const override { return true; }
        virtual TInlineString<100> GetNewDynamicInputPinName() const override;
        virtual uint32 GetDynamicInputPinValueType() const override { return (uint32) GraphValueType::Pose; }
        virtual void OnDynamicPinCreation( UUID pinID ) override;
        virtual void OnDynamicPinDestruction( UUID pinID ) override;
    };
}