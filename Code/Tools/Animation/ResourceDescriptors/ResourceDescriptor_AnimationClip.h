#pragma once

#include "Tools/Animation/_Module/API.h"
#include "Tools/Core/Resource/Compilers/ResourceDescriptor.h"
#include "Engine/Animation/AnimationSkeleton.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    struct KRG_TOOLS_ANIMATION_API AnimationClipResourceDescriptor final : public Resource::ResourceDescriptor
    {
        KRG_REGISTER_TYPE( AnimationClipResourceDescriptor );

        KRG_EXPOSE ResourcePath                m_animationPath;
        KRG_EXPOSE TResourcePtr<Skeleton>      m_pSkeleton = nullptr;
        KRG_EXPOSE String                      m_animationName; // Optional: if not set, will use the first animation in the file
    };
}