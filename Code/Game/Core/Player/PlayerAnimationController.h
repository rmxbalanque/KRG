#pragma once

#include "Game/Core/Player/GraphControllers/PlayerGraphController_Locomotion.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    class PlayerAnimationController final : public Animation::GraphController
    {
    public:

        PlayerAnimationController( Animation::AnimationGraphComponent* pGraphComponent, Animation::AnimatedMeshComponent* pMeshComponent );

        #if KRG_DEVELOPMENT_TOOLS
        virtual char const* GetName() const { return "Player Graph Controller"; }
        #endif

    private:

        ControlParameter<StringID>     m_characterStateParam = "CharacterState";
    };
}