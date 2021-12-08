#pragma once

#include "Game/Core/AI/PhysicsStates/AIPhysicsState_Ground.h"

//-------------------------------------------------------------------------

namespace KRG::AI
{
    class AIPhysicsController final : public Physics::PhysicsStateController
    {
    public:

        AIPhysicsController::AIPhysicsController()
        {
            m_registeredStates.emplace_back( KRG::New<GroundPhysicsState>() );
            m_pActiveState = m_registeredStates.back();
            m_pActiveState->Activate();
        }
    };
}