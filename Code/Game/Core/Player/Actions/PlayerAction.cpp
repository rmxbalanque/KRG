#include "PlayerAction.h"
#include "Game/Core/Player/Components/Component_MainPlayer.h"
#include "Game/Core/Player/PlayerPhysicsController.h"
#include "Game/Core/Player/PlayerAnimationController.h"
#include "Engine/Physics/Components/Component_PhysicsCharacter.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    ActionContext::~ActionContext()
    {
        KRG_ASSERT( m_pEntityUpdateContext == nullptr && m_pInputSystem == nullptr && m_pPhysicsWorld == nullptr && m_pCharacterController == nullptr );
        KRG_ASSERT( m_pCharacterComponent == nullptr && m_pCharacterController == nullptr );
        KRG_ASSERT( m_pPlayerComponent == nullptr && m_pAnimationController == nullptr && m_pCameraController == nullptr );
    }

    bool ActionContext::IsValid() const
    {
        if ( m_pPlayerComponent == nullptr || !m_pPlayerComponent->IsPlayerEnabled() )
        {
            return false;
        }

        if ( m_pCharacterComponent == nullptr || m_pCharacterController == nullptr || !m_pCharacterComponent->IsRootComponent() )
        {
            return false;
        }

        if ( m_pAnimationController == nullptr || !m_pAnimationController->HasSubGraphControllers() )
        {
            return false;
        }

        return m_pEntityUpdateContext != nullptr && m_pCameraController != nullptr && m_pInputSystem != nullptr && m_pPhysicsWorld != nullptr && m_pCharacterController != nullptr;
    }
}