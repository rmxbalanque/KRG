#pragma once

#include "Tools/Core/PropertyGrid/PropertyGrid.h"
#include "Tools/Core/Workspaces/ResourceWorkspace.h"
#include "Engine/Animation/AnimationClip.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class SkeletalMeshComponent;
}

namespace KRG::Animation
{
    class AnimationClipPlayerComponent;
    class EventEditor;

    //-------------------------------------------------------------------------

    class AnimationClipWorkspace : public TResourceWorkspace<AnimationClip>
    {
    public:

        AnimationClipWorkspace( WorkspaceInitializationContext const& context, EntityWorld* pWorld, ResourceID const& resourceID );
        virtual ~AnimationClipWorkspace();

    private:

        virtual void Initialize( UpdateContext const& context ) override;
        virtual void Shutdown( UpdateContext const& context ) override;
        virtual void BeginHotReload( TVector<Resource::ResourceRequesterID> const& usersToBeReloaded, TVector<ResourceID> const& resourcesToBeReloaded ) override;
        virtual void EndHotReload() override;
        virtual void InitializeDockingLayout( ImGuiID dockspaceID ) const override;
        virtual void DrawUI( UpdateContext const& context, ImGuiWindowClass* pWindowClass ) override;

        virtual bool HasViewportToolbar() const override { return true; }
        virtual void DrawViewportToolbar( UpdateContext const& context, Render::Viewport const* pViewport ) override;

        virtual bool IsDirty() const override;
        virtual bool Save() override;

        void DrawTimelineWindow( UpdateContext const& context );
        void DrawTrackDataWindow( UpdateContext const& context );

        void CreatePreviewEntity();

    private:

        String                          m_timelineWindowName;
        String                          m_detailsWindowName;
        String                          m_trackDataWindowName;

        Entity*                         m_pPreviewEntity = nullptr;
        AnimationClipPlayerComponent*   m_pAnimationComponent = nullptr;
        Render::SkeletalMeshComponent*  m_pMeshComponent = nullptr;
        EventEditor*                    m_pEventEditor = nullptr;
        PropertyGrid                    m_propertyGrid;

        Transform                       m_characterTransform = Transform::Identity;
        ResourceID                      m_previewMeshOverride;
        bool                            m_isRootMotionEnabled = true;
    };
}