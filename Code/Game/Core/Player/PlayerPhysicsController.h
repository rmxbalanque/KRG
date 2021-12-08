#pragma once

#include "Game/Core/Player/PhysicsStates/PlayerPhysicsState_Ground.h"
#include "Game/Core/Player/PhysicsStates/PlayerPhysicsState_Air.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    class PlayerPhysicsController final : public Physics::PhysicsStateController
    {
    public:

        PlayerPhysicsController()
        {
            m_registeredStates.emplace_back( KRG::New<GroundPhysicsState>() );
            m_registeredStates.emplace_back( KRG::New<AirPhysicsState>() );
        }
    };
}