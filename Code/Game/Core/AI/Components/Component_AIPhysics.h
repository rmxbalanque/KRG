#pragma once

#include "Game/Core/GameplayPhysics/Component_GameplayPhysics.h"

//-------------------------------------------------------------------------

namespace KRG::AI
{
    class KRG_GAME_CORE_API AIPhysicsComponent final : public GameplayPhysics::GameplayPhysicsComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( AIPhysicsComponent );

    protected:

        virtual void CreatePhysicsStatesInternal() override;
    };
}