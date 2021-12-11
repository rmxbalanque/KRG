#include "PlayerAction_Dash.h"
#include "Game/Core/Player/PlayerPhysicsController.h"
#include "Game/Core/Player/PlayerCameraController.h"
#include "Engine/Physics/Components/Component_PhysicsCharacter.h"
#include "System/Input/InputSystem.h"

// hack for now
#include "Game/Core/Player/GraphControllers/PlayerGraphController_Locomotion.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    Radians maxAngularSpeed = Radians( Degrees( 90 ) ); // radians
    float   dashDistance = 5.0f;                        // meters
    float   dashDuration = 0.15f;                       // seconds
    float   dashCooldown = 0.5f;                        // seconds

    bool    debugDistance = false;

    //-------------------------------------------------------------------------

    bool DashAction::TryStartInternal( ActionContext const& ctx )
    {
        if( ctx.m_pInputSystem->GetControllerState()->WasPressed( Input::ControllerButton::FaceButtonRight ) )
        {
            if( m_isFirstUse || m_timer.GetElapsedTimeSeconds() > dashCooldown )
            {
                ctx.m_pCharacterController->DisableGravity();

                auto const pControllerState = ctx.m_pInputSystem->GetControllerState();
                KRG_ASSERT( pControllerState != nullptr );

                // Use last frame camera orientation
                Vector movementInputs = pControllerState->GetLeftAnalogStickValue();

                if( movementInputs.GetLength2() > 0 )
                {
                    auto const& camFwd = ctx.m_pCameraController->GetCameraRelativeForwardVector2D();
                    auto const& camRight = ctx.m_pCameraController->GetCameraRelativeRightVector2D();
                    auto const forward = camFwd * movementInputs.m_y;
                    auto const right = camRight * movementInputs.m_x;
                    m_dashDirection = forward + right;
                }
                else
                {
                    m_dashDirection = ctx.m_pCharacterComponent->GetForwardVector();
                }
                m_initialVelocity = ctx.m_pCharacterComponent->GetCharacterVelocity() * Vector( 1.f, 1.f, 0.f );

                m_debugStartPosition = ctx.m_pCharacterComponent->GetPosition();

                m_isFirstUse = false;
                m_timer.Start();
                m_isInSettle = false;

                return true;
            }
        }

        return false;
    }

    Action::Status DashAction::UpdateInternal( ActionContext const& ctx )
    {
        auto const pControllerState = ctx.m_pInputSystem->GetControllerState();
        KRG_ASSERT( pControllerState != nullptr );

        // Calculate desired player displacement
        //-------------------------------------------------------------------------
        Vector const desiredVelocity = m_dashDirection * ( dashDistance / dashDuration );
        Quaternion const deltaOrientation = Quaternion::Identity;

        // Run physic Prediction if required
        //-------------------------------------------------------------------------
        // nothing for now
        
        // Update animation controller
        //-------------------------------------------------------------------------
        auto pLocomotionGraphController = ctx.GetAnimSubGraphController<LocomotionGraphController>();
        pLocomotionGraphController->SetLocomotionDesires(ctx.GetDeltaTime(), desiredVelocity, ctx.m_pCharacterComponent->GetForwardVector() );

        if( m_timer.GetElapsedTimeSeconds() > dashDuration && !m_isInSettle )
        {
            m_settleTimer.Start();
            m_isInSettle = true;
        }

        if( m_isInSettle  )
        {
            pLocomotionGraphController->SetLocomotionDesires( ctx.GetDeltaTime(), m_initialVelocity, ctx.m_pCharacterComponent->GetForwardVector() );

            if( m_settleTimer.GetElapsedTimeMilliseconds() > 100.f )
            {
                return Status::Completed;
            }
        }

        auto Debugger = ctx.m_pEntityUpdateContext->GetDrawingContext();
        if( debugDistance )
        {
            Vector const Origin = ctx.m_pCharacterComponent->GetPosition();
            Debugger.DrawArrow( m_debugStartPosition, m_debugStartPosition + m_dashDirection * dashDistance, Colors::HotPink );
        }

        return Status::Running;
    }

    void DashAction::StopInternal( ActionContext const& ctx, StopReason reason )
    {
        m_timer.Start();
    }

    void DashAction::DrawDebugUI()
    {
        ImGui::Dummy( ImVec2( 0, 10 ) );
        ImGui::Text( "Settings :" );
        ImGui::Separator();
        ImGui::InputFloat( "Dash distance", &dashDistance, 0.1f );
        ImGui::InputFloat( "Dash duration", &dashDuration, 0.1f );
        ImGui::InputFloat( "Dash cooldown", &dashCooldown, 0.1f );

        ImGui::Dummy( ImVec2( 0, 10 ) );
        ImGui::Text( "Debug Values :" );
        ImGui::Separator();
        ImGui::Text( "Cooldown : %.3f s", Math::Max( 0.0f, dashCooldown - m_timer.GetElapsedTimeSeconds() ) );

        ImGui::Dummy( ImVec2( 0, 10 ) );
        ImGui::Text( "Debug drawings :" );
        ImGui::Separator();
        ImGui::Checkbox( "Distance", &debugDistance );
    }
}