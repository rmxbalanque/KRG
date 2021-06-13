#pragma once
#include "../AnimationGraphTools_Node.h"
#include "Engine/Animation/AnimationClip.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class AnimationClipToolsNode final : public DataSlotNode
    {
        KRG_REGISTER_TYPE( AnimationClipToolsNode );

    public:

        AnimationClipToolsNode();

        virtual char const* GetDisplayName() const override { return m_name.c_str(); }
        virtual char const* GetTypeName() const override { return "Animation Clip"; }
        virtual char const* GetCategory() const override { return "Animations"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;
        virtual ResourceTypeID GetSlotResourceType() const override { return AnimationClip::GetStaticResourceTypeID(); }

    private:

        EXPOSE String       m_name = "Animation";
        EXPOSE bool         m_sampleRootMotion = true;
        EXPOSE bool         m_allowLooping = false;
    };
}