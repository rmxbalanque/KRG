#include "WorldSystem.h"
#include "DevelopmentPlayerSystem.h"
#include "Engine/Core/Components/PlayerSpawnComponent.h"
#include "Engine/Core/Components/CameraComponents.h"
#include "Engine/Core/Components/PlayerComponent.h"
#include "Engine/Core/Entity/Entity.h"
#include "Engine/Core/Entity/EntityUpdateContext.h"
#include "Engine/Core/Entity/EntityMap.h"
#include "Engine/Core/Entity/EntityCollection.h"
#include "System/TypeSystem/TypeRegistry.h"

//-------------------------------------------------------------------------

namespace KRG
{
    void WorldSystem::RegisterComponent( Entity const* pEntity, EntityComponent* pComponent )
    {
        if ( auto pPlayerComponent = TryCast<PlayerComponent>( pComponent ) )
        {
            auto pPlayerRecord = m_players.FindOrAddRecord( pEntity->GetID() );
            KRG_ASSERT( pPlayerRecord->m_pPlayerComponent == nullptr );
            pPlayerRecord->m_pPlayerComponent = pPlayerComponent;
            pPlayerRecord->m_pPlayerComponent->SetPlayerEnabled( false );
            m_registeredPlayerStateChanged = true;
        }
        else if ( auto pCameraComponent = TryCast<CameraComponent>( pComponent ) )
        {
            auto pPlayerRecord = m_players.FindOrAddRecord( pEntity->GetID() );
            KRG_ASSERT( pPlayerRecord->m_pCameraComponent == nullptr );
            pPlayerRecord->m_pCameraComponent = pCameraComponent;
            m_registeredPlayerStateChanged = true;
        }
        else if ( auto pSpawnComponent = TryCast<PlayerSpawnComponent>( pComponent ) )
        {
            m_spawnPoints.emplace_back( pSpawnComponent );
        }
    }

