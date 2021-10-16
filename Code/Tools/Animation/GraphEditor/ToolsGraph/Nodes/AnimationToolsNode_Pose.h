#pragma once
#include "../AnimationToolsGraph_FlowNode.h"
#include "Engine/Animation/AnimationClip.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class Tools_ZeroPoseNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_ZeroPoseNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Zero Pose"; }
        virtual char const* GetCategory() const override { return "Poses"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;
    };

    //-------------------------------------------------------------------------

    class Tools_ReferencePoseNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_ReferencePoseNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Reference Pose"; }
        virtual char const* GetCategory() const override { return "Poses"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;
    };

    //-------------------------------------------------------------------------

    class Tools_AnimationPoseNode final : public Tools_DataSlotNode
    {
        KRG_REGISTER_TYPE( Tools_AnimationPoseNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetDisplayName() const override { return m_name.c_str(); }
        virtual char const* GetTypeName() const override { return "Animation Pose"; }
        virtual char const* GetCategory() const override { return "Poses"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;
        virtual ResourceTypeID GetSlotResourceType() const override { return AnimationClip::GetStaticResourceTypeID(); }

    private:

        KRG_EXPOSE String       m_name = "Pose";
        KRG_EXPOSE FloatRange   m_inputTimeRemapRange = FloatRange( 0, 1 );
    };
}