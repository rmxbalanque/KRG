#pragma once
#include "Tools/Animation/GraphEditor/EditorGraph/Animation_EditorGraph_FlowGraph.h"
#include "Engine/Animation/AnimationClip.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::GraphNodes
{
    class AnimationClipEditorNode final : public DataSlotEditorNode
    {
        KRG_REGISTER_TYPE( AnimationClipEditorNode );

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        virtual char const* GetDisplayName() const override { return m_name.c_str(); }
        virtual char const* GetTypeName() const override { return "Animation Clip"; }
        virtual char const* GetCategory() const override { return "Animation"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree ); }
        virtual GraphNodeIndex Compile( EditorGraphCompilationContext& context ) const override;
        virtual ResourceTypeID GetSlotResourceTypeID() const override { return AnimationClip::GetStaticResourceTypeID(); }

    private:

        KRG_EXPOSE String       m_name = "Animation";
        KRG_EXPOSE bool         m_sampleRootMotion = true;
        KRG_EXPOSE bool         m_allowLooping = false;
    };
}