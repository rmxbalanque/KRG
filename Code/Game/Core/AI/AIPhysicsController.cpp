#include "AIPhysicsController.h"
#include "Engine/Physics/Systems/WorldSystem_Physics.h"
#include "Engine/Physics/Components/Component_PhysicsCharacter.h"
#include "System/Core/Math/Transform.h"

//-------------------------------------------------------------------------

namespace KRG::AI
{
    bool CharacterPhysicsController::TryMoveCapsule( Physics::PhysicsWorldSystem* pPhysicsSystem, Seconds const deltaTime, Vector const& deltaTranslation, Quaternion const& deltaRotation )
    {
        Transform newTransform = m_pCharacterComponent->GetWorldTransform();
        newTransform.AddTranslation( deltaTranslation );

        auto oq = m_pCharacterComponent->GetOrientation();
        Vector oup = Transform( oq ).GetUpVector();

        newTransform.AddRotation( deltaRotation );
        
        Vector nup = newTransform.GetUpVector();
        KRG_ASSERT( Vector::Dot3( nup, Vector::UnitZ ).ToFloat() > 0 );
        
        m_pCharacterComponent->MoveCharacter( deltaTime, newTransform );
        return true;
    }
}