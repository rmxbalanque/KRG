#include "PlayerAction_Locomotion.h"
#include "Game/Core/Player/PhysicsStates/PlayerPhysicsState_Ground.h"
#include "Game/Core/Player/GraphControllers/PlayerGraphController_Locomotion.h"
#include "Engine/Core/Components/Component_Cameras.h"
#include "Engine/Physics/Components/Component_PhysicsCharacter.h"
#include "System/Input/InputSystem.h"
#include "System/Core/Math/Vector.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    bool LocomotionAction::TryStartInternal( ActionContext const& ctx )
    {
        ctx.m_pPhysicsController->SetPhysicsState<GroundPhysicsState>( Physics::PhysicsStateController::DoNothingIfAlreadyActive );
        return true;
    }

    Action::Status LocomotionAction::UpdateInternal( ActionContext const& ctx )
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
        ctx.m_pCameraComponent->AdjustOrbitAngle( cameraInputs.m_x * 0.5f, cameraInputs.m_y * 0.5f );

        // Calculate desired player displacement
        //-------------------------------------------------------------------------
        Float2 movementInputs = pControllerState->GetLeftAnalogStickValue();

        float const maxLinearVelocity = 12.0f;
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
        pLocomotionGraphController->SetSpeed( desiredSpeed );

        // HACK (this is only because we don't have animation yet)
        //-------------------------------------------------------------------------
        ctx.m_pCharacterPhysicsComponent->m_deltaTranslationHACK = desiredDeltaDisplacement;
        ctx.m_pCharacterPhysicsComponent->m_deltaRotationHACK = deltaOrientation;
        //-------------------------------------------------------------------------

        return Status::Running;
    }

    void LocomotionAction::StopInternal( ActionContext const& ctx, StopReason reason )
    {

    }
}