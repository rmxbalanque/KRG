#pragma once
#include "../AnimationToolsGraph_FlowNode.h"
#include "Engine/Animation/Graph/Nodes/AnimationGraphNode_Floats.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class Tools_FloatRemapNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_FloatRemapNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Float Remap"; }
        virtual char const* GetCategory() const override { return "Floats"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;
        virtual void DrawInfoText( GraphEditor::DrawingContext const& ctx ) override;

    public:

        KRG_EXPOSE FloatRange                  m_inputRange = FloatRange( 0.0f );
        KRG_EXPOSE FloatRange                  m_outputRange = FloatRange( 0.0f );
    };

    //-------------------------------------------------------------------------

    class Tools_FloatClampNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_FloatClampNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Float Clamp"; }
        virtual char const* GetCategory() const override { return "Floats"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;

    public:

        KRG_EXPOSE FloatRange                  m_clampRange = FloatRange( 0 );
    };

    //-------------------------------------------------------------------------

    class Tools_FloatAbsNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_FloatAbsNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Float Abs"; }
        virtual char const* GetCategory() const override { return "Floats"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;
    };

    //-------------------------------------------------------------------------

    class Tools_FloatEaseNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_FloatEaseNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Float Ease"; }
        virtual char const* GetCategory() const override { return "Floats"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;

    public:

        KRG_EXPOSE Math::Easing::Type          m_easingType = Math::Easing::Type::Linear;
        KRG_EXPOSE float                       m_easeTime = 1.0f;
    };

    //-------------------------------------------------------------------------

    class Tools_FloatMathNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_FloatMathNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Float Math"; }
        virtual char const* GetCategory() const override { return "Floats"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;

    public:

        KRG_EXPOSE bool                       m_returnAbsoluteResult = false;
        KRG_EXPOSE FloatMathNode::Operator    m_operator = FloatMathNode::Operator::Add;
        KRG_EXPOSE float                      m_valueB = 0.0f;
    };

    //-------------------------------------------------------------------------

    class Tools_FloatComparisonNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_FloatComparisonNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Float Comparison"; }
        virtual char const* GetCategory() const override { return "Floats"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;
        virtual void DrawInfoText( GraphEditor::DrawingContext const& ctx ) override;

    private:

        KRG_EXPOSE FloatComparisonNode::Comparison         m_comparison = FloatComparisonNode::Comparison::GreaterThanEqual;
        KRG_EXPOSE float                                   m_epsilon = 0.0f;
        KRG_EXPOSE float                                   m_comparisonValue = 0.0f;
    };

    //-------------------------------------------------------------------------

    class Tools_FloatRangeComparisonNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_FloatRangeComparisonNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Float Range Check"; }
        virtual char const* GetCategory() const override { return "Floats"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;
        virtual void DrawInfoText( GraphEditor::DrawingContext const& ctx ) override;

    private:

        KRG_EXPOSE  FloatRange                              m_range = FloatRange( 0, 1 );
        KRG_EXPOSE  bool                                    m_isInclusiveCheck = true;
    };

    //-------------------------------------------------------------------------

    class Tools_FloatSwitchNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_FloatSwitchNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Float Switch"; }
        virtual char const* GetCategory() const override { return "Floats"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;
    };
}