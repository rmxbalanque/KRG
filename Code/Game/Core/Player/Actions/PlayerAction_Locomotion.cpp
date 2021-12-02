#include "PlayerAction_Locomotion.h"
#include "Game/Core/Player/Components/Component_PlayerPhysics.h"
#include "Game/Core/Player/PhysicsStates/PlayerPhysicsState_Locomotion.h"
#include "Engine/Core/Components/Component_Cameras.h"
#include "System/Input/InputSystem.h"
#include "System/Core/Math/Vector.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
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
        cameraInputs *= maxAngularVelocityForThisFrame;
        ctx.m_pCameraComponent->AdjustOrbitAngle( cameraInputs.m_x, cameraInputs.m_y );

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
        // For now the desired delta rotation is the actual rotation and not a delta because it would be hard to calculate because of the hackery for the animation
        Quaternion const desiredDeltaRotation = !desiredDeltaDisplacement.IsZero2() ? Quaternion::FromNormalizedOrientationVector( desiredDeltaDisplacement.GetNormalized2() ) : Quaternion::Identity;

        // Run physic Prediction if required
        //-------------------------------------------------------------------------
        // nothing for now
        
        // Update animation controller
        //-------------------------------------------------------------------------
        // nothing for now

        // HACK (this is only because we don't have animation yet)
        //-------------------------------------------------------------------------
        auto pLocomotionPhysicsState = ctx.m_pPhysicsComponent->GetActivePhysicsState<PlayerLocomotionPhysicsState>();
        pLocomotionPhysicsState->m_deltaMovementHack = desiredDeltaDisplacement;
        pLocomotionPhysicsState->m_deltaRotationHack = desiredDeltaRotation;
        //-------------------------------------------------------------------------

        return Status::Running;
    }

    void LocomotionAction::StopInternal( ActionContext const& ctx, StopReason reason )
    {

    }
}