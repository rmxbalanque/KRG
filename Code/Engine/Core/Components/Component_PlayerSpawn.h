#pragma once

#include "Engine/Core/_Module/API.h"
#include "Engine/Core/Entity/EntitySpatialComponent.h"
#include "Engine/Core/Entity/EntityCollectionDescriptor.h"
#include "System/Resource/ResourcePtr.h"

//-------------------------------------------------------------------------
// Player Spawn Component
//-------------------------------------------------------------------------
// This component defines the spawning location for the player in the game world

namespace KRG::Player
{
    class KRG_ENGINE_CORE_API PlayerSpawnComponent : public SpatialEntityComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( PlayerSpawnComponent );

    public:

        inline PlayerSpawnComponent() = default;

        inline EntityModel::EntityCollectionDescriptor const* GetEntityCollectionDesc() const { return m_pPlayerEntityDesc.GetPtr(); }

    private:

        KRG_EXPOSE TResourcePtr<EntityModel::EntityCollectionDescriptor>    m_pPlayerEntityDesc = nullptr;
    };
}