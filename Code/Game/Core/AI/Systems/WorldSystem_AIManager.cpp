#include "WorldSystem_AIManager.h"
#include "Game/Core/AI/Components/Component_AISpawn.h"
#include "Game/Core/AI/Components/Component_AI.h"
#include "Engine/Core/Entity/Entity.h"
#include "Engine/Core/Entity/EntityWorldUpdateContext.h"
#include "Engine/Core/Entity/EntityMap.h"
#include "Engine/Core/Entity/EntityCollection.h"
#include "System/TypeSystem/TypeRegistry.h"

//-------------------------------------------------------------------------

namespace KRG::AI
{
    void AIManager::ShutdownSystem()
    {
        KRG_ASSERT( m_spawnPoints.empty() );
    }

    void AIManager::RegisterComponent( Entity const* pEntity, EntityComponent* pComponent )
    {
        if ( auto pSpawnComponent = TryCast<AISpawnComponent>( pComponent ) )
        {
            m_spawnPoints.emplace_back( pSpawnComponent );
        }

        if ( auto pAIComponent = TryCast<AIComponent>( pComponent ) )
        {
            m_AIs.emplace_back( pAIComponent );
        }
    }

    void AIManager::UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent )
    {
        if ( auto pSpawnComponent = TryCast<AISpawnComponent>( pComponent ) )
        {
            m_spawnPoints.erase_first_unsorted( pSpawnComponent );
        }

        if ( auto pAIComponent = TryCast<AIComponent>( pComponent ) )
        {
            m_AIs.erase_first( pAIComponent );
        }
    }

    //-------------------------------------------------------------------------

    void AIManager::UpdateSystem( EntityWorldUpdateContext const& ctx )
    {
        if ( ctx.IsGameWorld() && !m_hasSpawnedAI )
        {
            m_hasSpawnedAI = TrySpawnAI( ctx );
        }
    }

    bool AIManager::TrySpawnAI( EntityWorldUpdateContext const& ctx )
    {
        if ( m_spawnPoints.empty() )
        {
            return false;
        }

        auto pTypeRegistry = ctx.GetSystem<TypeSystem::TypeRegistry>();
        auto pPersistentMap = ctx.GetPersistentMap();

        //-------------------------------------------------------------------------

        for ( auto pSpawnPoint : m_spawnPoints )
        {
            EntityModel::EntityCollection ec( *pTypeRegistry, UUID::GenerateID(), *pSpawnPoint->GetEntityCollectionDesc() );
            pPersistentMap->AddEntityCollection( pSpawnPoint->GetWorldTransform(), ec );
        }

        return true;
    }
}