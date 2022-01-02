#include "PlayerAction_Jump.h"
#include "Game/Core/Player/PlayerPhysicsController.h"
#include "Game/Core/Player/PlayerCameraController.h"
#include "Game/Core/Player/PlayerAnimationController.h"
#include "Game/Core/Player/GraphControllers/PlayerGraphController_Ability.h"
#include "Engine/Physics/Components/Component_PhysicsCharacter.h"
#include "System/Input/InputSystem.h"

// hack for now
#include "Game/Core/Player/GraphControllers/PlayerGraphController_Locomotion.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    static Radians  g_maxAngularSpeed = Radians( Degrees( 90 ) ); // radians/second
    static float    g_smallJumpDistance = 2.0f;                   // meters/second
    static float    g_bigJumpDistance = 6.0f;                     // meters/second
    static float    g_gravityAcceleration = 30.0f;                // meters/second squared
    static float    g_maxAirControlAcceleration = 10.0f;          // meters/second squared
    static float    g_maxAirControlSpeed = 6.5f;                  // meters/second
    static Seconds  g_bigJumpHoldTime = 0.3f;                     // seconds

    //-------------------------------------------------------------------------

    bool JumpAction::TryStartInternal( ActionContext const& ctx )
    {
        if( ctx.m_pInputState->GetControllerState()->WasReleased( Input::ControllerButton::FaceButtonDown ) )
        {
            ctx.m_pAnimationController->SetCharacterState( CharacterAnimationState::Ability );
            auto pAbilityAnimController = ctx.GetAnimSubGraphController<AbilityGraphController>();
            pAbilityAnimController->StartJump();

            ctx.m_pCharacterController->DisableGravity();
            ctx.m_pCharacterController->DisableProjectionOntoFloor();
            m_timer.Start();
            return true;
        }
        else // Check hold state
        {
            m_isChargedJumpReady = false;
            Seconds jumpHoldTime = 0.0f;
            if( ctx.m_pInputState->GetControllerState()->IsHeldDown( Input::ControllerButton::FaceButtonDown, &jumpHoldTime ) )
            {
                if( jumpHoldTime > g_bigJumpHoldTime )
                {
                    m_isChargedJumpReady = true;
                }
            }
        }

        return false;
    }

    Action::Status JumpAction::UpdateInternal( ActionContext const& ctx )
    {
        // Hack for the launch duration this should be reading the animation for the end of the launch
        if ( m_timer.GetElapsedTimeSeconds() >= 0.1f ) 
        {
            return Status::Completed;
        }
        else
        {
            auto const pControllerState = ctx.m_pInputState->GetControllerState();
            KRG_ASSERT( pControllerState != nullptr );

            // Calculate desired player displacement
            //-------------------------------------------------------------------------
            Vector const movementInputs = pControllerState->GetLeftAnalogStickValue();
            auto const& camFwd = ctx.m_pCameraController->GetCameraRelativeForwardVector2D();
            auto const& camRight = ctx.m_pCameraController->GetCameraRelativeRightVector2D();

            // Use last frame camera orientation
            Vector const CurrentVelocity = ctx.m_pCharacterComponent->GetCharacterVelocity();
            Vector const CurrentVelocity2D = CurrentVelocity * Vector( 1.0f, 1.0f, 0.0f );
            float const CurrentSpeed2D = CurrentVelocity2D.GetLength2();

            Vector const forward = camFwd * movementInputs.m_y;
            Vector const right = camRight * movementInputs.m_x;
            Vector const desiredHeadingVelocity2D = ( forward + right ) * g_maxAirControlAcceleration * ctx.GetDeltaTime();

            Vector ResultingVelocity = CurrentVelocity2D + desiredHeadingVelocity2D;
            float const Length = ResultingVelocity.GetLength2();
            if( Length > g_maxAirControlSpeed )
            {
                ResultingVelocity = ResultingVelocity.GetNormalized2() * g_maxAirControlSpeed;
            }

            //  1.) V = Vi + a(t)
            //
            //      0 = Vi + a(t)                   V = o since we want to reach the apex, hence velocity 0.
            //      Vi = -a(t)
            //
            //  2.)	d = Vi(t) + 0.5(a)(t^2)
            //
            //      d = -a(t)(t) + 0.5(a)(t^2)      substitute vi = -a(t) from 1.
            //      d = -a(t^2)  + 0.5(a)(t^2)
            //      d =  a(t^2)(-1 + 0.5)
            //      d = -0.5(a)(t^2)
            //      t^2 = -2(d)/a
            //      t = sqrt( -2(d)/a )
            //  
            //      Vi = -a(t)                      back to using 1. now that we have t we can calculate Vi.
            //      Vi = -a( sqrt( -2(d)/a ) )      the negative sign will disappear since our acceleration is negative

            if( m_isChargedJumpReady )
            {
                static float const bigJumpTime = Math::Sqrt( 2 * g_bigJumpDistance / g_gravityAcceleration );
                static float const bigJumpinitialSpeed = g_gravityAcceleration * bigJumpTime;
                ResultingVelocity.m_z = bigJumpinitialSpeed;
            }
            else
            {
                static float const smallJumpTime = Math::Sqrt( 2 * g_smallJumpDistance / g_gravityAcceleration );
                static float const smallJumpinitialSpeed = g_gravityAcceleration * smallJumpTime;
                ResultingVelocity.m_z = smallJumpinitialSpeed;
            }

            Vector const Facing = desiredHeadingVelocity2D.IsZero2() ? ctx.m_pCharacterComponent->GetForwardVector() : desiredHeadingVelocity2D.GetNormalized2();

            // Run physic Prediction if required
            //-------------------------------------------------------------------------
            // nothing for now

            // Update animation controller
            //-------------------------------------------------------------------------
            auto pLocomotionGraphController = ctx.GetAnimSubGraphController<LocomotionGraphController>();
            pLocomotionGraphController->SetLocomotionDesires( ctx.GetDeltaTime(), ResultingVelocity, Facing );
        }

        return Status::Running;
    }

    void JumpAction::StopInternal( ActionContext const& ctx, StopReason reason )
    {

    }

    void JumpAction::DrawDebugUI()
    {
        ImGui::Dummy( ImVec2( 0, 10 ) );
        ImGui::Text( "Settings :" );
        ImGui::Separator();

        ImGui::Dummy( ImVec2( 0, 10 ) );
        ImGui::Text( "Debug Values :" );
        ImGui::Separator();

        ImGui::Dummy( ImVec2( 0, 10 ) );
        ImGui::Text( "Debug drawings :" );
        ImGui::Separator();
    }
}