    void WorldSystem::UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent )
    {
        if ( auto pPlayerComponent = TryCast<PlayerComponent>( pComponent ) )
        {
            auto pFoundRecord = m_players.GetRecord( pEntity->GetID() );
            pFoundRecord->m_pPlayerComponent = nullptr;
            m_registeredPlayerStateChanged = true;

            if ( pFoundRecord->m_pPlayerComponent == nullptr && pFoundRecord->m_pCameraComponent == nullptr )
            {
                m_players.RemoveRecord( pEntity->GetID() );
            }
        }
        else if ( auto pCameraComponent = TryCast<CameraComponent>( pComponent ) )
        {
            auto pFoundRecord = m_players.GetRecord( pEntity->GetID() );
            pFoundRecord->m_pPlayerComponent = nullptr;
            m_registeredPlayerStateChanged = true;

            if ( pFoundRecord->m_pPlayerComponent == nullptr && pFoundRecord->m_pCameraComponent == nullptr )
            {
                m_players.RemoveRecord( pEntity->GetID() );
            }

            if ( m_pActiveCamera == pComponent )
            {
                m_pActiveCamera = nullptr;
            }
        }
        else if ( auto pSpawnComponent = TryCast<PlayerSpawnComponent>( pComponent ) )
        {
            m_spawnPoints.erase_first_unsorted( pSpawnComponent );
        }
    }

    //-------------------------------------------------------------------------

    #if KRG_DEVELOPMENT_TOOLS
    void WorldSystem::SetPlayerEnabled( bool isEnabled )
    {
        m_disableAllPlayers = !isEnabled;

        if ( auto pActivePlayer = GetActivePlayer() )
        {
            if ( pActivePlayer->m_pPlayerComponent != nullptr )
            {
                pActivePlayer->m_pPlayerComponent->SetPlayerEnabled( !m_disableAllPlayers );
            }
        }
    }

    void WorldSystem::SetDevelopmentPlayerEnabled( bool enableDevelopmentPlayer )
    {
        if ( enableDevelopmentPlayer != m_useDevelopmentPlayer )
        {
            // If we should use the development player
            if ( enableDevelopmentPlayer )
            {
                SwitchToPlayer( m_developmentPlayerID );
            }
            else // Switch back to regular player if possible
            {
                if ( m_previouslyActivePlayerID.IsValid() )
                {
                    SwitchToPlayer( m_previouslyActivePlayerID );
                }
                else
                {
                    SwitchToLastValidPlayer();
                }
            }

            m_useDevelopmentPlayer = enableDevelopmentPlayer;
        }
    }
    #endif

    //-------------------------------------------------------------------------

    void WorldSystem::UpdateSystem( EntityUpdateContext const& ctx )
    {
        // Spawning
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        if ( !m_hasSpawnedDevelopmentPlayer )
        {
            SpawnDevelopmentPlayer( ctx );
            m_hasSpawnedDevelopmentPlayer = true;
        }
        #endif

        //-------------------------------------------------------------------------

        if ( ctx.IsGameWorld() && !m_hasSpawnedPlayer )
        {
            m_hasSpawnedPlayer = TrySpawnPlayer( ctx );
        }

        // Handle players spawning / despawning
        //-------------------------------------------------------------------------

        if ( m_registeredPlayerStateChanged )
        {
            auto pActivePlayer = GetActivePlayer();
            if ( pActivePlayer == nullptr || !pActivePlayer->IsValid() )
            {
                SwitchToLastValidPlayer();
            }
            else
            {
                #if KRG_DEVELOPMENT_TOOLS
                if ( m_activePlayerID == m_developmentPlayerID && !m_useDevelopmentPlayer )
                {
                    SwitchToLastValidPlayer();
                }
                #endif
            }

            m_registeredPlayerStateChanged = false;
        }
    }

    //-------------------------------------------------------------------------

    WorldSystem::RegisteredPlayer const* WorldSystem::GetActivePlayer() const
    {
        if ( !m_activePlayerID.IsValid() )
        {
            return nullptr;
        }

        for ( auto& player : m_players )
        {
            if ( player.m_pPlayerComponent->GetID() == m_activePlayerID )
            {
                return &player;
            }
        }

        return nullptr;
    }

    WorldSystem::RegisteredPlayer const* WorldSystem::GetPlayer( UUID const& playerID ) const
    {
        for ( auto& player : m_players )
        {
            if ( player.m_pPlayerComponent->GetID() == playerID )
            {
                return &player;
            }
        }

        // Invalid player ID supplied
        KRG_UNREACHABLE_CODE();
        return nullptr;
    }

    void WorldSystem::SwitchToLastValidPlayer()
    {
        RegisteredPlayer* pValidPlayer = nullptr;
        for ( auto& player : m_players )
        {
            if ( player.IsValid() )
            {
                pValidPlayer = &player;
            }
        }

        //-------------------------------------------------------------------------

        SetPlayerActive( *pValidPlayer );
    }

    void WorldSystem::SwitchToPlayer( UUID const& playerID )
    {
        KRG_ASSERT( playerID.IsValid() );

        for ( auto& player : m_players )
        {
            if ( player.m_pPlayerComponent->GetID() != playerID )
            {
                continue;
            }

            //-------------------------------------------------------------------------

            if ( player.IsValid() )
            {
                SetPlayerActive( player );
            }
            else
            {
                KRG_LOG_ERROR( "Player", "Cant switch to request player since they are in an invalid state!" );
            }

            return;
        }

        // Invalid player ID supplied
        KRG_UNREACHABLE_CODE();
    }

    void WorldSystem::SetPlayerActive( RegisteredPlayer& newPlayer )
    {
        KRG_ASSERT( newPlayer.IsValid() );

        m_previouslyActivePlayerID = m_activePlayerID;
        if ( m_previouslyActivePlayerID.IsValid() )
        {
            auto pPreviousPlayer = GetPlayer( m_previouslyActivePlayerID );
            if ( pPreviousPlayer->IsValid() )
            {
                pPreviousPlayer->m_pPlayerComponent->SetPlayerEnabled( false );
            }

            #if KRG_DEVELOPMENT_TOOLS
            if ( newPlayer.m_pPlayerComponent->GetID() == m_developmentPlayerID )
            {
                // Transform camera position/orientation to dev player
                newPlayer.m_pCameraComponent->SetWorldTransform( pPreviousPlayer->m_pCameraComponent->GetWorldTransform() );
            }
            #endif
        }

        //-------------------------------------------------------------------------

        m_activePlayerID = newPlayer.m_pPlayerComponent->GetID();
        m_pActiveCamera = newPlayer.m_pCameraComponent;
        newPlayer.m_pPlayerComponent->SetPlayerEnabled( !m_disableAllPlayers );
    }

    //-------------------------------------------------------------------------

    bool WorldSystem::TrySpawnPlayer( EntityUpdateContext const& ctx )
    {
        if ( m_spawnPoints.empty() )
        {
            return false;
        }

        auto pTypeRegistry = ctx.GetSystem<TypeSystem::TypeRegistry>();
        auto pPersistentMap = ctx.GetPersistentMap();
        //auto pCameraWorldSystem = ctx.GetWorldSystem<CameraWorldSystem>();

        //-------------------------------------------------------------------------

        // For now we only support a single spawn point
        EntityModel::EntityCollection ec( *pTypeRegistry, UUID::GenerateID(), *m_spawnPoints[0]->GetEntityCollectionDesc() );
        pPersistentMap->AddEntityCollection( m_spawnPoints[0]->GetWorldTransform(), ec );
        return true;
    }

    #if KRG_DEVELOPMENT_TOOLS
    void WorldSystem::SpawnDevelopmentPlayer( EntityUpdateContext const& ctx )
    {
        auto pCameraComponent = KRG::New<FreeLookCameraComponent>( StringID( "Debug Camera Component" ) );
        pCameraComponent->SetPositionAndLookatTarget( Vector( 0, 5, 5 ), Vector( 0, 0, 0 ) );
        auto pPlayerComponent = KRG::New<PlayerComponent>( StringID( "Debug Player Component" ) );

        auto pEntity = KRG::New<Entity>( StringID( "Debug Player" ) );
        pEntity->AddComponent( pCameraComponent );
        pEntity->AddComponent( pPlayerComponent );
        pEntity->CreateSystem<DevelopmentPlayerSystem>();

        ctx.GetPersistentMap()->AddEntity( pEntity );
        m_developmentPlayerID = pPlayerComponent->GetID();
    }
    #endif
}