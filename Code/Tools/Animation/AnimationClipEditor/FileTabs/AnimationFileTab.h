#pragma once

#include "Tools/Core/PropertyGrid/PropertyGrid.h"
#include "Tools/Core/Editor/EditorFileTab.h"
#include "Tools/Core/Editor/EditorModel.h"
#include "Engine/Animation/AnimationClip.h"
#include "System/DevTools/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG::Animation { class AnimationPlayerComponent; }
namespace KRG::Animation { class EventEditor; }

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class AnimationFileTab : public TResourceFileTab<AnimationClip>
    {
    public:

        AnimationFileTab( EditorModel* pModel, Resource::ResourceSystem* pResourceSystem, ResourceID const& resourceID );
        virtual ~AnimationFileTab();
    
        virtual void Activate( EntityWorld* pPreviewWorld ) override;
        virtual void Deactivate( EntityWorld* pPreviewWorld ) override;
        virtual void DrawTools( UpdateContext const& context, Render::ViewportManager& viewportManager ) override;

    private:

        Entity*                         m_pPreviewEntity = nullptr;
        AnimationPlayerComponent*       m_pAnimationComponent = nullptr;
        EventEditor*                    m_pEventEditor = nullptr;
        PropertyGrid                    m_propertyGrid;
    };
}