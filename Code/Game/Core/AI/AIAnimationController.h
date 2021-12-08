#pragma once

#include "Game/Core/Player/GraphControllers/PlayerGraphController_Locomotion.h"

//-------------------------------------------------------------------------

namespace KRG::AI
{
    class AIAnimationController final : public Animation::GraphController
    {
    public:

        AIAnimationController( Animation::AnimationGraphComponent* pGraphComponent, Animation::AnimatedMeshComponent* pMeshComponent );

        #if KRG_DEVELOPMENT_TOOLS
        virtual char const* GetName() const { return "AI Graph Controller"; }
        #endif

    private:

        ControlParameter<StringID>     m_characterStateParam = "CharacterState";
    };
}