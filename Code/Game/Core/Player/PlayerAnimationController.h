#pragma once
#include "Engine/Animation/Graph/Animation_RuntimeGraph_Controller.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    enum class CharacterAnimationState : uint8
    {
        Locomotion = 0,
        Falling,
        Ability,

        NumStates
    };

    //-------------------------------------------------------------------------

    class AnimationController final : public Animation::GraphController
    {

    public:

        AnimationController( Animation::AnimationGraphComponent* pGraphComponent, Animation::AnimatedMeshComponent* pMeshComponent );

        void SetCharacterState( CharacterAnimationState state );

        #if KRG_DEVELOPMENT_TOOLS
        virtual char const* GetName() const { return "Player Graph Controller"; }
        #endif

    private:

        ControlParameter<StringID>     m_characterStateParam = "CharacterState";
    };
}