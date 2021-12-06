#pragma once
#include "Engine/Animation/Graph/AnimationGraphController.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    class PlayerStateGraphController final : public Animation::GraphController
    {
    public:

        KRG_ANIM_GRAPH_CONTROLLER_ID( PlayerStateGraphController );

    public:

        PlayerStateGraphController( Animation::AnimationGraphComponent* pGraphComponent );

    private:

        ControlParameter<StringID>     m_playerStateParam = ControlParameter<StringID>( "PlayerState" );
    };
}