#pragma once

#include "Tools/Core/PropertyGrid/PropertyGrid.h"
#include "Tools/Core/Resource/ResourceEditorWorkspace.h"
#include "Engine/Animation/AnimationClip.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class AnimationPlayerComponent;
    class AnimatedMeshComponent;
    class EventEditor;

    //-------------------------------------------------------------------------

    class AnimationClipResourceEditor : public TResourceEditorWorkspace<AnimationClip>
    {
        static char const* const s_timelineWindowName;
        static char const* const s_detailsWindowName;
        static char const* const s_trackDataWindowName;

    public:

        AnimationClipResourceEditor( ResourceEditorContext const& context, ResourceID const& resourceID );
        virtual ~AnimationClipResourceEditor();
    
        virtual char const* GetWorkspaceName() const override { return "Animation Clip"; }
        virtual void Activate( EntityWorld* pPreviewWorld ) override;
        virtual void Deactivate( EntityWorld* pPreviewWorld ) override;

        virtual void InitializeDockingLayout( ImGuiID dockspaceID ) const override;
        virtual void UpdateAndDraw( UpdateContext const& context, ImGuiWindowClass* pWindowClass ) override;

        virtual bool HasViewportToolbar() const override { return true; }
        virtual void DrawViewportToolbar( UpdateContext const& context, Render::Viewport const* pViewport ) override;

        virtual bool IsDirty() const override;

    private:

        virtual bool OnSave() override;

        void DrawTimelineWindow( UpdateContext const& context );
        void DrawTrackDataWindow( UpdateContext const& context );

    private:

        Entity*                         m_pPreviewEntity = nullptr;
        AnimationPlayerComponent*       m_pAnimationComponent = nullptr;
        AnimatedMeshComponent*          m_pMeshComponent = nullptr;
        EventEditor*                    m_pEventEditor = nullptr;
        PropertyGrid                    m_propertyGrid;

        Transform                       m_characterTransform = Transform::Identity;
        ResourceID                      m_previewMeshOverride;
        bool                            m_isRootMotionEnabled = true;
    };
}