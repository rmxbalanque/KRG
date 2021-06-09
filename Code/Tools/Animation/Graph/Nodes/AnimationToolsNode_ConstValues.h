#pragma once
#include "../AnimationGraphTools_Node.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class ConstBoolToolsNode final : public ToolsNode
    {
        KRG_REGISTER_TYPE( ConstBoolToolsNode );

    public:

        ConstBoolToolsNode();

        virtual char const* GetTypeName() const override { return "Bool"; }
        virtual char const* GetCategory() const override { return "Const Values"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;

    private:

        EXPOSE bool m_value;
    };

    //-------------------------------------------------------------------------

    class ConstIDToolsNode final : public ToolsNode
    {
        KRG_REGISTER_TYPE( ConstIDToolsNode );

    public:

        ConstIDToolsNode();

        virtual char const* GetTypeName() const override { return "ID"; }
        virtual char const* GetCategory() const override { return "Const Values"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;

    private:

        EXPOSE StringID m_value;
    };

    //-------------------------------------------------------------------------
    
    class ConstIntToolsNode final : public ToolsNode
    {
        KRG_REGISTER_TYPE( ConstIntToolsNode );

    public:

        ConstIntToolsNode();

        virtual char const* GetTypeName() const override { return "Int"; }
        virtual char const* GetCategory() const override { return "Const Values"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;

    private:

        EXPOSE int32 m_value;
    };

    //-------------------------------------------------------------------------

    class ConstFloatToolsNode final : public ToolsNode
    {
        KRG_REGISTER_TYPE( ConstFloatToolsNode );

    public:

        ConstFloatToolsNode();

        virtual char const* GetTypeName() const override { return "Float"; }
        virtual char const* GetCategory() const override { return "Const Values"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;

    private:

        EXPOSE float m_value;
    };

    //-------------------------------------------------------------------------

    class ConstVectorToolsNode final : public ToolsNode
    {
        KRG_REGISTER_TYPE( ConstVectorToolsNode );

    public:

        ConstVectorToolsNode();

        virtual char const* GetTypeName() const override { return "Vector"; }
        virtual char const* GetCategory() const override { return "Const Values"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;

    private:

        EXPOSE Float4 m_value;
    };

    //-------------------------------------------------------------------------

    class ConstTargetToolsNode final : public ToolsNode
    {
        KRG_REGISTER_TYPE( ConstTargetToolsNode );

    public:

        ConstTargetToolsNode();

        virtual char const* GetTypeName() const override { return "Target"; }
        virtual char const* GetCategory() const override { return "Const Values"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;

    private:

        EXPOSE Transform m_value;
    };
}