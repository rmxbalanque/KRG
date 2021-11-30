#pragma once

#include "Game/Core/_Module/API.h"
#include "Engine/Core/Entity/EntityWorldSystem.h"
#include "System/Core/Types/IDVector.h"

//-------------------------------------------------------------------------

namespace KRG::AI
{
    class AISpawnComponent;

    //-------------------------------------------------------------------------

    class KRG_GAME_CORE_API AIManager : public IWorldEntitySystem
    {

    public:

        KRG_REGISTER_TYPE( AIManager );
        KRG_ENTITY_WORLD_SYSTEM( AIManager, RequiresUpdate( UpdateStage::PrePhysics ) );

    private:

        virtual void ShutdownSystem() override final;
        virtual void RegisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;
        virtual void UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;
        virtual void UpdateSystem( EntityUpdateContext const& ctx ) override;

        bool TrySpawnAI( EntityUpdateContext const& ctx );

    private:

        TVector<AISpawnComponent*>          m_spawnPoints;
        bool                                m_hasSpawnedAI = false;
    };
} 