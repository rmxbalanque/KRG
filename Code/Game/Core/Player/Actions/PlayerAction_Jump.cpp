#include "PlayerAction_Jump.h"
#include "Game/Core/Player/PhysicsStates/PlayerPhysicsState_Air.h"
#include "Engine/Core/Components/Component_Cameras.h"
#include "Engine/Physics/Components/Component_PhysicsCharacter.h"
#include "System/Input/InputSystem.h"

// hack for now
#include "Game/Core/Player/GraphControllers/PlayerGraphController_Locomotion.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    bool JumpAction::TryStartInternal( ActionContext const& ctx )
    {
        if( ctx.m_pInputSystem->GetControllerState()->WasReleased( Input::ControllerButton::FaceButtonDown ) )
        {
            auto pInAirPhysicsState = ctx.m_pPhysicsController->SetPhysicsState<AirPhysicsState>( Physics::PhysicsStateController::SetStateOption::DoNothingIfAlreadyActive );
            if( m_isChargedJumpReady )
            {
                pInAirPhysicsState->AddImpulse( Vector( 0.f, 0.f, 25.0f ) );
            }
            else
            {
                pInAirPhysicsState->AddImpulse( Vector( 0.f, 0.f, 15.0f ) );
            }
            m_isChargedJumpReady = false;
            m_timer.Start();
            return true;
        }
        else // Check hold state
        {
            m_isChargedJumpReady = false;
            Seconds jumpHoldTime = 0.0f;
            if ( ctx.m_pInputSystem->GetControllerState()->IsHeldDown( Input::ControllerButton::FaceButtonDown, &jumpHoldTime ) )
            {
                if ( jumpHoldTime > 0.5f )
                {
                    m_isChargedJumpReady = true;
                }
            }
        }

        return false;
    }

    Action::Status JumpAction::UpdateInternal( ActionContext const& ctx )
    {
        if ( m_timer.GetElapsedTimeSeconds() >= 0.5f )
        {
            return Status::Completed;
        }
        else
        {
            auto const pControllerState = ctx.m_pInputSystem->GetControllerState();
            KRG_ASSERT( pControllerState != nullptr );

            // Update camera rotation
            //-------------------------------------------------------------------------
            Vector cameraInputs = pControllerState->GetRightAnalogStickValue();

            Radians const maxAngularVelocity = Math::Pi;
            Radians const maxAngularVelocityForThisFrame = maxAngularVelocity * ctx.GetDeltaTime();

            cameraInputs.Normalize2();
            cameraInputs *= (float) maxAngularVelocityForThisFrame;
            ctx.m_pCameraComponent->AdjustOrbitAngle( cameraInputs.m_x, cameraInputs.m_y );

            // Calculate desired player displacement
            //-------------------------------------------------------------------------
            Float2 movementInputs = pControllerState->GetLeftAnalogStickValue();

            float const maxLinearVelocity = 6.0f;
            float const maxLinearVelocityForThisFrame = maxLinearVelocity * ctx.GetDeltaTime();

            auto const camFwd = ctx.m_pCameraComponent->GetCameraRelativeForwardVector().GetNormalized2();
            auto const camRight = ctx.m_pCameraComponent->GetCameraRelativeRightVector().GetNormalized2();

            // Use last frame camera orientation
            auto forward = camFwd * movementInputs.m_y;
            auto right = camRight * movementInputs.m_x;

            Vector const desiredDeltaDisplacement = ( forward * maxLinearVelocityForThisFrame ) + ( right * maxLinearVelocityForThisFrame );
            Quaternion deltaOrientation = Quaternion::Identity;
            if( !desiredDeltaDisplacement.IsZero2() )
            {
                Quaternion const desiredOrientation = Quaternion::FromNormalizedOrientationVector( desiredDeltaDisplacement.GetNormalized2() );
                deltaOrientation = Quaternion::Delta( ctx.m_pCharacterPhysicsComponent->GetOrientation(), desiredOrientation );
            }

            // Run physic Prediction if required
            //-------------------------------------------------------------------------
            // nothing for now

            // Update animation controller
            //-------------------------------------------------------------------------
            auto pLocomotionGraphController = ctx.GetAnimSubGraphController<LocomotionGraphController>();
            float const desiredSpeed = desiredDeltaDisplacement.GetLength3() / ctx.GetDeltaTime();
            pLocomotionGraphController->SetSpeed( 0.f );

            // HACK (this is only because we don't have animation yet)
            //-------------------------------------------------------------------------
            ctx.m_pCharacterPhysicsComponent->m_deltaTranslationHACK = desiredDeltaDisplacement;
            ctx.m_pCharacterPhysicsComponent->m_deltaRotationHACK = deltaOrientation;
            //-------------------------------------------------------------------------
        }

        return Status::Running;
    }

    void JumpAction::StopInternal( ActionContext const& ctx, StopReason reason )
    {

    }
}