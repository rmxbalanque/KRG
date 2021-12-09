#include "AIPhysicsController.h"
#include "Engine/Physics/Systems/WorldSystem_Physics.h"
#include "Engine/Physics/Components/Component_PhysicsCharacter.h"
#include "System/Core/Math/Transform.h"

//-------------------------------------------------------------------------

namespace KRG::AI
{
    bool CharacterPhysicsController::TryMoveCapsule( Physics::PhysicsWorldSystem* pPhysicsSystem, Physics::CharacterComponent* pCharacterComponent, Seconds const deltaTime, Vector const& deltaTranslation, Quaternion const& deltaRotation )
    {
        Transform const newTransform = Transform( deltaRotation, deltaTranslation ) * pCharacterComponent->GetWorldTransform();
        pCharacterComponent->MoveCharacter( deltaTime, newTransform );
        return true;
    }
}