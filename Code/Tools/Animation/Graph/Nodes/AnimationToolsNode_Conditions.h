#pragma once
#include "../AnimationGraphTools_Node.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class AndToolsNode final : public ToolsNode
    {
        KRG_REGISTER_TYPE( AndToolsNode );

    public:

        AndToolsNode();

        virtual char const* GetTypeName() const override { return "And"; }
        virtual char const* GetCategory() const override { return "Conditions"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual bool SupportsDynamicInputPins() const override { return true; }
        virtual InlineString<100> GetNewDynamicInputPinName() const override { return "And"; }
        virtual uint32 GetDynamicInputPinValueType() const override { return (uint32) NodeValueType::Bool; }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;
    };

    //-------------------------------------------------------------------------

    class OrToolsNode final : public ToolsNode
    {
        KRG_REGISTER_TYPE( OrToolsNode );

    public:

        OrToolsNode();

        virtual char const* GetTypeName() const override { return "Or"; }
        virtual char const* GetCategory() const override { return "Conditions"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual bool SupportsDynamicInputPins() const override { return true; }
        virtual InlineString<100> GetNewDynamicInputPinName() const override{ return "Or"; }
        virtual uint32 GetDynamicInputPinValueType() const override{ return (uint32) NodeValueType::Bool; }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;
    };

    //-------------------------------------------------------------------------

    class NotToolsNode final : public ToolsNode
    {
        KRG_REGISTER_TYPE( NotToolsNode );

    public:

        NotToolsNode();

        virtual char const* GetTypeName() const override { return "Not"; }
        virtual char const* GetCategory() const override { return "Conditions"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;
    };
}