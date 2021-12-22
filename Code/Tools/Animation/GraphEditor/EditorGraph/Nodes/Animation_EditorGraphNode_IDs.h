#pragma once
#include "Tools/Animation/GraphEditor/EditorGraph/Animation_EditorGraph_FlowGraph.h"
#include "Engine/Animation/Graph/Nodes/Animation_RuntimeGraphNode_IDs.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class IDComparisonEditorNode final : public EditorGraphNode
    {
        KRG_REGISTER_TYPE( IDComparisonEditorNode );

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "ID Comparison"; }
        virtual char const* GetCategory() const override { return "Values/ID"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( EditorGraphCompilationContext& context ) const override;

        virtual void DrawInfoText( VisualGraph::DrawContext const& ctx ) override;

    private:

        KRG_EXPOSE IDComparisonNode::Comparison     m_comparison = IDComparisonNode::Comparison::Matches;
        KRG_EXPOSE TVector<StringID>                m_IDs;
    };

    //-------------------------------------------------------------------------

    class IDToFloatEditorNode final : public EditorGraphNode
    {
        KRG_REGISTER_TYPE( IDToFloatEditorNode );

        struct Mapping : public IRegisteredType
        {
            KRG_REGISTER_TYPE( Mapping );

            StringID    m_ID;
            float       m_value;
        };

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "ID to Float"; }
        virtual char const* GetCategory() const override { return "Values/ID"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( EditorGraphCompilationContext& context ) const override;

    private:

        KRG_EXPOSE float                            m_defaultValue = 0.0f;
        KRG_EXPOSE TVector<Mapping>                 m_mappings;
    };
}