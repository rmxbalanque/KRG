#pragma once

#include "Tools/Animation/_Module/API.h"
#include "Tools/Core/Resource/Compilers/ResourceDescriptor.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    namespace Graph
    {
        class AnimationToolsGraph; 
        class ToolsGraphCompilationContext;
    }

    //-------------------------------------------------------------------------

    struct KRG_TOOLS_ANIMATION_API AnimationGraphResourceDescriptor final : public Resource::ResourceDescriptor
    {
        KRG_REGISTER_TYPE( AnimationGraphResourceDescriptor );
    };

    //-------------------------------------------------------------------------

    struct KRG_TOOLS_ANIMATION_API AnimationGraphVariationResourceDescriptor final : public Resource::ResourceDescriptor
    {
        KRG_REGISTER_TYPE( AnimationGraphVariationResourceDescriptor );

        KRG_EXPOSE ResourcePath                 m_graphPath;
        KRG_EXPOSE StringID                     m_variationID; // Optional: if not set, will use the default variation
    };
}