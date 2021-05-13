#pragma once

#include "Applications/Editor/Editor/EditorFileTab.h"
#include "System/DevTools/ImguiX.h"
#include "Engine/Animation/AnimationSkeleton.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Tools
{
    class SkeletonResourceFile : public TResourceFileTab<Skeleton>
    {
    public:

        using TResourceFileTab::TResourceFileTab;
    
        virtual void SetActive( EntityWorld* pPreviewWorld ) override;
        virtual void SetInactive( EntityWorld* pPreviewWorld ) override;
        virtual void DrawTools( UpdateContext const& context, Render::ViewportManager& viewportManager ) override;
    };
}