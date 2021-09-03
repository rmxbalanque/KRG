#pragma once

#include "../ResourceWorkspace.h"
#include "Tools/Core/PropertyGrid/PropertyGrid.h"
#include "Engine/Animation/AnimationClip.h"
#include "System/DevTools/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG::Animation { class AnimationPlayerComponent; }
namespace KRG::Animation { class EventEditor; }

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class AnimationClipWorkspace : public Resource::TResourceWorkspace<AnimationClip>
    {
        static char const* const s_infoWindowName;

    public:

        AnimationClipWorkspace( EditorModel* pModel, Resource::ResourceSystem* pResourceSystem, ResourceID const& resourceID );
        virtual ~AnimationClipWorkspace();
    
        virtual char const* GetWorkspaceName() const override { return "Animation Clip"; }
        virtual void Activate( EntityWorld* pPreviewWorld ) override;
        virtual void Deactivate( EntityWorld* pPreviewWorld ) override;
        virtual void Draw( UpdateContext const& context, Render::ViewportManager& viewportManager, ImGuiWindowClass* pWindowClass ) override;

    private:

        Entity*                         m_pPreviewEntity = nullptr;
        AnimationPlayerComponent*       m_pAnimationComponent = nullptr;
        EventEditor*                    m_pEventEditor = nullptr;
        PropertyGrid                    m_propertyGrid;
    };
}