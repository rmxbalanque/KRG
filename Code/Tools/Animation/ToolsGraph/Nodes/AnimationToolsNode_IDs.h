#pragma once
#include "../AnimationToolsGraph_FlowNode.h"
#include "Engine/Animation/Graph/Nodes/AnimationGraphNode_IDs.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class Tools_IDComparisonNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_IDComparisonNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "ID Comparison"; }
        virtual char const* GetCategory() const override { return "IDs"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;

    private:

        EXPOSE IDComparisonNode::Comparison     m_comparison = IDComparisonNode::Comparison::Matches;
        EXPOSE TVector<StringID>                m_IDs;
    };

    //-------------------------------------------------------------------------

    class Tools_IDToFloatNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_IDToFloatNode );

        struct Mapping : public IRegisteredType
        {
            KRG_REGISTER_TYPE( Mapping );

            StringID    m_ID;
            float       m_value;
        };

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "ID to Float"; }
        virtual char const* GetCategory() const override { return "IDs"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;

    private:

        EXPOSE float                            m_defaultValue = 0.0f;
        EXPOSE TVector<Mapping>                 m_mappings;
    };
}