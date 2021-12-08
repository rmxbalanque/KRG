#include "PlayerAction_Falling.h"
#include "Game/Core/Player/PhysicsStates/PlayerPhysicsState_Air.h"
#include "Engine/Core/Components/Component_Cameras.h"
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
            return true;
        }

        return false;
    }

    Action::Status FallingAction::UpdateInternal( ActionContext const& ctx )
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
            deltaOrientation = Quaternion::FromRotationBetweenNormalizedVectors( ctx.m_pCharacterPhysicsComponent->GetForwardVector().GetNormalized2(), desiredDeltaDisplacement.GetNormalized2() );
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