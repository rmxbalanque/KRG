#pragma once
#include "../AnimationGraphTools_FlowNode.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class ConstBoolToolsNode final : public FlowToolsNode
    {
        KRG_REGISTER_TYPE( ConstBoolToolsNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Bool"; }
        virtual char const* GetCategory() const override { return "Const Values"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;
        virtual void DrawExtraControls( GraphEditor::DrawingContext const& ctx ) override;

    private:

        EXPOSE bool m_value;
    };

    //-------------------------------------------------------------------------

    class ConstIDToolsNode final : public FlowToolsNode
    {
        KRG_REGISTER_TYPE( ConstIDToolsNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "ID"; }
        virtual char const* GetCategory() const override { return "Const Values"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;
        virtual void DrawExtraControls( GraphEditor::DrawingContext const& ctx ) override;

    private:

        EXPOSE StringID m_value;
    };

    //-------------------------------------------------------------------------
    
    class ConstIntToolsNode final : public FlowToolsNode
    {
        KRG_REGISTER_TYPE( ConstIntToolsNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Int"; }
        virtual char const* GetCategory() const override { return "Const Values"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;
        virtual void DrawExtraControls( GraphEditor::DrawingContext const& ctx ) override;

    private:

        EXPOSE int32 m_value;
    };

    //-------------------------------------------------------------------------

    class ConstFloatToolsNode final : public FlowToolsNode
    {
        KRG_REGISTER_TYPE( ConstFloatToolsNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Float"; }
        virtual char const* GetCategory() const override { return "Const Values"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;
        virtual void DrawExtraControls( GraphEditor::DrawingContext const& ctx ) override;

    private:

        EXPOSE float m_value;
    };

    //-------------------------------------------------------------------------

    class ConstVectorToolsNode final : public FlowToolsNode
    {
        KRG_REGISTER_TYPE( ConstVectorToolsNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Vector"; }
        virtual char const* GetCategory() const override { return "Const Values"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;
        virtual void DrawExtraControls( GraphEditor::DrawingContext const& ctx ) override;

    private:

        EXPOSE Float4 m_value;
    };

    //-------------------------------------------------------------------------

    class ConstTargetToolsNode final : public FlowToolsNode
    {
        KRG_REGISTER_TYPE( ConstTargetToolsNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Target"; }
        virtual char const* GetCategory() const override { return "Const Values"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;
        virtual void DrawExtraControls( GraphEditor::DrawingContext const& ctx ) override;

    private:

        EXPOSE Transform m_value;
    };
}