#pragma once

#include "../_Module/API.h"
#include "Engine/Core/Entity/EntityWorldSystem.h"
#include "Engine/Core/Entity/EntityIDs.h"
#include "System/Core/Types/IDVector.h"

//-------------------------------------------------------------------------
// Player Management World System
//-------------------------------------------------------------------------
// This is the core world system responsibly for management the core world state
// * Active cameras
// * Active players
// * World Settings, etc...

namespace KRG
{
    class CameraComponent;

    namespace Player
    {
        class PlayerComponent;
        class PlayerSpawnComponent;
    }

    //-------------------------------------------------------------------------

    class KRG_ENGINE_CORE_API PlayerManager : public IWorldEntitySystem
    {
        friend class CameraDebugView;

        //-------------------------------------------------------------------------

        struct RegisteredPlayer
        {
            RegisteredPlayer( EntityID const& entityID ) : m_entityID( entityID ) {};
            inline EntityID const& GetID() const { return m_entityID; }
            inline bool IsValid() const { return m_pPlayerComponent != nullptr && m_pCameraComponent != nullptr; }

        public:

            EntityID                            m_entityID;
            Player::PlayerComponent*            m_pPlayerComponent = nullptr;
            CameraComponent*                    m_pCameraComponent = nullptr;
        };

    public:

        KRG_REGISTER_TYPE( PlayerManager );
        KRG_ENTITY_WORLD_SYSTEM( PlayerManager, RequiresUpdate( UpdateStage::FrameStart, UpdatePriority::Highest ) );

        // Camera
        //-------------------------------------------------------------------------

        inline bool HasActiveCamera() const { return m_pActiveCamera != nullptr; }
        inline CameraComponent* GetActiveCamera() const { return m_pActiveCamera; }

        // Player
        //-------------------------------------------------------------------------

        inline bool HasActivePlayer() const { return m_activePlayerID.IsValid(); }

        #if KRG_DEVELOPMENT_TOOLS
        void SetPlayerEnabled( bool isEnabled );
        void SetDevelopmentPlayerEnabled( bool enableDevelopmentPlayer );
        #endif

    private:

        virtual void ShutdownSystem() override final;
        virtual void RegisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;
        virtual void UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;
        virtual void UpdateSystem( EntityUpdateContext const& ctx ) override;

        // Player
        //-------------------------------------------------------------------------
        
        RegisteredPlayer const* GetActivePlayer() const;
        RegisteredPlayer const* GetPlayer( ComponentID const& playerID ) const;

        void SwitchToLastValidPlayer();
        void SwitchToPlayer( ComponentID const& playerID );
        bool TrySpawnPlayer( EntityUpdateContext const& ctx );

        void SetPlayerActive( RegisteredPlayer& player );

        #if KRG_DEVELOPMENT_TOOLS
        void SpawnDevelopmentPlayer( EntityUpdateContext const& ctx );
        #endif

    private:

        TIDVector<EntityID, RegisteredPlayer>   m_players;
        TVector<Player::PlayerSpawnComponent*>  m_spawnPoints;
        CameraComponent*                        m_pActiveCamera = nullptr;
        ComponentID                             m_activePlayerID;
        ComponentID                             m_previouslyActivePlayerID;
        bool                                    m_hasSpawnedPlayer = false;
        bool                                    m_registeredPlayerStateChanged = false;
        bool                                    m_disableAllPlayers = false;

        #if KRG_DEVELOPMENT_TOOLS
        bool                                    m_hasSpawnedDevelopmentPlayer = false;
        bool                                    m_useDevelopmentPlayer = false;
        ComponentID                             m_developmentPlayerID;
        #endif
    };
} 