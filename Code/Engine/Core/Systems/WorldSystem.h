#pragma once

#include "../_Module/API.h"
#include "Engine/Core/Entity/EntityWorldSystem.h"
#include "System/Render/RenderViewport.h"
#include "System/Core/Systems/ISystem.h"

//-------------------------------------------------------------------------
// World System
//-------------------------------------------------------------------------
// This is the core world system responsibly for management the core world state
// * Active cameras
// * Active players
// * World Settings, etc...

namespace KRG
{
    class PlayerComponent;
    class PlayerSpawnComponent;
    class CameraComponent;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_CORE_API WorldSystem : public IWorldEntitySystem
    {
        friend class CameraDebugView;

        //-------------------------------------------------------------------------

        struct RegisteredPlayer : public EntityRegistryRecord
        {
            inline bool IsValid() const { return m_pPlayerComponent != nullptr && m_pCameraComponent != nullptr; }

        public:

            PlayerComponent*                    m_pPlayerComponent = nullptr;
            CameraComponent*                    m_pCameraComponent = nullptr;
        };

    public:

        KRG_REGISTER_TYPE( WorldSystem );
        KRG_ENTITY_WORLD_SYSTEM( WorldSystem, RequiresUpdate( UpdateStage::PrePhysics ) );

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

        virtual void RegisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;
        virtual void UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;
        virtual void UpdateSystem( EntityUpdateContext const& ctx ) override;

        // Player
        //-------------------------------------------------------------------------
        
        RegisteredPlayer const* GetActivePlayer() const;
        RegisteredPlayer const* GetPlayer( UUID const& playerID ) const;

        void SwitchToLastValidPlayer();
        void SwitchToPlayer( UUID const& playerID );
        bool TrySpawnPlayer( EntityUpdateContext const& ctx );

        void SetPlayerActive( RegisteredPlayer& player );

        #if KRG_DEVELOPMENT_TOOLS
        void SpawnDevelopmentPlayer( EntityUpdateContext const& ctx );
        #endif

    private:

        EntityRegistry<RegisteredPlayer>        m_players;
        TVector<PlayerSpawnComponent*>          m_spawnPoints;
        CameraComponent*                        m_pActiveCamera = nullptr;
        UUID                                    m_activePlayerID;
        UUID                                    m_previouslyActivePlayerID;
        bool                                    m_hasSpawnedPlayer = false;
        bool                                    m_registeredPlayerStateChanged = false;
        bool                                    m_disableAllPlayers = false;

        #if KRG_DEVELOPMENT_TOOLS
        bool                                    m_hasSpawnedDevelopmentPlayer = false;
        bool                                    m_useDevelopmentPlayer = false;
        UUID                                    m_developmentPlayerID;
        #endif
    };
} 