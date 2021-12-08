#include "AIPhysicsState_Ground.h"
#include "Engine/Physics/Systems/WorldSystem_Physics.h"
#include "Engine/Physics/Components/Component_PhysicsCharacter.h"

//------------------------------------------------------------------------- 

namespace KRG::AI
{
    void GroundPhysicsState::Activate()
    {
    }

    void GroundPhysicsState::Deactivate()
    {
    }

    bool GroundPhysicsState::TryMoveCapsule( Physics::PhysicsWorldSystem* pPhysicsSystem, Physics::CharacterComponent* pCharacterComponent, Seconds const deltaTime, Vector const& deltaTranslation, Quaternion const& deltaRotation )
    {
        Transform const newTransform = Transform( deltaRotation, deltaTranslation ) * pCharacterComponent->GetWorldTransform();
        pCharacterComponent->MoveCharacter( deltaTime, newTransform );
        return true;
    }
}