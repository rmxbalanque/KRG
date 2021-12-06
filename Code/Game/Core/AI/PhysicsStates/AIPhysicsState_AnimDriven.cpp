#include "AIPhysicsState_AnimDriven.h"
#include "Engine/Physics/Systems/WorldSystem_Physics.h"
#include "Engine/Physics/Components/Component_PhysicsCharacter.h"

//------------------------------------------------------------------------- 

namespace KRG::AI
{
    void AnimDrivenPhysicsState::Activate()
    {
    }

    void AnimDrivenPhysicsState::Deactivate()
    {
    }

    bool AnimDrivenPhysicsState::TryMoveCapsule( Physics::PhysicsWorldSystem* pPhysicsSystem, Physics::CharacterComponent* pCharacterComponent, float const deltaTime, Transform const& deltaTransform )
    {
        Transform const newTransform = deltaTransform * pCharacterComponent->GetWorldTransform();
        pCharacterComponent->SetWorldTransform( newTransform );
        return true;
    }
}