#pragma once

#include "Applications/Editor/Editor/EditorFileTab.h"
#include "Applications/Editor/Common/PropertyGrid/PropertyGrid.h"
#include "Engine/Animation/AnimationClip.h"
#include "System/DevTools/ImguiX.h"
#include "Applications/Editor/Editor/EditorModel.h"
#include "Game/Core/TestComponent.h"

//-------------------------------------------------------------------------

namespace KRG::Animation { class AnimationPlayerComponent; }
namespace KRG::Animation::Tools { class EventEditor; }

//-------------------------------------------------------------------------

namespace KRG::Animation::Tools
{
    class AnimationResourceFile : public TResourceFileTab<AnimationClip>
    {
    public:

        AnimationResourceFile( EditorModel* pModel, FileSystem::Path const& sourceDataDirectory, Resource::ResourceSystem* pResourceSystem, ResourceID const& resourceID )
            : TResourceFileTab<AnimationClip>( pModel, sourceDataDirectory, pResourceSystem, resourceID)
            , m_propertyGrid( pModel )
        {
            m_propertyGrid.SetTypeToEdit( &m_testComponent );
        }

        virtual ~AnimationResourceFile();
    
        virtual void SetActive( EntityWorld* pPreviewWorld ) override;
        virtual void SetInactive( EntityWorld* pPreviewWorld ) override;
        virtual void DrawTools( UpdateContext const& context, Render::ViewportManager& viewportManager ) override;

    private:

        Entity*                         m_pPreviewEntity = nullptr;
        AnimationPlayerComponent*       m_pAnimationComponent = nullptr;
        Tools::EventEditor*             m_pTrackEditor = nullptr;
        TypeSystem::PropertyGrid       m_propertyGrid;
        TestComponent                   m_testComponent;
    };
}