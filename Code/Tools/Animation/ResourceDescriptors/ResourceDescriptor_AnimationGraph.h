#pragma once

#include "Tools/Animation/_Module/API.h"
#include "Tools/Core/Resource/Compilers/ResourceDescriptor.h"
#include "Engine/Animation/Graph/Animation_RuntimeGraph_Resources.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    namespace Graph
    {
        class AnimationGraphEditorDefinition; 
        class EditorGraphCompilationContext;
    }

    //-------------------------------------------------------------------------

    struct KRG_TOOLS_ANIMATION_API AnimationGraphResourceDescriptor final : public Resource::ResourceDescriptor
    {
        KRG_REGISTER_TYPE( AnimationGraphResourceDescriptor );

        virtual bool IsUserCreateableDescriptor() const override { return true; }
        virtual ResourceTypeID GetCompiledResourceTypeID() const override { return AnimationGraphDefinition::GetStaticResourceTypeID(); }
    };

    //-------------------------------------------------------------------------

    struct KRG_TOOLS_ANIMATION_API AnimationGraphVariationResourceDescriptor final : public Resource::ResourceDescriptor
    {
        KRG_REGISTER_TYPE( AnimationGraphVariationResourceDescriptor );

    public:

        KRG_EXPOSE ResourcePath                 m_graphPath;
        KRG_EXPOSE StringID                     m_variationID; // Optional: if not set, will use the default variation
    };
}