#pragma once

#include "Tools/Core/PropertyGrid/PropertyGrid.h"
#include "Tools/Core/Workspaces/ResourceWorkspace.h"
#include "Engine/Animation/AnimationClip.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class AnimationPlayerComponent;
    class AnimatedMeshComponent;
    class EventEditor;

    //-------------------------------------------------------------------------

    class AnimationClipWorkspace : public TResourceWorkspace<AnimationClip>
    {
    public:

        AnimationClipWorkspace( EditorContext const& context, EntityWorld* pWorld, ResourceID const& resourceID );
        virtual ~AnimationClipWorkspace();

    private:

        virtual void Initialize( UpdateContext const& context ) override;
        virtual void Shutdown( UpdateContext const& context ) override;
        virtual void InitializeDockingLayout( ImGuiID dockspaceID ) const override;
        virtual void UpdateAndDrawWindows( UpdateContext const& context, ImGuiWindowClass* pWindowClass ) override;

        virtual bool HasViewportToolbar() const override { return true; }
        virtual void DrawViewportToolbar( UpdateContext const& context, Render::Viewport const* pViewport ) override;

        virtual bool IsDirty() const override;
        virtual bool Save() override;

        void DrawTimelineWindow( UpdateContext const& context );
        void DrawTrackDataWindow( UpdateContext const& context );

    private:

        String                          m_timelineWindowName;
        String                          m_detailsWindowName;
        String                          m_trackDataWindowName;

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