#include "PlayerSystem.h"
#include "Engine/Physics/PhysicsWorldSystem.h"
#include "Engine/Physics/Components/PhysicsCapsuleComponent.h"
#include "Engine/Core/Components/CameraComponents.h"
#include "Engine/Core/Entity/EntityUpdateContext.h"
#include "Engine/Core/Components/PlayerComponent.h"
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

    PlayerSystem::PlayerSystem()
        : m_currentMoveSpeed( g_moveSpeed )
    {}

    PlayerSystem::~PlayerSystem()
    {
        KRG_ASSERT( m_pRootComponent == nullptr && m_pCameraComponent == nullptr );
    }

    void PlayerSystem::RegisterComponent( EntityComponent* pComponent )
    {
        if ( auto pSpatialComponent = TryCast<SpatialEntityComponent>( pComponent ) )
        {
            if ( pSpatialComponent->IsRootComponent() )
            {
                KRG_ASSERT( m_pRootComponent == nullptr );
                m_pRootComponent = pSpatialComponent;
            }
        }

        if ( auto pCapsuleComponent = TryCast<Physics::CapsuleComponent>( pComponent ) )
        {
            // TODO: handle multiple comps per player
            KRG_ASSERT( m_pCapsuleComponent == nullptr );
            m_pCapsuleComponent = pCapsuleComponent;
        }

        if ( auto pCameraComponent = TryCast<OrbitCameraComponent>( pComponent ) )
        {
            m_pCameraComponent = pCameraComponent;
        }

        if ( auto pPlayerComponent = TryCast<PlayerComponent>( pComponent ) )
        {
            m_pPlayerComponent = pPlayerComponent;
        }
    }

    void PlayerSystem::UnregisterComponent( EntityComponent* pComponent )
    {
        if ( auto pSpatialComponent = TryCast<SpatialEntityComponent>( pComponent ) )
        {
            if ( pSpatialComponent->IsRootComponent() && m_pRootComponent == pSpatialComponent )
            {
                m_pRootComponent = nullptr;
            }
        }

        if ( auto pCapsuleComponent = TryCast<Physics::CapsuleComponent>( pComponent ) )
        {
            if ( m_pCapsuleComponent == pCapsuleComponent )
            {
                m_pCapsuleComponent = nullptr;
            }
        }

        if ( auto pCameraComponent = TryCast<OrbitCameraComponent>( pComponent ) )
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

    void PlayerSystem::Update( EntityUpdateContext const& ctx )
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

        if ( pInputSystem->GetNumConnectedControllers() == 0 )
        {
            m_pCameraComponent->FinalizeCameraPosition();
            return;
        }

        //-------------------------------------------------------------------------

        auto const pControllerState = pInputSystem->GetControllerState();
        KRG_ASSERT( pControllerState != nullptr );

        // Update camera rotation
        //-------------------------------------------------------------------------

        Vector cameraInputs = pControllerState->GetRightAnalogStickValue();

        Radians const maxAngularVelocity = Math::Pi;
        Radians maxAngularVelocityForThisFrame = maxAngularVelocity * ctx.GetDeltaTime();

        cameraInputs.Normalize2();
        cameraInputs *= maxAngularVelocityForThisFrame;
        m_pCameraComponent->AdjustOrbitAngle( cameraInputs.m_x, cameraInputs.m_y );

        // Move Player
        //-------------------------------------------------------------------------

        Float2 movementInputs = pControllerState->GetLeftAnalogStickValue();

        float const maxLinearVelocity = 12.0f;
        float maxLinearVelocityForThisFrame = maxLinearVelocity * ctx.GetDeltaTime();

        auto const camFwd = m_pCameraComponent->GetCameraRelativeForwardVector().GetNormalized2();
        auto const camRight = m_pCameraComponent->GetCameraRelativeRightVector().GetNormalized2();

        // Use last frame camera orientation
        auto forward = camFwd * movementInputs.m_y;
        auto right = camRight * movementInputs.m_x;
        auto deltaMovement = ( forward * maxLinearVelocityForThisFrame ) + ( right * maxLinearVelocityForThisFrame );

        Vector direction;
        Vector speedVector;
        deltaMovement.ToDirectionAndLength2( direction, speedVector );
        float speed = speedVector.ToFloat();
        if ( speed > maxLinearVelocityForThisFrame )
        {
            deltaMovement = direction * maxLinearVelocityForThisFrame;
            speed = maxLinearVelocityForThisFrame;
        }

        if ( speed > 0 )
        {
            auto pPhysWorld = ctx.GetWorldSystem<Physics::PhysicsWorldSystem>();

            Physics::QueryFilter filter;
            filter.SetLayerMask( Physics::CreateLayerMask( Physics::Layers::Environment, Physics::Layers::Characters ) );
            filter.AddIgnoredEntity( m_pCapsuleComponent->GetEntityID() );

            Physics::SweepResults results;
            pPhysWorld->AcquireReadLock();

            if ( pPhysWorld->CapsuleSweep( m_pCapsuleComponent->GetCylinderPortionHalfHeight(), m_pCapsuleComponent->GetRadius(), m_pCapsuleComponent->GetOrientation(), m_pCapsuleComponent->GetPosition(), m_pCapsuleComponent->GetPosition() + deltaMovement, filter, results ) )
            {
                if ( results.hasBlock )
                {
                    if ( results.block.hadInitialOverlap() )
                    {
                        KRG_LOG_MESSAGE( "Physics", "overlap: %s", results.block.actor->getName() );
                    }

                    KRG_LOG_MESSAGE( "Physics", "block: %s", results.block.actor->getName() );
                }
                else
                {
                    int32 numTouches = results.getNbTouches();
                    for ( int32 i =0; i < numTouches; i++ )
                    {
                        if ( results.getTouch( i ).hadInitialOverlap() )
                        {
                            KRG_LOG_MESSAGE( "Physics", "initial overlap: %s", results.getTouch( i ).actor->getName() );
                        }
                        KRG_LOG_MESSAGE( "Physics", "touch: %s", results.getTouch(i).actor->getName() );
                    }
                }
            }

            pPhysWorld->ReleaseReadLock();

            Transform currentRootTransform = m_pRootComponent->GetWorldTransform();
            currentRootTransform.SetRotation( Quaternion::FromOrientationVector( deltaMovement.GetNormalized3() ) );
            currentRootTransform.AddTranslationOffset( deltaMovement );
            m_pRootComponent->SetWorldTransform( currentRootTransform );
        }

        //-------------------------------------------------------------------------

        m_pCameraComponent->FinalizeCameraPosition();
    }
}