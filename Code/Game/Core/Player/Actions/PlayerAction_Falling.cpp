#include "PlayerAction_Falling.h"
#include "Game/Core/Player/PlayerPhysicsController.h"
#include "Game/Core/Player/PlayerCameraController.h"
#include "Engine/Physics/Components/Component_PhysicsCharacter.h"
#include "Engine/Physics/Systems/WorldSystem_Physics.h"
#include "System/Input/InputSystem.h"

// hack for now
#include "Game/Core/Player/GraphControllers/PlayerGraphController_Locomotion.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    static Radians  maxAngularSpeed = Radians( Degrees( 90 ) ); // radians/second
    static float    maxAirControlSpeed = 6.0f;                  // meters/second
    static float    GroundDetectionDistance = 0.05f;            // meters

    //-------------------------------------------------------------------------

    bool FallingAction::TryStartInternal( ActionContext const& ctx )
    {
        if( !ProbeForGround( ctx.m_pPhysicsWorld, ctx.m_pCharacterComponent, ctx.m_pCharacterComponent->GetCapsulePosition() ) )
        {
            ctx.m_pCharacterController->EnableGravity();
            return true;
        }

        return false;
    }

    Action::Status FallingAction::UpdateInternal( ActionContext const& ctx )
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
        Vector const desiredHeadingVelocity = ( forward + right ) * maxAirControlSpeed;

        // Run physic Prediction if required
        //-------------------------------------------------------------------------
        // nothing for now

        // Update animation controller
        //-------------------------------------------------------------------------
        auto pLocomotionGraphController = ctx.GetAnimSubGraphController<LocomotionGraphController>();
        pLocomotionGraphController->SetLocomotionDesires( ctx.GetDeltaTime(), desiredHeadingVelocity, desiredHeadingVelocity.GetNormalized2() );

        if( ProbeForGround( ctx.m_pPhysicsWorld, ctx.m_pCharacterComponent, ctx.m_pCharacterComponent->GetCapsulePosition() ) )
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

    bool FallingAction::ProbeForGround( Physics::PhysicsWorldSystem* pPhysicsWorld, Physics::CharacterComponent const* pCharacterComponent, Vector const& startPosition )
    {
        pPhysicsWorld->AcquireReadLock();

        Physics::QueryFilter filter;
        filter.SetLayerMask( Physics::CreateLayerMask( Physics::Layers::Environment, Physics::Layers::Characters ) );
        filter.AddIgnoredEntity( pCharacterComponent->GetEntityID() );

        Physics::SweepResults sweepResults;
        bool foundGround = pPhysicsWorld->CapsuleSweep( pCharacterComponent->GetCapsuleCylinderPortionHalfHeight(), pCharacterComponent->GetCapsuleRadius(), pCharacterComponent->GetCapsuleOrientation(), startPosition, -Vector::UnitZ, 0.05f, filter, sweepResults );

        pPhysicsWorld->ReleaseReadLock();

        return foundGround;
    }
}