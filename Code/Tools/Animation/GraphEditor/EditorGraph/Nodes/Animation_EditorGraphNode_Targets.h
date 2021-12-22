#pragma once
#include "Tools/Animation/GraphEditor/EditorGraph/Animation_EditorGraph_FlowGraph.h"
#include "Engine/Animation/Graph/Nodes/Animation_RuntimeGraphNode_Targets.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class IsTargetSetEditorNode final : public EditorGraphNode
    {
        KRG_REGISTER_TYPE( IsTargetSetEditorNode );

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Is Target Set"; }
        virtual char const* GetCategory() const override { return "Values/Target"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( EditorGraphCompilationContext& context ) const override;
    };

    //-------------------------------------------------------------------------

    class TargetInfoEditorNode final : public EditorGraphNode
    {
        KRG_REGISTER_TYPE( TargetInfoEditorNode );

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Target Info"; }
        virtual char const* GetCategory() const override { return "Values/Target"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( EditorGraphCompilationContext& context ) const override;
        virtual void DrawInfoText( VisualGraph::DrawContext const& ctx ) override;

    private:

        KRG_EXPOSE TargetInfoNode::Info     m_infoType = TargetInfoNode::Info::Distance;
        KRG_EXPOSE bool                     m_isWorldSpaceTarget = true;
    };

    //-------------------------------------------------------------------------

    class TargetOffsetEditorNode final : public EditorGraphNode
    {
        KRG_REGISTER_TYPE( TargetOffsetEditorNode );

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Target Offset"; }
        virtual char const* GetCategory() const override { return "Values/Target"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( EditorGraphCompilationContext& context ) const override;

    private:

        KRG_EXPOSE bool                 m_isBoneSpaceOffset  = true;
        KRG_EXPOSE Quaternion           m_rotationOffset = Quaternion::Identity;
        KRG_EXPOSE Vector               m_translationOffset = Vector::Zero;
    };
}