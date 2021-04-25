#pragma once
#include "AnimationToolkit_Model.h"
#include "Applications/Editor/EditorTool.h"
#include "Applications/Editor/Common/Timeline/TimelineEditor.h"
#include "System/DevTools/ThirdParty/imgui/imgui_internal.h"

//-------------------------------------------------------------------------

namespace KRG::Animation 
{
    class AnimationClip;
    class AnimationPlayerComponent; 
}

//-------------------------------------------------------------------------

namespace KRG::Animation::AnimationTools
{
    class AnimationTrackEditor;

    //-------------------------------------------------------------------------

    class AnimationEditor : public TEditorTool<Model>
    {

    public:

        AnimationEditor( EditorModel* pModel ) : TEditorTool<Model>( pModel ) {}
        ~AnimationEditor();

    private:

        virtual char const * const GetName() { return "Animation Timeline"; }
        virtual void Initialize( UpdateContext const& context ) override;
        virtual void Shutdown( UpdateContext const& context ) override;
        virtual void FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager ) override;

        void DrawAnimationInfo( UpdateContext const& context );
        void DrawAnimationTimeline( UpdateContext const& context );

    private:

        ResourceID                              m_previewedItem;
        TVector<AnimationTrackEditor*>          m_trackEditors;
    };
}