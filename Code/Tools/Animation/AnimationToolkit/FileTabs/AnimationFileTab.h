#pragma once

#include "Tools/Editor/Widgets/PropertyGrid/PropertyGrid.h"
#include "Tools/Editor/EditorFileTab.h"
#include "Tools/Editor/EditorModel.h"
#include "Engine/Animation/AnimationClip.h"
#include "System/DevTools/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG::Animation { class AnimationPlayerComponent; }
namespace KRG::Animation::Tools { class EventEditor; }

//-------------------------------------------------------------------------

namespace KRG::Animation::Tools
{
    class AnimationFileTab : public TResourceFileTab<AnimationClip>
    {
    public:

        AnimationFileTab( EditorModel* pModel, FileSystem::Path const& sourceDataDirectory, Resource::ResourceSystem* pResourceSystem, ResourceID const& resourceID );
        virtual ~AnimationFileTab();
    
        virtual void Activate( EntityWorld* pPreviewWorld ) override;
        virtual void Deactivate( EntityWorld* pPreviewWorld ) override;
        virtual void DrawTools( UpdateContext const& context, Render::ViewportManager& viewportManager ) override;

    private:

        Entity*                         m_pPreviewEntity = nullptr;
        AnimationPlayerComponent*       m_pAnimationComponent = nullptr;
        Tools::EventEditor*             m_pTrackEditor = nullptr;
        Editor::PropertyGrid            m_propertyGrid;
    };
}