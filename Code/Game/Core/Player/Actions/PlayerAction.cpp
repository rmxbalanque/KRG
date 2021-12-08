#include "PlayerAction.h"
#include "Engine/Animation/Graph/AnimationGraphController.h"
#include "Engine/Physics/Components/Component_PhysicsCharacter.h"
#include "Game/Core/Player/Components/Component_MainPlayer.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    ActionContext::~ActionContext()
    {
        KRG_ASSERT( m_pEntityUpdateContext == nullptr && m_pInputSystem == nullptr && m_pPhysicsWorld == nullptr && m_pCameraComponent == nullptr && m_pPhysicsController == nullptr );
        KRG_ASSERT( m_pPlayerComponent == nullptr && m_pCharacterPhysicsComponent == nullptr && m_pAnimationController == nullptr );
    }

    bool ActionContext::IsValid() const
    {
        if ( m_pPlayerComponent == nullptr || !m_pPlayerComponent->IsPlayerEnabled() )
        {
            return false;
        }

        if ( m_pCharacterPhysicsComponent == nullptr || !m_pCharacterPhysicsComponent->IsRootComponent() )
        {
            return false;
        }

        if ( m_pAnimationController == nullptr || !m_pAnimationController->HasSubGraphControllers() )
        {
            return false;
        }

        return m_pEntityUpdateContext != nullptr && m_pInputSystem != nullptr && m_pPhysicsWorld != nullptr && m_pCameraComponent != nullptr && m_pPhysicsController != nullptr;
    }
}