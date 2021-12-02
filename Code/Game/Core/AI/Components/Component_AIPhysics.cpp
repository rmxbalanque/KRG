#include "Component_AIPhysics.h"
#include "Game/Core/AI/PhysicsStates/AIPhysicsState_AnimDriven.h"

//-------------------------------------------------------------------------

namespace KRG::AI
{
    void AIPhysicsComponent::CreatePhysicsStatesInternal()
    {
        m_registeredStates.emplace_back( KRG::New<AnimDrivenPhysicsState>() );
        m_pActiveState = m_registeredStates.back();
        m_pActiveState->Activate();
    }
}