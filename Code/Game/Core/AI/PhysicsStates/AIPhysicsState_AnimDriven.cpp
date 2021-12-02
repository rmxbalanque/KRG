#include "AIPhysicsState_AnimDriven.h"
#include "Engine/Physics/Systems/WorldSystem_Physics.h"
#include "Engine/Physics/Components/Component_PhysicsCapsule.h"

//------------------------------------------------------------------------- 

namespace KRG::AI
{
    void AnimDrivenPhysicsState::Activate()
    {
    }

    void AnimDrivenPhysicsState::Deactivate()
    {
    }

    Transform AnimDrivenPhysicsState::TryMoveCapsule( Physics::PhysicsWorldSystem* pPhysicsSystem, Physics::CapsuleComponent const* pCapsuleComponent, float const deltaTime, Quaternion const& deltaRotation, Vector const& deltaTranslation )
    {
        Transform newTransform = pCapsuleComponent->GetWorldTransform();
        newTransform.SetRotation( deltaRotation * newTransform.GetRotation() );
        newTransform.AddTranslationOffset( deltaTranslation );
        return newTransform;
    }
}