#include "PlayerAction_Falling.h"
#include "Game/Core/Player/PlayerPhysicsController.h"
#include "Game/Core/Player/PlayerCameraController.h"
#include "Engine/Physics/PhysicsQuery.h"
#include "Engine/Physics/Systems/WorldSystem_Physics.h"
#include "Engine/Physics/Components/Component_PhysicsCharacter.h"
#include "System/Input/InputSystem.h"

// hack for now
#include "Game/Core/Player/GraphControllers/PlayerGraphController_Locomotion.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    bool FallingAction::TryStartInternal( ActionContext const& ctx )
    {
        if( !ProbeForGround( ctx.m_pPhysicsWorld, ctx.m_pCharacterPhysicsComponent, ctx.m_pCharacterPhysicsComponent->GetCapsulePosition() ) )
        {
            ctx.m_pCharacterPhysicsController->EnableGravity();
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
        Float2 movementInputs = pControllerState->GetLeftAnalogStickValue();

        auto const& camFwd = ctx.m_pCameraController->GetCameraRelativeForwardVector2D();
        auto const& camRight = ctx.m_pCameraController->GetCameraRelativeRightVector2D();

        // Use last frame camera orientation
        Vector const forward = camFwd * movementInputs.m_y;
        Vector const right = camRight * movementInputs.m_x;
        Vector const desiredHeadingVelocity = ( forward + right ) * 6.0f;

        // Run physic Prediction if required
        //-------------------------------------------------------------------------
        // nothing for now

        // Update animation controller
        //-------------------------------------------------------------------------
        auto pLocomotionGraphController = ctx.GetAnimSubGraphController<LocomotionGraphController>();
        pLocomotionGraphController->SetLocomotionDesires( ctx.GetDeltaTime(), desiredHeadingVelocity, desiredHeadingVelocity.GetNormalized2() );

        if( ProbeForGround( ctx.m_pPhysicsWorld, ctx.m_pCharacterPhysicsComponent, ctx.m_pCharacterPhysicsComponent->GetCapsulePosition() ) )
        {
            return Status::Completed;
        }

        return Status::Running;
    }

    void FallingAction::StopInternal( ActionContext const& ctx, StopReason reason )
    {

    }

    bool FallingAction::ProbeForGround( Physics::PhysicsWorldSystem* pPhysicsWorld, Physics::CharacterComponent const* pCapsuleComponent, Vector const& startPosition )
    {
        pPhysicsWorld->AcquireReadLock();

        Physics::QueryFilter filter;
        filter.SetLayerMask( Physics::CreateLayerMask( Physics::Layers::Environment, Physics::Layers::Characters ) );
        filter.AddIgnoredEntity( pCapsuleComponent->GetEntityID() );

        Physics::SweepResults sweepResults;
        bool foundGround = pPhysicsWorld->CapsuleSweep( pCapsuleComponent->GetCapsuleCylinderPortionHalfHeight(), pCapsuleComponent->GetCapsuleRadius(), pCapsuleComponent->GetCapsuleOrientation(), startPosition, -Vector::UnitZ, 0.05f, filter, sweepResults );

        pPhysicsWorld->ReleaseReadLock();

        return foundGround;
    }
}