#pragma once

#include "Game/Core/_Module/API.h"
#include "Engine/Core/Entity/EntityWorldSystem.h"
#include "System/Core/Types/IDVector.h"

//-------------------------------------------------------------------------

namespace KRG::AI
{
    class AISpawnComponent;
    class AIComponent;

    //-------------------------------------------------------------------------

    class KRG_GAME_CORE_API AIManager : public IWorldEntitySystem
    {
        friend class AIDebugView;

    public:

        KRG_REGISTER_TYPE( AIManager );
        KRG_ENTITY_WORLD_SYSTEM( AIManager, RequiresUpdate( UpdateStage::PrePhysics ) );

    private:

        virtual void ShutdownSystem() override final;
        virtual void RegisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;
        virtual void UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;
        virtual void UpdateSystem( EntityWorldUpdateContext const& ctx ) override;

        bool TrySpawnAI( EntityWorldUpdateContext const& ctx );

    private:

        TVector<AISpawnComponent*>          m_spawnPoints;
        TVector<AIComponent*>               m_AIs;
        bool                                m_hasSpawnedAI = false;
    };
} 