#include "Component_PlayerGameplayPhysics.h"
#include "Game/Core/Player/PhysicsStates/PlayerPhysicsState_Locomotion.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    void PlayerGameplayPhysicsComponent::CreatePhysicsStatesInternal()
    {
        m_registeredStates.emplace_back( KRG::New<PlayerLocomotionPhysicsState>() );
        m_pActiveState = m_registeredStates.back();
        m_pActiveState->Activate();
    }
}