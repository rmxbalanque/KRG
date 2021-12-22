#pragma once
#include "Tools/Animation/GraphEditor/EditorGraph/Animation_EditorGraph_FlowGraph.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class ConstBoolEditorNode final : public EditorGraphNode
    {
        KRG_REGISTER_TYPE( ConstBoolEditorNode );

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Const Bool"; }
        virtual char const* GetCategory() const override { return "Values/Bool"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( EditorGraphCompilationContext& context ) const override;
        virtual void DrawExtraControls( VisualGraph::DrawContext const& ctx ) override;

    private:

        KRG_EXPOSE bool m_value;
    };

    //-------------------------------------------------------------------------

    class ConstIDEditorNode final : public EditorGraphNode
    {
        KRG_REGISTER_TYPE( ConstIDEditorNode );

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Const ID"; }
        virtual char const* GetCategory() const override { return "Values/ID"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( EditorGraphCompilationContext& context ) const override;
        virtual void DrawExtraControls( VisualGraph::DrawContext const& ctx ) override;

    private:

        KRG_EXPOSE StringID m_value;
    };

    //-------------------------------------------------------------------------
    
    class ConstIntEditorNode final : public EditorGraphNode
    {
        KRG_REGISTER_TYPE( ConstIntEditorNode );

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Const Int"; }
        virtual char const* GetCategory() const override { return "Values/Int"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( EditorGraphCompilationContext& context ) const override;
        virtual void DrawExtraControls( VisualGraph::DrawContext const& ctx ) override;

    private:

        KRG_EXPOSE int32 m_value;
    };

    //-------------------------------------------------------------------------

    class ConstFloatEditorNode final : public EditorGraphNode
    {
        KRG_REGISTER_TYPE( ConstFloatEditorNode );

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Const Float"; }
        virtual char const* GetCategory() const override { return "Values/Float"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( EditorGraphCompilationContext& context ) const override;
        virtual void DrawExtraControls( VisualGraph::DrawContext const& ctx ) override;

    private:

        KRG_EXPOSE float m_value;
    };

    //-------------------------------------------------------------------------

    class ConstVectorEditorNode final : public EditorGraphNode
    {
        KRG_REGISTER_TYPE( ConstVectorEditorNode );

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Const Vector"; }
        virtual char const* GetCategory() const override { return "Values/Vector"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( EditorGraphCompilationContext& context ) const override;
        virtual void DrawExtraControls( VisualGraph::DrawContext const& ctx ) override;

    private:

        KRG_EXPOSE Float4 m_value;
    };

    //-------------------------------------------------------------------------

    class ConstTargetEditorNode final : public EditorGraphNode
    {
        KRG_REGISTER_TYPE( ConstTargetEditorNode );

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Const Target"; }
        virtual char const* GetCategory() const override { return "Values/Target"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( EditorGraphCompilationContext& context ) const override;
        virtual void DrawExtraControls( VisualGraph::DrawContext const& ctx ) override;

    private:

        KRG_EXPOSE Transform m_value;
    };
}