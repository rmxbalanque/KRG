#pragma once

#include "Tools/Core/PropertyGrid/PropertyGrid.h"
#include "Tools/Core/Editors/ResourceEditorWorkspace.h"
#include "Engine/Animation/AnimationClip.h"
#include "System/Imgui/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class AnimationPlayerComponent;
    class EventEditor;

    //-------------------------------------------------------------------------

    class AnimationClipResourceEditor : public TResourceEditorWorkspace<AnimationClip>
    {
        static char const* const s_infoWindowName;

    public:

        AnimationClipResourceEditor( EditorModel* pModel, ResourceID const& resourceID );
        virtual ~AnimationClipResourceEditor();
    
        virtual char const* GetWorkspaceName() const override { return "Animation Clip"; }
        virtual void Activate( EntityWorld* pPreviewWorld ) override;
        virtual void Deactivate( EntityWorld* pPreviewWorld ) override;

        virtual void InitializeDockingLayout( ImGuiID dockspaceID ) const override;
        virtual void Draw( UpdateContext const& context, Render::ViewportManager& viewportManager, ImGuiWindowClass* pWindowClass ) override;

    private:

        Entity*                         m_pPreviewEntity = nullptr;
        AnimationPlayerComponent*       m_pAnimationComponent = nullptr;
        EventEditor*                    m_pEventEditor = nullptr;
        PropertyGrid                    m_propertyGrid;
    };
}