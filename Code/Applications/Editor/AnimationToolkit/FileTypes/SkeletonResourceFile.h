#pragma once

#include "Applications/Editor/Editor/EditorFile.h"
#include "System/DevTools/ImguiX.h"
#include "Engine/Animation/AnimationSkeleton.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::AnimationTools
{
    class SkeletonResourceFile : public TResourceFile<Skeleton>
    {
    public:

        using TResourceFile::TResourceFile;
    
        virtual void SetActive( EntityWorld* pPreviewWorld ) override;
        virtual void SetInactive( EntityWorld* pPreviewWorld ) override;
        virtual void DrawTools( UpdateContext const& context, Render::ViewportManager& viewportManager ) override;
    };
}