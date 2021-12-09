#include "AIBehavior.h"
#include "Engine/Animation/Graph/AnimationGraphController.h"
#include "Engine/Physics/Components/Component_PhysicsCharacter.h"
#include "Game/Core/Player/Components/Component_MainPlayer.h"

//-------------------------------------------------------------------------

namespace KRG::AI
{
    BehaviorContext::~BehaviorContext()
    {
        KRG_ASSERT( m_pEntityUpdateContext == nullptr && m_pNavmeshSystem == nullptr && m_pPhysicsWorld == nullptr && m_pCharacterPhysicsController == nullptr );
        KRG_ASSERT( m_pAIComponent == nullptr && m_pCharacterPhysicsComponent == nullptr && m_pAnimationController == nullptr );
    }

    bool BehaviorContext::IsValid() const
    {
        if ( m_pAIComponent == nullptr )
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

        return m_pEntityUpdateContext != nullptr && m_pNavmeshSystem != nullptr && m_pPhysicsWorld != nullptr && m_pCharacterPhysicsController != nullptr;
    }
}