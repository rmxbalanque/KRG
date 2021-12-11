#pragma once
#include "../AnimationToolsGraph_FlowNode.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class Tools_ConstBoolNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_ConstBoolNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Bool"; }
        virtual char const* GetCategory() const override { return "Const Values"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;
        virtual void DrawExtraControls( GraphEditor::DrawContext const& ctx ) override;

    private:

        KRG_EXPOSE bool m_value;
    };

    //-------------------------------------------------------------------------

    class Tools_ConstIDNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_ConstIDNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "ID"; }
        virtual char const* GetCategory() const override { return "Const Values"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;
        virtual void DrawExtraControls( GraphEditor::DrawContext const& ctx ) override;

    private:

        KRG_EXPOSE StringID m_value;
    };

    //-------------------------------------------------------------------------
    
    class Tools_ConstIntNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_ConstIntNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Int"; }
        virtual char const* GetCategory() const override { return "Const Values"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;
        virtual void DrawExtraControls( GraphEditor::DrawContext const& ctx ) override;

    private:

        KRG_EXPOSE int32 m_value;
    };

    //-------------------------------------------------------------------------

    class Tools_ConstFloatNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_ConstFloatNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Float"; }
        virtual char const* GetCategory() const override { return "Const Values"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;
        virtual void DrawExtraControls( GraphEditor::DrawContext const& ctx ) override;

    private:

        KRG_EXPOSE float m_value;
    };

    //-------------------------------------------------------------------------

    class Tools_ConstVectorNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_ConstVectorNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Vector"; }
        virtual char const* GetCategory() const override { return "Const Values"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;
        virtual void DrawExtraControls( GraphEditor::DrawContext const& ctx ) override;

    private:

        KRG_EXPOSE Float4 m_value;
    };

    //-------------------------------------------------------------------------

    class Tools_ConstTargetNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_ConstTargetNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Target"; }
        virtual char const* GetCategory() const override { return "Const Values"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;
        virtual void DrawExtraControls( GraphEditor::DrawContext const& ctx ) override;

    private:

        KRG_EXPOSE Transform m_value;
    };
}