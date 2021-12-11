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

        CharacterPhysicsController( Physics::CharacterComponent* pCharacterComponent )
            : m_pCharacterComponent( pCharacterComponent )
        {
            KRG_ASSERT( m_pCharacterComponent != nullptr );
        }

        bool TryMoveCapsule( Physics::PhysicsWorldSystem* pPhysicsSystem, Seconds const deltaTime, Vector const& deltaTranslation, Quaternion const& deltaRotation );

    public:

        Physics::CharacterComponent*        m_pCharacterComponent = nullptr;
    };
}