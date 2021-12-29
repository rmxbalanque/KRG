#pragma once

#include "Tools/Animation/_Module/API.h"
#include "Tools/Core/Resource/Compilers/ResourceDescriptor.h"
#include "System/Animation/AnimationSkeleton.h"
#include "Engine/Render/Mesh/SkeletalMesh.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    struct KRG_TOOLS_ANIMATION_API SkeletonResourceDescriptor final : public Resource::ResourceDescriptor
    {
        KRG_REGISTER_TYPE( SkeletonResourceDescriptor );
    
        virtual bool IsUserCreateableDescriptor() const override { return true; }
        virtual ResourceTypeID GetCompiledResourceTypeID() const override{ return Skeleton::GetStaticResourceTypeID(); }

    public:

        KRG_EXPOSE ResourcePath                             m_skeletonPath;

        // Optional value that specifies the name of the skeleton hierarchy to use, if it is unset, we use the first skeleton we find
        KRG_EXPOSE String                                   m_skeletonRootBoneName;

        // Editor-only preview mesh
        KRG_EXPOSE TResourcePtr<Render::SkeletalMesh>       m_previewMesh;
    };
}