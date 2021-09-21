#pragma once
#include "../AnimationToolsGraph_FlowNode.h"
#include "Engine/Animation/Graph/Nodes/AnimationGraphNode_Targets.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class Tools_IsTargetSetNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_IsTargetSetNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Is Target Set"; }
        virtual char const* GetCategory() const override { return "Targets"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;
    };

    //-------------------------------------------------------------------------

    class Tools_TargetInfoNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_TargetInfoNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Target Info"; }
        virtual char const* GetCategory() const override { return "Targets"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;

    private:

        EXPOSE CoordinateSpace          m_expectedCoordinateSpace;
        EXPOSE TargetInfoNode::Info     m_infoType = TargetInfoNode::Info::Distance;
    };

    //-------------------------------------------------------------------------

    class Tools_TargetOffsetNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_TargetOffsetNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Target Offset"; }
        virtual char const* GetCategory() const override { return "Targets"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;

    private:

        EXPOSE bool                 m_isBoneSpaceOffset  = true;
        EXPOSE Quaternion           m_rotationOffset = Quaternion::Identity;
        EXPOSE Vector               m_translationOffset = Vector::Zero;
    };
}