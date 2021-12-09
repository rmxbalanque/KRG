#pragma once
#include "System/Core/Math/Quaternion.h"
#include "System/Core/Time/Time.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class CharacterComponent;
    class PhysicsWorldSystem;
}

//-------------------------------------------------------------------------

namespace KRG::AI
{
    class CharacterPhysicsController final
    {
    public:

        bool TryMoveCapsule( Physics::PhysicsWorldSystem* pPhysicsSystem, Physics::CharacterComponent* pCharacterComponent, Seconds const deltaTime, Vector const& deltaTranslation, Quaternion const& deltaRotation );
    };
}