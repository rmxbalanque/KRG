#include "PlayerAction_Jump.h"
#include "Game/Core/Player/PlayerPhysicsController.h"
#include "Game/Core/Player/PlayerCameraController.h"
#include "Engine/Physics/Components/Component_PhysicsCharacter.h"
#include "System/Input/InputSystem.h"

// hack for now
#include "Game/Core/Player/GraphControllers/PlayerGraphController_Locomotion.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    static Radians  maxAngularSpeed = Radians( Degrees( 90 ) ); // radians/second
    static float    maxAirControlSpeed = 6.0f;                  // meters/second
    static float    smallJumpVelocity = 4.0f;                   // meters/second
    static float    bigJumpVelocity = 7.0f;                     // meters/second
    static Seconds  BigJumpHoldTime = 0.3f;                     // seconds

    //-------------------------------------------------------------------------

    bool JumpAction::TryStartInternal( ActionContext const& ctx )
    {
        if( ctx.m_pInputSystem->GetControllerState()->WasReleased( Input::ControllerButton::FaceButtonDown ) )
        {
            ctx.m_pCharacterController->EnableGravity();
            m_timer.Start();
            return true;
        }
        else // Check hold state
        {
            m_isChargedJumpReady = false;
            Seconds jumpHoldTime = 0.0f;
            if( ctx.m_pInputSystem->GetControllerState()->IsHeldDown( Input::ControllerButton::FaceButtonDown, &jumpHoldTime ) )
            {
                if( jumpHoldTime > BigJumpHoldTime )
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
            auto const pControllerState = ctx.m_pInputSystem->GetControllerState();
            KRG_ASSERT( pControllerState != nullptr );

            // Calculate desired player displacement
            //-------------------------------------------------------------------------
            Vector const movementInputs = pControllerState->GetLeftAnalogStickValue();
            auto const& camFwd = ctx.m_pCameraController->GetCameraRelativeForwardVector2D();
            auto const& camRight = ctx.m_pCameraController->GetCameraRelativeRightVector2D();

            // Use last frame camera orientation
            Vector const forward = camFwd * movementInputs.m_y;
            Vector const right = camRight * movementInputs.m_x;
            Vector desiredHeadingVelocity = ( forward + right ) * maxAirControlSpeed;

            if( m_isChargedJumpReady )
            {
                desiredHeadingVelocity += Vector( 0.f, 0.f, bigJumpVelocity );
            }
            else
            {
                desiredHeadingVelocity += Vector( 0.f, 0.f, smallJumpVelocity );
            }

            // Run physic Prediction if required
            //-------------------------------------------------------------------------
            // nothing for now

            // Update animation controller
            //-------------------------------------------------------------------------
            auto pLocomotionGraphController = ctx.GetAnimSubGraphController<LocomotionGraphController>();
            pLocomotionGraphController->SetLocomotionDesires( ctx.GetDeltaTime(), desiredHeadingVelocity, desiredHeadingVelocity.GetNormalized2() );

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