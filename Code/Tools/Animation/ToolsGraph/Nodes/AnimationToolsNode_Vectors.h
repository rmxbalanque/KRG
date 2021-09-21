#pragma once
#include "../AnimationToolsGraph_FlowNode.h"
#include "Engine/Animation/Graph/Nodes/AnimationGraphNode_Vectors.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class Tools_VectorInfoNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_VectorInfoNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Vector Info"; }
        virtual char const* GetCategory() const override { return "Vectors"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;

    private:

        EXPOSE VectorInfoNode::Info            m_desiredInfo = VectorInfoNode::Info::X;
    };

    //-------------------------------------------------------------------------

    class Tools_VectorNegateNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_VectorNegateNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Vector Negate"; }
        virtual char const* GetCategory() const override { return "Vectors"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;
    };
}