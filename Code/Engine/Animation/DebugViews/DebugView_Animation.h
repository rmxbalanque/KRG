#pragma once

#include "Engine/Animation/_Module/API.h"
#include "Engine/Core/Entity/EntityWorldDebugView.h"
#include "Engine/Core/Entity/EntityIDs.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Animation
{
    class AnimationWorldSystem;
    class AnimationGraphComponent;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API AnimationDebugView : public EntityWorldDebugView
    {
        KRG_REGISTER_TYPE( AnimationDebugView );

    public:

        AnimationDebugView();

    private:

        virtual void Initialize( SystemRegistry const& systemRegistry, EntityWorld const* pWorld ) override;
        virtual void Shutdown() override;
        virtual void DrawWindows( EntityUpdateContext const& context, ImGuiWindowClass* pWindowClass ) override;

        void DrawMenu( EntityUpdateContext const& context );

        bool DrawControlParameters( AnimationGraphComponent* pGraphComponent );

    private:

        EntityWorld const*                      m_pWorld = nullptr;
        AnimationWorldSystem*                   m_pAnimationWorldSystem = nullptr;
        TVector<ComponentID>                    m_activeControlParameterDebuggers;
    };
}
#endif