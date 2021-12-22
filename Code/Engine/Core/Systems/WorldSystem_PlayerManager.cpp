#include "WorldSystem_PlayerManager.h"
#include "Engine/Core/Components/Component_PlayerSpawn.h"
#include "Engine/Core/Components/Component_Cameras.h"
#include "Engine/Core/Components/Component_Player.h"
#include "Engine/Core/Entity/Entity.h"
#include "Engine/Core/Entity/EntityUpdateContext.h"
#include "Engine/Core/Entity/EntityMap.h"
#include "Engine/Core/Entity/EntityCollection.h"
#include "System/Input/InputSystem.h"
#include "System/TypeSystem/TypeRegistry.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace
    {
        constexpr static float const g_lookSpeed = Math::Pi / 8;
        constexpr static float const g_moveSpeed = 10.0f; // 10 m/s
        static FloatRange const g_moveSpeedLimits( 0.5f, 100 );
    }

    //-------------------------------------------------------------------------

    void PlayerManager::ShutdownSystem()
    {
        KRG_ASSERT( !m_player.m_entityID.IsValid() );
        KRG_ASSERT( m_spawnPoints.empty() );
        m_pDebugCameraComponent = nullptr;
    }

    void PlayerManager::RegisterComponent( Entity const* pEntity, EntityComponent* pComponent )
    {
        if ( auto pSpawnComponent = TryCast<Player::PlayerSpawnComponent>( pComponent ) )
        {
            m_spawnPoints.emplace_back( pSpawnComponent );
        }
        else if ( auto pPlayerComponent = TryCast<Player::PlayerComponent>( pComponent ) )
        {
            if ( m_player.m_entityID.IsValid() )
            {
                KRG_LOG_ERROR( "Player", "Multiple players spawned! this is not supported" );
            }
            else
            {
                m_player.m_entityID = pEntity->GetID();
                KRG_ASSERT( m_player.m_pPlayerComponent == nullptr && m_player.m_pCameraComponent == nullptr );
                m_player.m_pPlayerComponent = pPlayerComponent;

                // Check if the camera for this player has already been registered
                for ( auto pCamera : m_cameras )
                {
                    if ( pCamera->GetEntityID() == m_player.m_entityID )
                    {
                        m_player.m_pCameraComponent = pCamera;
                    }
                }

                m_registeredPlayerStateChanged = true;
            }
        }
        else if ( auto pCameraComponent = TryCast<CameraComponent>( pComponent ) )
        {
            m_cameras.emplace_back( pCameraComponent );

            // If this is the camera for the player set it in the player struct
            if ( m_player.m_entityID == pEntity->GetID() )
            {
                KRG_ASSERT( m_player.m_pCameraComponent == nullptr );
                m_player.m_pCameraComponent = pCameraComponent;
                m_registeredPlayerStateChanged = true;
            }
        }
    }

    void PlayerManager::UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent )
    {
        if ( auto pSpawnComponent = TryCast<Player::PlayerSpawnComponent>( pComponent ) )
        {
            m_spawnPoints.erase_first_unsorted( pSpawnComponent );
        }
        else if ( auto pPlayerComponent = TryCast<Player::PlayerComponent>( pComponent ) )
        {
            // Remove the player
            if ( m_player.m_entityID == pEntity->GetID() )
            {
                KRG_ASSERT( m_player.m_pPlayerComponent == pPlayerComponent );
                m_player.m_entityID.Clear();
                m_player.m_pPlayerComponent = nullptr;
                m_player.m_pCameraComponent = nullptr;
                m_registeredPlayerStateChanged = true;
            }
        }
        else if ( auto pCameraComponent = TryCast<CameraComponent>( pComponent ) )
        {
            if ( m_pActiveCamera == pCameraComponent )
            {
                m_pActiveCamera = nullptr;
            }

            m_cameras.erase_first( pCameraComponent );

            // Remove camera from the player state
            if ( m_player.IsValid() && m_player.m_entityID == pEntity->GetID() )
            {
                KRG_ASSERT( m_player.m_pCameraComponent == pCameraComponent );
                m_player.m_pCameraComponent = nullptr;
                m_registeredPlayerStateChanged = true;
            }
        }
    }

    //-------------------------------------------------------------------------

    bool PlayerManager::TrySpawnPlayer( EntityUpdateContext const& ctx )
    {
        if ( m_spawnPoints.empty() )
        {
            return false;
        }

        auto pTypeRegistry = ctx.GetSystem<TypeSystem::TypeRegistry>();
        auto pPersistentMap = ctx.GetPersistentMap();

        //-------------------------------------------------------------------------

        // For now we only support a single spawn point
        EntityModel::EntityCollection ec( *pTypeRegistry, UUID::GenerateID(), *m_spawnPoints[0]->GetEntityCollectionDesc() );
        pPersistentMap->AddEntityCollection( m_spawnPoints[0]->GetWorldTransform(), ec );
        return true;
    }

    void PlayerManager::SetPlayerControllerState( bool isEnabled )
    {
        m_isControllerEnabled = isEnabled;

        if ( m_player.m_pPlayerComponent != nullptr )
        {
            m_player.m_pPlayerComponent->SetPlayerEnabled( m_isControllerEnabled );
        }
    }

    //-------------------------------------------------------------------------

    #if KRG_DEVELOPMENT_TOOLS
    void PlayerManager::SpawnDebugCamera( EntityUpdateContext const& ctx )
    {
        KRG_ASSERT( m_pDebugCameraComponent == nullptr );
        m_pDebugCameraComponent = KRG::New<FreeLookCameraComponent>( StringID( "Debug Camera Component" ) );
        m_pDebugCameraComponent->SetPositionAndLookatTarget( Vector( 0, -5, 5 ), Vector( 0, 0, 0 ) );
        m_debugCameraMoveSpeed = g_moveSpeed;

        auto pEntity = KRG::New<Entity>( StringID( "Debug Camera" ) );
        pEntity->AddComponent( m_pDebugCameraComponent );
        ctx.GetPersistentMap()->AddEntity( pEntity );
    }

    void PlayerManager::UpdateDebugCamera( EntityUpdateContext const& ctx )
    {
        KRG_ASSERT( m_pDebugCameraComponent != nullptr );
        KRG_ASSERT( m_isControllerEnabled );

        //-------------------------------------------------------------------------

        Seconds const deltaTime = ctx.GetDeltaTime();
        auto pInputSystem = ctx.GetSystem<Input::InputSystem>();
        KRG_ASSERT( pInputSystem != nullptr );

        auto const pMouseState = pInputSystem->GetMouseState();
        auto const pKeyboardState = pInputSystem->GetKeyboardState();

        // Speed Update
        //-------------------------------------------------------------------------

        if ( pMouseState->WasReleased( Input::MouseButton::Middle ) )
        {
            m_debugCameraMoveSpeed = g_moveSpeed;
        }

        int32 const wheelDelta = pMouseState->GetWheelDelta();
        m_debugCameraMoveSpeed = g_moveSpeedLimits.GetClampedValue( m_debugCameraMoveSpeed + ( wheelDelta * 0.5f ) );

        // Position update
        //-------------------------------------------------------------------------

        Vector positionDelta = Vector::Zero;

        bool const fwdButton = pKeyboardState->IsHeldDown( Input::KeyboardButton::Key_W );
        bool const backButton = pKeyboardState->IsHeldDown( Input::KeyboardButton::Key_S );
        bool const leftButton = pKeyboardState->IsHeldDown( Input::KeyboardButton::Key_A );
        bool const rightButton = pKeyboardState->IsHeldDown( Input::KeyboardButton::Key_D );
        bool const needsPositionUpdate = fwdButton || backButton || leftButton || rightButton;

        if ( needsPositionUpdate )
        {
            float LR = 0;
            if ( leftButton ) { LR -= 1.0f; }
            if ( rightButton ) { LR += 1.0f; }

            float FB = 0;
            if ( fwdButton ) { FB += 1.0f; }
            if ( backButton ) { FB -= 1.0f; }

            Vector const spatialInput( LR, FB, 0, 0 );

            Transform cameraTransform = m_pDebugCameraComponent->GetWorldTransform();
            Vector const forwardDirection = cameraTransform.GetForwardVector();
            Vector const rightDirection = cameraTransform.GetRightVector();

            Vector const moveDelta = Vector( m_debugCameraMoveSpeed * deltaTime );
            Vector const deltaForward = spatialInput.GetSplatY() * moveDelta;
            Vector const deltaRight = spatialInput.GetSplatX() * moveDelta;

            // Calculate position delta
            positionDelta = deltaRight * rightDirection;
            positionDelta += deltaForward * forwardDirection;
            positionDelta.m_w = 0.0f;

            // Update camera transform
            cameraTransform.AddTranslation( positionDelta );
            m_pDebugCameraComponent->SetWorldTransform( cameraTransform );
        }

        // Direction update
        //-------------------------------------------------------------------------

        Radians headingDelta = 0.0f, pitchDelta = 0.0f;

        if ( pMouseState->IsHeldDown( Input::MouseButton::Right ) )
        {
            Vector const mouseDelta = pMouseState->GetMovementDelta();
            Vector const angleDelta = mouseDelta * Vector( g_lookSpeed * deltaTime );
            if ( !angleDelta.IsNearZero2() )
            {
                headingDelta = -angleDelta.GetX();
                pitchDelta = -angleDelta.GetY();
                m_pDebugCameraComponent->AdjustPitchAndYaw( headingDelta, pitchDelta );
            }
        }
    }

    void PlayerManager::SetDebugMode( DebugMode newMode )
    {
        switch ( newMode )
        {
            case DebugMode::None:
            {
                if ( m_player.IsValid() )
                {
                    m_pActiveCamera = m_player.m_pCameraComponent;
                    m_player.m_pPlayerComponent->SetPlayerEnabled( true );
                }
            }
            break;

            case DebugMode::UseDebugCamera:
            {
                m_pActiveCamera = m_pDebugCameraComponent;

                if ( m_player.IsValid() )
                {
                    // Transform camera position/orientation to the debug camera
                    if ( m_debugMode == DebugMode::None )
                    {
                        m_pDebugCameraComponent->SetWorldTransform( m_player.m_pCameraComponent->GetWorldTransform() );
                    }

                    m_player.m_pPlayerComponent->SetPlayerEnabled( true );
                }
            }
            break;

            case DebugMode::FullDebug:
            {
                m_pActiveCamera = m_pDebugCameraComponent;

                if ( m_player.IsValid() )
                {
                    // Transform camera position/orientation to the debug camera
                    if ( m_debugMode == DebugMode::None )
                    {
                        m_pDebugCameraComponent->SetWorldTransform( m_player.m_pCameraComponent->GetWorldTransform() );
                    }

                    m_player.m_pPlayerComponent->SetPlayerEnabled( false );
                }
            }
            break;
        }

        m_debugMode = newMode;
    }
    #endif

    //-------------------------------------------------------------------------

    void PlayerManager::UpdateSystem( EntityUpdateContext const& ctx )
    {
        if ( ctx.GetUpdateStage() == UpdateStage::FrameStart )
        {
            #if KRG_DEVELOPMENT_TOOLS
            if ( m_pDebugCameraComponent == nullptr )
            {
                SpawnDebugCamera( ctx );
            }

            // Always ensure we have an active camera set
            if ( m_pActiveCamera == nullptr && m_pDebugCameraComponent->IsInitialized() )
            {
                m_pActiveCamera = m_pDebugCameraComponent;
            }

            // If we are in full debug mode, update the camera position
            if ( m_debugMode == DebugMode::FullDebug && m_isControllerEnabled )
            {
                UpdateDebugCamera( ctx );
            }
            #endif

            //-------------------------------------------------------------------------

            // Spawn players in game worlds
            if ( ctx.IsGameWorld() && !m_hasSpawnedPlayer )
            {
                m_hasSpawnedPlayer = TrySpawnPlayer( ctx );
            }

            // Handle players state changes
            if ( m_registeredPlayerStateChanged )
            {
                if ( m_player.IsValid() )
                {
                    m_pActiveCamera = m_player.m_pCameraComponent;
                    m_player.m_pPlayerComponent->SetPlayerEnabled( m_isControllerEnabled );
                }

                m_registeredPlayerStateChanged = false;
            }
        }
        else if ( ctx.GetUpdateStage() == UpdateStage::Paused )
        {
            // If we are in full debug mode, update the camera position
            if ( m_debugMode == DebugMode::FullDebug && m_isControllerEnabled )
            {
                UpdateDebugCamera( ctx );
            }
        }
        else
        {
            KRG_UNREACHABLE_CODE();
        }
    }
}