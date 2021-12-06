#include "PlayerAction.h"
#include "Engine/Animation/Graph/AnimationGraphController.h"
#include "Engine/Physics/Components/Component_PhysicsCharacter.h"
#include "Game/Core/Player/Components/Component_MainPlayer.h"

//-------------------------------------------------------------------------

namespace KRG
{
    bool Player::ActionContext::IsValid() const
    {
        if ( m_pPlayerComponent == nullptr || !m_pPlayerComponent->IsPlayerEnabled() )
        {
            return false;
        }

        if ( m_pCharacterComponent == nullptr || !m_pCharacterComponent->IsRootComponent() )
        {
            return false;
        }

        if ( m_pAnimationControllerRegistry == nullptr || !m_pAnimationControllerRegistry->HasControllers() )
        {
            return false;
        }

        return m_pEntityUpdateContext != nullptr && m_pInputSystem != nullptr && m_pCameraComponent != nullptr && m_pPhysicsController != nullptr;
    }
}