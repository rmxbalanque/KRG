#pragma once

#include "Applications/Editor/Editor/EditorFile.h"
#include "Engine/Animation/AnimationClip.h"
#include "System/DevTools/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG::Animation { class AnimationPlayerComponent; }

//-------------------------------------------------------------------------

namespace KRG::Animation::AnimationTools
{
    class AnimationEventEditor;

    //-------------------------------------------------------------------------

    class AnimationResourceFile : public TResourceFile<AnimationClip>
    {
    public:

        using TResourceFile::TResourceFile;
        virtual ~AnimationResourceFile();
    
        virtual void SetActive( EntityWorld* pPreviewWorld ) override;
        virtual void SetInactive( EntityWorld* pPreviewWorld ) override;
        virtual void DrawTools( UpdateContext const& context, Render::ViewportManager& viewportManager ) override;

    private:

        void DrawAnimationInfo( UpdateContext const& context );
        void DrawAnimationTimeline( UpdateContext const& context );

    private:

        Entity*                         m_pPreviewEntity = nullptr;
        AnimationPlayerComponent*       m_pAnimationComponent = nullptr;
        AnimationEventEditor*           m_pTrackEditor = nullptr;
    };
}