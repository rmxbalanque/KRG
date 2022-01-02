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
    static Radians  g_maxAngularSpeed = Radians( Degrees( 90 ) ); // radians/second
    static float    g_maxSprintSpeed = 7.5f;                      // meters/second
    static float    g_maxRunSpeed = 5.0f;                         // meters/second
    static Seconds  g_timeToTriggerSprint = 2.25f;                // seconds
    static float    g_sprintStickAmplitude = 0.8f;                // [0,1]

    static bool     g_debugVelocity = false;

    //-------------------------------------------------------------------------

    bool LocomotionAction::TryStartInternal( ActionContext const& ctx )
    {
        ctx.m_pAnimationController->SetCharacterState( CharacterAnimationState::Locomotion );
        ctx.m_pCharacterController->EnableGravity( 0.0f /* it is assumed we are on the floor */ );
        ctx.m_pCharacterController->EnableProjectionOntoFloor();

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

        // Check for sprint
        float const currentLinearSpeed = ctx.m_pCharacterComponent->GetCharacterVelocity().GetLength3();
        if( stickAmplitude > g_sprintStickAmplitude ) // TODO : Run prediction for collision
        {
            if ( !ctx.m_pPlayerComponent->m_sprintFlag )
            {
                if( !m_timer.IsRunning() )
                {
                    m_timer.Start( g_timeToTriggerSprint );
                }

                if ( m_timer.Update() )
                {
                    ctx.m_pPlayerComponent->m_sprintFlag = true;
                }
            }
        }
        else
        {
            m_timer.Stop();
            ctx.m_pPlayerComponent->m_sprintFlag = false;
        }

        float const Speed = ctx.m_pPlayerComponent->m_sprintFlag ? g_maxSprintSpeed : g_maxRunSpeed;

        // Use last frame camera orientation
        Vector const forward = camFwd * movementInputs.m_y;
        Vector const right = camRight * movementInputs.m_x;
        Vector const desiredHeadingVelocity2D = ( forward + right ) * Speed;
        Vector const ResultingVelocity = desiredHeadingVelocity2D;
        Vector const Facing = desiredHeadingVelocity2D.IsZero2() ? ctx.m_pCharacterComponent->GetForwardVector() : desiredHeadingVelocity2D.GetNormalized2();

        m_debugSpeed = desiredHeadingVelocity2D.GetLength3();

        // Run physic Prediction if required
        //-------------------------------------------------------------------------
        // nothing for now
        
        // Update animation controller
        //-------------------------------------------------------------------------
        auto pLocomotionGraphController = ctx.GetAnimSubGraphController<LocomotionGraphController>();
        pLocomotionGraphController->SetLocomotionDesires( ctx.GetDeltaTime(), ResultingVelocity, Facing );

        // Debug
        //-------------------------------------------------------------------------
        auto Debugger = ctx.m_pEntityWorldUpdateContext->GetDrawingContext();
        if( g_debugVelocity && !ResultingVelocity.GetNormalized3().IsZero3() )
        {
            Vector const Origin = ctx.m_pCharacterComponent->GetPosition();
            Debugger.DrawArrow( Origin, Origin + ResultingVelocity.GetNormalized3(), Colors::AliceBlue );
        }

        return Status::Running;
    }

    void LocomotionAction::StopInternal( ActionContext const& ctx, StopReason reason )
    {
        m_debugSpeed = 0.0f;
        ctx.m_pPlayerComponent->m_sprintFlag = false;
        m_timer.Stop();
    }

    void LocomotionAction::DrawDebugUI()
    {
        ImGui::Dummy( ImVec2( 0, 10 ) );
        ImGui::Text( "Settings :" );
        ImGui::Separator();
        ImGui::InputFloat( "Max run speed (m/s)", &g_maxRunSpeed, 0.1f );
        ImGui::InputFloat( "Max sprint speed (m/s)", &g_maxSprintSpeed, 0.1f );

        float sprintTriggerTime = g_timeToTriggerSprint.ToFloat();
        if ( ImGui::InputFloat( "Time to trigger sprint (s)", &sprintTriggerTime, 0.1f ) )
        {
            g_timeToTriggerSprint = sprintTriggerTime;
        }

        ImGui::Dummy( ImVec2( 0, 10 ) );
        ImGui::Text( "Debug Values :" );
        ImGui::Separator();
        ImGui::Text( "Speed : %.3f m/s", m_debugSpeed );

        ImGui::Dummy( ImVec2( 0, 10 ) );
        ImGui::Text( "Debug drawings :" );
        ImGui::Separator();
        ImGui::Checkbox( "Velocity", &g_debugVelocity );
    }
}