#pragma once

#include "Tools/Core/PropertyGrid/PropertyGrid.h"
#include "Tools/Core/Editors/ResourceEditorWorkspace.h"
#include "Engine/Animation/AnimationClip.h"
#include "System/Imgui/ImguiX.h"

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

        AnimationClipResourceEditor( EditorModel* pModel, ResourceID const& resourceID );
        virtual ~AnimationClipResourceEditor();
    
        virtual char const* GetWorkspaceName() const override { return "Animation Clip"; }
        virtual void Activate( EntityWorld* pPreviewWorld ) override;
        virtual void Deactivate( EntityWorld* pPreviewWorld ) override;

        virtual void InitializeDockingLayout( ImGuiID dockspaceID ) const override;
        virtual void UpdateAndDraw( UpdateContext const& context, Render::ViewportManager& viewportManager, ImGuiWindowClass* pWindowClass ) override;

        virtual bool HasViewportToolbar() const override { return true; }
        virtual void DrawViewportToolbar( UpdateContext const& context, Render::ViewportManager& viewportManager ) override;

    private:

        void DrawTimelineWindow( UpdateContext const& context, Render::ViewportManager& viewportManager, ImGuiWindowClass* pWindowClass );
        void DrawDetailsWindow( UpdateContext const& context, Render::ViewportManager& viewportManager, ImGuiWindowClass* pWindowClass );
        void DrawTrackDataWindow( UpdateContext const& context, Render::ViewportManager& viewportManager, ImGuiWindowClass* pWindowClass );

    private:

        Entity*                         m_pPreviewEntity = nullptr;
        AnimationPlayerComponent*       m_pAnimationComponent = nullptr;
        AnimatedMeshComponent*          m_pMeshComponent = nullptr;
        EventEditor*                    m_pEventEditor = nullptr;
        PropertyGrid                    m_propertyGrid;

        Transform                       m_characterTransform = Transform::Identity;
        bool                            m_isRootMotionEnabled = true;
    };
}