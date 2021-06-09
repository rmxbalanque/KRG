#pragma once

#include "Tools/Core/Editor/EditorFileTab.h"
#include "System/DevTools/ImguiX.h"
#include "Engine/Animation/AnimationSkeleton.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class SkeletonFileTab : public TResourceFileTab<Skeleton>
    {
    public:

        using TResourceFileTab::TResourceFileTab;
    
        virtual void Activate( EntityWorld* pPreviewWorld ) override;
        virtual void Deactivate( EntityWorld* pPreviewWorld ) override;
        virtual void DrawTools( UpdateContext const& context, Render::ViewportManager& viewportManager ) override;
    };
}