#pragma once

#include "Applications/Editor/EditorModel.h"
#include "System/Resource/ResourcePtr.h"

//-------------------------------------------------------------------------

namespace KRG { class Entity; }
namespace KRG::Animation { class AnimationSystem; class AnimationPlayerComponent; }

//-------------------------------------------------------------------------

namespace KRG::Animation::AnimationTools
{
    class Model final : public EditorModel
    {
    public:

        virtual void Initialize( UpdateContext const& context ) override;
        virtual void Shutdown( UpdateContext const& context ) override;

        inline Entity* GetPreviewEntity() { return m_pPreviewEntity; }
        inline AnimationPlayerComponent* GetPreviewAnimComponent() { return m_previewAnimationComponent; }

    private:

        virtual void OnSetActiveResource( ResourceID const& resourceID ) override;
        virtual void OnClearActiveResource() override;

    private:

        Entity*                             m_pPreviewEntity = nullptr;
        AnimationSystem*                    m_pPreviewAnimationSystem = nullptr;
        AnimationPlayerComponent*           m_previewAnimationComponent = nullptr;
    };
}