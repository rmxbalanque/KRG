#pragma once
#include "../AnimationToolsGraph_FlowNode.h"
#include "Engine/Animation/Graph/Nodes/AnimationGraphNode_StateConditions.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class Tools_StateCompletedConditionNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_StateCompletedConditionNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "State Completed"; }
        virtual char const* GetCategory() const override { return "State Conditions"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;
    };

    //-------------------------------------------------------------------------

    class Tools_TimeConditionNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_TimeConditionNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Time Condition"; }
        virtual char const* GetCategory() const override { return "State Conditions"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;

    private:

        KRG_EXPOSE float                                    m_comparand = 0.0f;
        KRG_EXPOSE TimeConditionNode::ComparisonType        m_type = TimeConditionNode::ComparisonType::ElapsedTime;
        KRG_EXPOSE TimeConditionNode::Operator              m_operator = TimeConditionNode::Operator::LessThan;
    };
}