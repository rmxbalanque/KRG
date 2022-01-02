#pragma once
#include "System/Core/Math/Quaternion.h"
#include "System/Core/Time/Time.h"

//-------------------------------------------------------------------------

namespace KRG { class EntityWorldUpdateContext; }

namespace KRG::Physics
{
    class CharacterComponent;
    class Scene;
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

        bool TryMoveCapsule( EntityWorldUpdateContext const& ctx, Physics::Scene* pPhysicsScene, Vector const& deltaTranslation, Quaternion const& deltaRotation );

    public:

        Physics::CharacterComponent*        m_pCharacterComponent = nullptr;
    };
}