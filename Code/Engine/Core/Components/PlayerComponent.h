#pragma once

#include "Engine/Core/_Module/API.h"
#include "Engine/Core/Entity/EntitySpatialComponent.h"

//-------------------------------------------------------------------------
// Player Component
//-------------------------------------------------------------------------
// This component identifies all player entities and provides common data needed for player controller systems

namespace KRG
{
    class KRG_ENGINE_CORE_API PlayerComponent : public EntityComponent
    {
        KRG_REGISTER_SINGLETON_ENTITY_COMPONENT( PlayerComponent );

    public:

        inline PlayerComponent() = default;
        inline PlayerComponent( StringID name ) : EntityComponent( UUID::GenerateID(), name ) {}

        inline bool IsPlayerEnabled() const { return m_isEnabled; }
        inline void SetPlayerEnabled( bool isEnabled ) { m_isEnabled = isEnabled; }

    private:

        bool m_isEnabled = true;
    };
}