#include "PlayerManagerWorldSystem.h"
#include "Engine/Core/Player/PlayerComponent.h"
#include "Engine/Core/Entity/Entity.h"
#include "Engine/Core/Entity/EntityUpdateContext.h"

//-------------------------------------------------------------------------

namespace KRG
{
    PlayerManagerSystem::RegisteredPlayer::RegisteredPlayer( UUID entityID, PlayerComponent* pComponent )
        : m_entityID( entityID )
        , m_pComponent( pComponent )
    {
        KRG_ASSERT( entityID.IsValid() && pComponent != nullptr );
    }

    //-------------------------------------------------------------------------

    void PlayerManagerSystem::RegisterComponent( Entity const* pEntity, EntityComponent* pComponent )
    {
        auto pPlayerComponent = ComponentCast<PlayerComponent>( pComponent );
        if ( pPlayerComponent != nullptr )
        {
            m_players.emplace_back( RegisteredPlayer( pEntity->GetID(), pPlayerComponent ) );
        }
    }

    void PlayerManagerSystem::UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent )
    {
        for ( auto i = 0; i < m_players.size(); i++ )
        {
            if ( m_players[i].m_entityID != pEntity->GetID() )
            {
                continue;
            }

            m_players.erase_unsorted( m_players.begin() + i );
            i--;
        }
    }

    //-------------------------------------------------------------------------

    #if KRG_DEVELOPMENT_TOOLS
    void PlayerManagerSystem::SetPlayerControllerState( bool isEnabled )
    {
        for ( auto& player : m_players )
        {
            player.m_pComponent->SetPlayerInputState( isEnabled );
        }
    }
    #endif

    //-------------------------------------------------------------------------

    void PlayerManagerSystem::UpdateSystem( EntityUpdateContext const& ctx )
    {
     
    }
}