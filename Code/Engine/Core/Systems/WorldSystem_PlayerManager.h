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
    class FreeLookCameraComponent;

    //-------------------------------------------------------------------------

    namespace Player
    {
        class PlayerComponent;
        class PlayerSpawnComponent;
    }

    //-------------------------------------------------------------------------

    class KRG_ENGINE_CORE_API PlayerManager : public IWorldEntitySystem
    {
        struct RegisteredPlayer
        {
            inline bool IsValid() const { return m_pPlayerComponent != nullptr && m_pCameraComponent != nullptr; }

        public:

            EntityID                            m_entityID;
            Player::PlayerComponent*            m_pPlayerComponent = nullptr;
            CameraComponent*                    m_pCameraComponent = nullptr;
        };

    public:

        #if KRG_DEVELOPMENT_TOOLS
        enum class DebugMode
        {
            None,               // Default mode - game plays as normal
            UseDebugCamera,     // Game plays as normal, but we use the debug camera to view it 
            FullDebug,          // Player controller is disabled and we have full control of the debug camera
        };

        //-------------------------------------------------------------------------

        constexpr static float const            s_debugCameraDefaultSpeed = 15.0f; // m/s
        constexpr static float const            s_debugCameraMinSpeed = 0.5f; // m/s
        constexpr static float const            s_debugCameraMaxSpeed = 100.0f; // m/s
        #endif

    public:

        KRG_REGISTER_TYPE( PlayerManager );
        KRG_ENTITY_WORLD_SYSTEM( PlayerManager, RequiresUpdate( UpdateStage::FrameStart, UpdatePriority::Highest ), RequiresUpdate( UpdateStage::Paused ) );

        // Camera
        //-------------------------------------------------------------------------

        inline bool HasActiveCamera() const { return m_pActiveCamera != nullptr; }
        inline CameraComponent* GetActiveCamera() const { return m_pActiveCamera; }

        // Player
        //-------------------------------------------------------------------------

        inline bool HasPlayer() const { return m_player.IsValid(); }
        inline EntityID GetPlayerEntityID() const { return m_player.m_entityID; }
        inline CameraComponent* GetPlayerCamera() const { return m_player.m_pCameraComponent; }
        void SetPlayerControllerState( bool isEnabled );

        #if KRG_DEVELOPMENT_TOOLS
        void SetDebugMode( DebugMode mode );
        inline DebugMode GetDebugMode() const { return m_debugMode; }
        inline FreeLookCameraComponent* GetDebugCamera() const { return m_pDebugCameraComponent; }
        inline void ResetDebugCameraSpeed() { m_debugCameraMoveSpeed = s_debugCameraDefaultSpeed; }
        inline void SetDebugCameraSpeed( float speed ) { m_debugCameraMoveSpeed = FloatRange( s_debugCameraMinSpeed, s_debugCameraMaxSpeed ).GetClampedValue( speed ); }
        inline void SetDebugCameraView( Transform const& cameraTransform );
        inline float GetDebugCameraMoveSpeed() const { return m_debugCameraMoveSpeed; }
        #endif

    private:

        virtual void ShutdownSystem() override final;
        virtual void RegisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;
        virtual void UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;
        virtual void UpdateSystem( EntityWorldUpdateContext const& ctx ) override;

        bool TrySpawnPlayer( EntityWorldUpdateContext const& ctx );

        #if KRG_DEVELOPMENT_TOOLS
        void SpawnDebugCamera( EntityWorldUpdateContext const& ctx );
        void UpdateDebugCamera( EntityWorldUpdateContext const& ctx );
        #endif

    private:

        RegisteredPlayer                            m_player;
        TVector<Player::PlayerSpawnComponent*>      m_spawnPoints;
        TVector<CameraComponent*>                   m_cameras;
        CameraComponent*                            m_pActiveCamera = nullptr;
        bool                                        m_hasSpawnedPlayer = false;
        bool                                        m_registeredPlayerStateChanged = false;
        bool                                        m_isControllerEnabled = true;


        #if KRG_DEVELOPMENT_TOOLS
        FreeLookCameraComponent*                    m_pDebugCameraComponent = nullptr;
        float                                       m_debugCameraMoveSpeed = s_debugCameraDefaultSpeed;
        Vector                                      m_directionChangeAccumulator = Vector::Zero;
        DebugMode                                   m_debugMode = DebugMode::None;
        #endif
    };
} 