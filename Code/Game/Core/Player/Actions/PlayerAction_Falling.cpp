#include "PlayerAction_Falling.h"
#include "Game/Core/Player/PlayerPhysicsController.h"
#include "Game/Core/Player/PlayerCameraController.h"
#include "Game/Core/Player/PlayerAnimationController.h"
#include "Engine/Physics/Components/Component_PhysicsCharacter.h"
#include "Engine/Physics/Systems/WorldSystem_Physics.h"
#include "System/Input/InputSystem.h"

// hack for now
#include "Game/Core/Player/GraphControllers/PlayerGraphController_Locomotion.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    static float    g_maxAirControlAcceleration = 10.0f;          // meters/second squared
    static float    g_maxAirControlSpeed = 6.5f;                  // meters/second

    //-------------------------------------------------------------------------

    bool FallingAction::TryStartInternal( ActionContext const& ctx )
    {
        if( !ctx.m_pCharacterController->IsOnGround() )
        {
            ctx.m_pAnimationController->SetCharacterState( CharacterAnimationState::Falling );
            ctx.m_pCharacterController->EnableGravity( ctx.m_pCharacterComponent->GetCharacterVelocity().m_z );
            return true;
        }

        return false;
    }

    Action::Status FallingAction::UpdateInternal( ActionContext const& ctx )
    {
        auto const pControllerState = ctx.m_pInputState->GetControllerState();
        KRG_ASSERT( pControllerState != nullptr );

        // Calculate desired player displacement
        //-------------------------------------------------------------------------
        Vector const movementInputs = pControllerState->GetLeftAnalogStickValue();

        auto const& camFwd = ctx.m_pCameraController->GetCameraRelativeForwardVector2D();
        auto const& camRight = ctx.m_pCameraController->GetCameraRelativeRightVector2D();

        // Use last frame camera orientation
        Vector const currentVelocity = ctx.m_pCharacterComponent->GetCharacterVelocity();
        Vector const currentVelocity2D = currentVelocity * Vector( 1.0f, 1.0f, 0.0f );
        float const currentSpeed2D = currentVelocity2D.GetLength2();

        Vector const forward = camFwd * movementInputs.m_y;
        Vector const right = camRight * movementInputs.m_x;
        Vector const desiredHeadingVelocity2D = ( forward + right ) * g_maxAirControlAcceleration * ctx.GetDeltaTime();

        Vector resultingVelocity = currentVelocity2D + desiredHeadingVelocity2D;
        float const speed2D = resultingVelocity.GetLength2();
        if( speed2D > g_maxAirControlSpeed )
        {
            resultingVelocity = resultingVelocity.GetNormalized2() * g_maxAirControlSpeed;
        }

        Vector const facing = desiredHeadingVelocity2D.IsZero2() ? ctx.m_pCharacterComponent->GetForwardVector() : desiredHeadingVelocity2D.GetNormalized2();

        // Run physic Prediction if required
        //-------------------------------------------------------------------------
        // nothing for now

        // Update animation controller
        //-------------------------------------------------------------------------
        auto pLocomotionGraphController = ctx.GetAnimSubGraphController<LocomotionGraphController>();
        pLocomotionGraphController->SetLocomotionDesires( ctx.GetDeltaTime(), resultingVelocity, facing );

        if( ctx.m_pCharacterController->IsOnGround() )
        {
            return Status::Completed;
        }

        return Status::Running;
    }

    void FallingAction::StopInternal( ActionContext const& ctx, StopReason reason )
    {

    }

    void FallingAction::DrawDebugUI()
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