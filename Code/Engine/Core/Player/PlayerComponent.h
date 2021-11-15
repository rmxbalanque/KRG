#pragma once

#include "../_Module/API.h"
#include "Engine/Core/Entity/EntitySpatialComponent.h"

//-------------------------------------------------------------------------
// Player Component
//-------------------------------------------------------------------------
// This component identifies all player entities and provides common data needed for player controller systems

namespace KRG
{
    class KRG_ENGINE_CORE_API PlayerComponent : public EntityComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( PlayerComponent );

    public:

        inline PlayerComponent() = default;
        inline PlayerComponent( StringID name ) : EntityComponent( UUID::GenerateID(), name ) {}

        inline bool IsPlayerInputEnabled() const { return m_isInputEnabled; }
        inline void SetPlayerInputState( bool isInputEnabled ) { m_isInputEnabled = isInputEnabled; }

    private:

        bool m_isInputEnabled = true;
    };
}