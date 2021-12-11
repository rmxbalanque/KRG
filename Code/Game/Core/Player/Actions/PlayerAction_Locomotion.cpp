#include "PlayerAction_Locomotion.h"
#include "Game/Core/Player/Components/Component_MainPlayer.h"
#include "Game/Core/Player/GraphControllers/PlayerGraphController_Locomotion.h"
#include "Game/Core/Player/PlayerPhysicsController.h"
#include "Game/Core/Player/PlayerCameraController.h"
#include "Engine/Physics/Components/Component_PhysicsCharacter.h"
#include "System/Input/InputSystem.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    static Radians  maxAngularSpeed = Radians( Degrees( 90 ) ); // radians/second
    static float    maxSprintSpeed = 6.5f;                      // meters/second
    static float    maxRunSpeed = 5.0f;                         // meters/second
    static float    timeToTriggerSprint = 1.0f;                 // seconds
    static float    sprintStickAmplitude = 0.8f;                // [0,1]

    static bool     DebugVelocity = false;

    //-------------------------------------------------------------------------

    bool LocomotionAction::TryStartInternal( ActionContext const& ctx )
    {
        ctx.m_pCharacterController->EnableGravity();
        return true;
    }

    Action::Status LocomotionAction::UpdateInternal( ActionContext const& ctx )
    {
        auto const pControllerState = ctx.m_pInputSystem->GetControllerState();
        KRG_ASSERT( pControllerState != nullptr );

        // Calculate desired player displacement
        //-------------------------------------------------------------------------
        Vector const movementInputs = pControllerState->GetLeftAnalogStickValue();
        float const stickAmplitude = movementInputs.GetLength2();

        auto const& camFwd = ctx.m_pCameraController->GetCameraRelativeForwardVector2D();
        auto const& camRight = ctx.m_pCameraController->GetCameraRelativeRightVector2D();

        // Check for sprint
        float const currentLinearSpeed = ctx.m_pCharacterComponent->GetCharacterVelocity().GetLength3();
        if( stickAmplitude > sprintStickAmplitude ) // TODO : Run prediction for collision
        {
            if( !m_timerStarted )
            {
                m_timer.Start();
                m_timerStarted = true;
            }
            else if( m_timer.GetElapsedTimeSeconds() > timeToTriggerSprint )
            {
                ctx.m_pPlayerComponent->m_sprintFlag = true;
            }
        }
        else
        {
            m_timer.Reset();
            m_timerStarted = false;
            ctx.m_pPlayerComponent->m_sprintFlag = false;
        }

        float const Speed = ctx.m_pPlayerComponent->m_sprintFlag ? maxSprintSpeed : maxRunSpeed;

        // Use last frame camera orientation
        Vector const forward = camFwd * movementInputs.m_y;
        Vector const right = camRight * movementInputs.m_x;
        Vector const desiredHeadingVelocity = ( forward + right ) * Speed;

        m_debugSpeed = desiredHeadingVelocity.GetLength3();

        // Run physic Prediction if required
        //-------------------------------------------------------------------------
        // nothing for now
        
        // Update animation controller
        //-------------------------------------------------------------------------
        auto pLocomotionGraphController = ctx.GetAnimSubGraphController<LocomotionGraphController>();
        pLocomotionGraphController->SetLocomotionDesires( ctx.GetDeltaTime(), desiredHeadingVelocity, desiredHeadingVelocity.GetNormalized2() );

        // Debug
        //-------------------------------------------------------------------------
        auto Debugger = ctx.m_pEntityUpdateContext->GetDrawingContext();
        if( DebugVelocity && !desiredHeadingVelocity.GetNormalized3().IsZero3() )
        {
            Vector const Origin = ctx.m_pCharacterComponent->GetPosition();
            Debugger.DrawArrow( Origin, Origin + desiredHeadingVelocity.GetNormalized3(), Colors::AliceBlue );
        }

        return Status::Running;
    }

    void LocomotionAction::StopInternal( ActionContext const& ctx, StopReason reason )
    {
        m_debugSpeed = 0.0f;
    }

    void LocomotionAction::DrawDebugUI()
    {
        ImGui::Dummy( ImVec2( 0, 10 ) );
        ImGui::Text( "Settings :" );
        ImGui::Separator();
        ImGui::InputFloat( "Max run speed (m/s)", &maxRunSpeed, 0.1f );
        ImGui::InputFloat( "Max sprint speed (m/s)", &maxSprintSpeed, 0.1f );
        ImGui::InputFloat( "Time to trigger sprint (s)", &timeToTriggerSprint, 0.1f );

        ImGui::Dummy( ImVec2( 0, 10 ) );
        ImGui::Text( "Debug Values :" );
        ImGui::Separator();
        ImGui::Text( "Speed : %.3f m/s", m_debugSpeed );

        ImGui::Dummy( ImVec2( 0, 10 ) );
        ImGui::Text( "Debug drawings :" );
        ImGui::Separator();
        ImGui::Checkbox( "Velocity", &DebugVelocity );
    }
}