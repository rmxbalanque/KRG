#pragma once

#include "Game/Core/GameplayPhysics/Component_GameplayPhysics.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    class KRG_GAME_CORE_API PlayerPhysicsComponent final : public GameplayPhysics::GameplayPhysicsComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( PlayerPhysicsComponent );

    protected:

        virtual void CreatePhysicsStatesInternal() override;
    };
}