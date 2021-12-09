#pragma once

#include "Game/Core/_Module/API.h"
#include "Engine/Core/Components/Component_Player.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    class KRG_GAME_CORE_API MainPlayerComponent : public PlayerComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( MainPlayerComponent );

    public:

        bool m_sprintFlag = false;
    };
}