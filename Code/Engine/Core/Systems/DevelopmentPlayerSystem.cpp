#include "DevelopmentPlayerSystem.h"
#include "Engine/Core/Components/CameraComponents.h"
#include "Engine/Core/Components/PlayerComponent.h"
#include "Engine/Core/Entity/EntityUpdateContext.h"
#include "System/Input/InputSystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace
    {
        static float const g_lookSpeed = Math::Pi;
        static float const g_moveSpeed = 10.0f; // 10 m/s
        static FloatRange const g_moveSpeedLimits( 0.5f, 100 );
    }

    //-------------------------------------------------------------------------

    DevelopmentPlayerSystem::DevelopmentPlayerSystem()
        : m_currentMoveSpeed( g_moveSpeed )
    {}

    DevelopmentPlayerSystem::~DevelopmentPlayerSystem()
    {
        KRG_ASSERT( m_pRootComponent == nullptr && m_pCameraComponent == nullptr );
    }

    void DevelopmentPlayerSystem::RegisterComponent( EntityComponent* pComponent )
    {
        if ( auto pSpatialComponent = TryCast<SpatialEntityComponent>( pComponent ) )
        {
            if ( pSpatialComponent->IsRootComponent() )
            {
                KRG_ASSERT( m_pRootComponent == nullptr );
                m_pRootComponent = pSpatialComponent;
            }
        }

        if ( auto pCameraComponent = TryCast<FreeLookCameraComponent>( pComponent ) )
        {
            m_pCameraComponent = pCameraComponent;
        }

        if ( auto pPlayerComponent = TryCast<PlayerComponent>( pComponent ) )
        {
            m_pPlayerComponent = pPlayerComponent;
        }
    }

    void DevelopmentPlayerSystem::UnregisterComponent( EntityComponent* pComponent )
    {
        if ( auto pSpatialComponent = TryCast<SpatialEntityComponent>( pComponent ) )
        {
            if ( pSpatialComponent->IsRootComponent() && m_pRootComponent == pSpatialComponent )
            {
                m_pRootComponent = nullptr;
            }
        }

        if ( auto pCameraComponent = TryCast<FreeLookCameraComponent>( pComponent ) )
        {
            if ( m_pCameraComponent == pCameraComponent )
            {
                m_pCameraComponent = nullptr;
            }
        }

        if ( auto pPlayerComponent = TryCast<PlayerComponent>( pComponent ) )
        {
            if ( m_pPlayerComponent == pPlayerComponent )
            {
                pPlayerComponent = nullptr;
            }
        }
    }

    void DevelopmentPlayerSystem::Update( EntityUpdateContext const& ctx )
    {
        if ( m_pCameraComponent == nullptr || m_pRootComponent == nullptr || m_pPlayerComponent == nullptr )
        {
            return;
        }

        if ( !m_pPlayerComponent->IsPlayerEnabled() )
        {
            return;
        }

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
            m_currentMoveSpeed = g_moveSpeed;
        }

        int32 const wheelDelta = pMouseState->GetWheelDelta();
        m_currentMoveSpeed = g_moveSpeedLimits.GetClampedValue( m_currentMoveSpeed + ( wheelDelta * 0.5f ) );

        // Position update
        //-------------------------------------------------------------------------

        Vector positionDelta = Vector::Zero;

        bool const fwdButton = pKeyboardState->IsHeldDown( Input::KeyboardButton::Key_W );
        bool const backButton = pKeyboardState->IsHeldDown( Input::KeyboardButton::Key_S );
        bool const leftButton = pKeyboardState->IsHeldDown( Input::KeyboardButton::Key_A );
        bool const rightButton = pKeyboardState->IsHeldDown( Input::KeyboardButton::Key_D );
        bool const needsPositionUpdate = fwdButton || backButton || leftButton || rightButton;

        if( needsPositionUpdate )
        {
            float LR = 0;
            if ( leftButton ) { LR -= 1.0f; }
            if ( rightButton ) { LR += 1.0f; }

            float FB = 0;
            if ( fwdButton ) { FB += 1.0f; }
            if ( backButton ) { FB -= 1.0f; }

            Vector const spatialInput( LR, FB, 0, 0 );

            Transform const& entityTransform = m_pRootComponent->GetWorldTransform();
            Vector const forwardDirection = entityTransform.GetForwardVector();
            Vector const rightDirection = entityTransform.GetRightVector();

            Vector const moveDelta = Vector( m_currentMoveSpeed * deltaTime );
            Vector const deltaForward = spatialInput.GetSplatY() * moveDelta;
            Vector const deltaRight = spatialInput.GetSplatX() * moveDelta;

            // Calculate position delta
            positionDelta = deltaRight * rightDirection;
            positionDelta += deltaForward * forwardDirection;
            positionDelta.m_w = 0.0f;

            // Update character transform
            Transform newEntityTransform = entityTransform;
            newEntityTransform.SetTranslation( entityTransform.GetTranslation() + positionDelta );
            m_pRootComponent->SetLocalTransform( newEntityTransform );
        }

        // Direction update
        //-------------------------------------------------------------------------

        Radians headingDelta = 0.0f, pitchDelta = 0.0f;

        if ( pMouseState->IsHeldDown( Input::MouseButton::Right ) )
        {
            auto const directionDelta = pMouseState->GetMovementDelta();
            if ( directionDelta.m_x != 0 || directionDelta.m_y != 0 )
            {
                // Adjust heading and pitch based on input
                headingDelta = -directionDelta.m_x * g_lookSpeed * deltaTime;
                pitchDelta = -directionDelta.m_y * g_lookSpeed * deltaTime;
                m_pCameraComponent->AdjustHeadingAndPitch( headingDelta, pitchDelta );
            }
        }
    }
}