#include "PlayerAction_Locomotion.h"
#include "Game/Core/Player/Components/Component_MainPlayer.h"
#include "Game/Core/Player/GraphControllers/PlayerGraphController_Locomotion.h"
#include "Game/Core/Player/PlayerAnimationController.h"
#include "Game/Core/Player/PlayerPhysicsController.h"
#include "Game/Core/Player/PlayerCameraController.h"
#include "Engine/Physics/Components/Component_PhysicsCharacter.h"
#include "System/Input/InputSystem.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    static float    g_maxSprintSpeed = 7.5f;                      // meters/second
    static float    g_maxRunSpeed = 5.0f;                         // meters/second
    static float    g_maxCrouchSpeed = 3.0f;                      // meters/second
    static Seconds  g_timeToTriggerSprint = 1.5f;                 // seconds
    static Seconds  g_timeToTriggerCrouch = 0.5f;                 // seconds
    static float    g_sprintStickAmplitude = 0.8f;                // [0,1]

    //-------------------------------------------------------------------------

    bool LocomotionAction::TryStartInternal( ActionContext const& ctx )
    {
        ctx.m_pAnimationController->SetCharacterState( CharacterAnimationState::Locomotion );
        ctx.m_pCharacterController->EnableGravity( ctx.m_pCharacterComponent->GetCharacterVelocity().m_z );
        ctx.m_pCharacterController->EnableProjectionOntoFloor();
        ctx.m_pCharacterController->EnableStepHeight();

        if ( ctx.m_pCharacterComponent->GetCharacterVelocity().GetLength2() > g_maxRunSpeed )
        {
            ctx.m_pPlayerComponent->m_sprintFlag = true;
        }

        return true;
    }

    Action::Status LocomotionAction::UpdateInternal( ActionContext const& ctx )
    {
        auto const pControllerState = ctx.m_pInputState->GetControllerState();
        KRG_ASSERT( pControllerState != nullptr );

        // Calculate desired player displacement
        //-------------------------------------------------------------------------
        Vector const movementInputs = pControllerState->GetLeftAnalogStickValue();
        float const stickAmplitude = movementInputs.GetLength2();

        auto const& camFwd = ctx.m_pCameraController->GetCameraRelativeForwardVector2D();
        auto const& camRight = ctx.m_pCameraController->GetCameraRelativeRightVector2D();

        // Check for crouch
        if( ctx.m_pInputState->GetControllerState()->IsHeldDown( Input::ControllerButton::ThumbstickLeft ) )
        {
            if( !m_crouchTimer.IsRunning() )
            {
                m_crouchTimer.Start( g_timeToTriggerCrouch );
            }
            if( !m_crouchToggled && m_crouchTimer.Update( ctx.GetDeltaTime() ) )
            {
                ctx.m_pPlayerComponent->m_CrouchFlag = !ctx.m_pPlayerComponent->m_CrouchFlag;
                m_crouchToggled = true;
            }
        }
        else
        {
            m_crouchToggled = false;
            m_crouchTimer.Stop();
        }

        if( !ctx.m_pPlayerComponent->m_CrouchFlag )
        {
            // Check for sprint
            float const currentLinearSpeed = ctx.m_pCharacterComponent->GetCharacterVelocity().GetLength3();
            if( stickAmplitude > g_sprintStickAmplitude ) // TODO : Run prediction for collision
            {
                if ( !ctx.m_pPlayerComponent->m_sprintFlag )
                {
                    if( !m_sprintTimer.IsRunning() )
                    {
                        m_sprintTimer.Start( g_timeToTriggerSprint );
                    }

                    if ( m_sprintTimer.Update( ctx.GetDeltaTime() ) )
                    {
                        ctx.m_pPlayerComponent->m_sprintFlag = true;
                    }
                }
            }
            else
            {
                m_sprintTimer.Stop();
                ctx.m_pPlayerComponent->m_sprintFlag = false;
            }
        }

        float const Speed = ctx.m_pPlayerComponent->m_sprintFlag ? g_maxSprintSpeed : ctx.m_pPlayerComponent->m_CrouchFlag ? g_maxCrouchSpeed : g_maxRunSpeed;

        // Use last frame camera orientation
        Vector const forward = camFwd * movementInputs.m_y;
        Vector const right = camRight * movementInputs.m_x;
        Vector const desiredHeadingVelocity2D = ( forward + right ) * Speed;
        Vector const ResultingVelocity = desiredHeadingVelocity2D;
        Vector Facing = desiredHeadingVelocity2D.IsZero2() ? ctx.m_pCharacterComponent->GetForwardVector() : desiredHeadingVelocity2D.GetNormalized2();

        // Run physic Prediction if required
        //-------------------------------------------------------------------------
        // nothing for now
        
        // Update animation controller
        //-------------------------------------------------------------------------
        auto pLocomotionGraphController = ctx.GetAnimSubGraphController<LocomotionGraphController>();

        if( ctx.m_pCharacterController->GetFloorType() != CharacterPhysicsController::FloorType::Navigable &&
            ctx.m_pCharacterComponent->GetCharacterVelocity().m_z < -Math::Epsilon )
        {
            m_slideTimer.Update( ctx.GetDeltaTime() );
            if( m_slideTimer.GetElapsedTimeSeconds() > 0.35f )
            {
                pLocomotionGraphController->SetSliding( true );
                Facing = ctx.m_pCharacterComponent->GetCharacterVelocity().GetNormalized2();
            }
        }
        else
        {
            m_slideTimer.Reset();
            pLocomotionGraphController->SetSliding( false );
        }

        pLocomotionGraphController->SetLocomotionDesires( ctx.GetDeltaTime(), ResultingVelocity, Facing );
        pLocomotionGraphController->SetCrouch( ctx.m_pPlayerComponent->m_CrouchFlag );

        return Status::Interruptible;
    }

    void LocomotionAction::StopInternal( ActionContext const& ctx, StopReason reason )
    {
        ctx.m_pPlayerComponent->m_sprintFlag = false;
        ctx.m_pPlayerComponent->m_CrouchFlag = false;
        m_sprintTimer.Stop();
    }
}