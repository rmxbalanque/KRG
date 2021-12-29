#pragma once
#include "Engine/Animation/Graph/Animation_RuntimeGraph_Controller.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    class AbilityGraphController final : public Animation::SubGraphController
    {
    public:

        KRG_ANIMATION_SUBGRAPH_CONTROLLER_ID( AbilityGraphController );

    public:

        AbilityGraphController( Animation::AnimationGraphComponent* pGraphComponent, Render::SkeletalMeshComponent* pMeshComponent );

        void StartJump();
        void StartDash();

    private:

        ControlParameter<StringID>     m_abilityID = ControlParameter<StringID>( "Ability_ID" );
    };
}