#pragma once
#include "Tools/Animation/GraphEditor/EditorGraph/Animation_EditorGraph_FlowGraph.h"
#include "Engine/Animation/Graph/Nodes/Animation_RuntimeGraphNode_Floats.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::GraphNodes
{
    class FloatRemapEditorNode final : public EditorGraphNode
    {
        KRG_REGISTER_TYPE( FloatRemapEditorNode );

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Float Remap"; }
        virtual char const* GetCategory() const override { return "Values/Float"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual GraphNodeIndex Compile( EditorGraphCompilationContext& context ) const override;
        virtual void DrawInfoText( VisualGraph::DrawContext const& ctx ) override;

    public:

        KRG_EXPOSE FloatRange                  m_inputRange = FloatRange( 0.0f );
        KRG_EXPOSE FloatRange                  m_outputRange = FloatRange( 0.0f );
    };

    //-------------------------------------------------------------------------

    class FloatClampEditorNode final : public EditorGraphNode
    {
        KRG_REGISTER_TYPE( FloatClampEditorNode );

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Float Clamp"; }
        virtual char const* GetCategory() const override { return "Values/Float"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual GraphNodeIndex Compile( EditorGraphCompilationContext& context ) const override;

    public:

        KRG_EXPOSE FloatRange                  m_clampRange = FloatRange( 0 );
    };

    //-------------------------------------------------------------------------

    class FloatAbsEditorNode final : public EditorGraphNode
    {
        KRG_REGISTER_TYPE( FloatAbsEditorNode );

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Float Abs"; }
        virtual char const* GetCategory() const override { return "Values/Float"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual GraphNodeIndex Compile( EditorGraphCompilationContext& context ) const override;
    };

    //-------------------------------------------------------------------------

    class FloatEaseEditorNode final : public EditorGraphNode
    {
        KRG_REGISTER_TYPE( FloatEaseEditorNode );

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Float Ease"; }
        virtual char const* GetCategory() const override { return "Values/Float"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual GraphNodeIndex Compile( EditorGraphCompilationContext& context ) const override;

    public:

        KRG_EXPOSE Math::Easing::Type          m_easingType = Math::Easing::Type::Linear;
        KRG_EXPOSE float                       m_easeTime = 1.0f;
    };

    //-------------------------------------------------------------------------

    class FloatMathEditorNode final : public EditorGraphNode
    {
        KRG_REGISTER_TYPE( FloatMathEditorNode );

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Float Math"; }
        virtual char const* GetCategory() const override { return "Values/Float"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual GraphNodeIndex Compile( EditorGraphCompilationContext& context ) const override;

    public:

        KRG_EXPOSE bool                       m_returnAbsoluteResult = false;
        KRG_EXPOSE FloatMathNode::Operator    m_operator = FloatMathNode::Operator::Add;
        KRG_EXPOSE float                      m_valueB = 0.0f;
    };

    //-------------------------------------------------------------------------

    class FloatComparisonEditorNode final : public EditorGraphNode
    {
        KRG_REGISTER_TYPE( FloatComparisonEditorNode );

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Float Comparison"; }
        virtual char const* GetCategory() const override { return "Values/Float"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual GraphNodeIndex Compile( EditorGraphCompilationContext& context ) const override;
        virtual void DrawInfoText( VisualGraph::DrawContext const& ctx ) override;

    private:

        KRG_EXPOSE FloatComparisonNode::Comparison         m_comparison = FloatComparisonNode::Comparison::GreaterThanEqual;
        KRG_EXPOSE float                                   m_comparisonValue = 0.0f;
        KRG_EXPOSE float                                   m_epsilon = 0.0f;
    };

    //-------------------------------------------------------------------------

    class FloatRangeComparisonEditorNode final : public EditorGraphNode
    {
        KRG_REGISTER_TYPE( FloatRangeComparisonEditorNode );

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Float Range Check"; }
        virtual char const* GetCategory() const override { return "Values/Float"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual GraphNodeIndex Compile( EditorGraphCompilationContext& context ) const override;
        virtual void DrawInfoText( VisualGraph::DrawContext const& ctx ) override;

    private:

        KRG_EXPOSE  FloatRange                              m_range = FloatRange( 0, 1 );
        KRG_EXPOSE  bool                                    m_isInclusiveCheck = true;
    };

    //-------------------------------------------------------------------------

    class FloatSwitchEditorNode final : public EditorGraphNode
    {
        KRG_REGISTER_TYPE( FloatSwitchEditorNode );

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Float Switch"; }
        virtual char const* GetCategory() const override { return "Values/Float"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual GraphNodeIndex Compile( EditorGraphCompilationContext& context ) const override;
    };
}