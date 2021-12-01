#include "PlayerGameplayState_Locomotion.h"
#include "PlayerGameplayState.h"
#include "Game/Core/Player/Components/Component_PlayerGameplayPhysics.h"
#include "Game/Core/Player/PhysicsStates/PlayerPhysicsState_Locomotion.h"
#include "Engine/Core/Components/Component_Cameras.h"
#include "System/Input/InputSystem.h"
#include "System/Core/Math/Vector.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    GameplayState::Status LocomotionGameplayState::Update( GameplayStateContext const& ctx )
    {
        auto pInputSystem = ctx.GetSystem<Input::InputSystem>();
        KRG_ASSERT( pInputSystem != nullptr );

        if ( pInputSystem->GetNumConnectedControllers() == 0 )
        {
            return Status::Running;
        }

        auto const pControllerState = pInputSystem->GetControllerState();
        KRG_ASSERT( pControllerState != nullptr );

        // Update camera rotation
        //-------------------------------------------------------------------------

        Vector cameraInputs = pControllerState->GetRightAnalogStickValue();

        Radians const maxAngularVelocity = Math::Pi;
        Radians maxAngularVelocityForThisFrame = maxAngularVelocity * ctx.GetDeltaTime();

        cameraInputs.Normalize2();
        cameraInputs *= maxAngularVelocityForThisFrame;
        ctx.m_pCameraComponent->AdjustOrbitAngle( cameraInputs.m_x, cameraInputs.m_y );

        // Move Player
        //-------------------------------------------------------------------------
        Float2 movementInputs = pControllerState->GetLeftAnalogStickValue();

        float const maxLinearVelocity = 12.0f;
        float maxLinearVelocityForThisFrame = maxLinearVelocity * ctx.GetDeltaTime();

        auto const camFwd = ctx.m_pCameraComponent->GetCameraRelativeForwardVector().GetNormalized2();
        auto const camRight = ctx.m_pCameraComponent->GetCameraRelativeRightVector().GetNormalized2();

        // Use last frame camera orientation
        auto forward = camFwd * movementInputs.m_y;
        auto right = camRight * movementInputs.m_x;

        auto pLocomotionPhysicsState = ctx.m_pGameplayPhysicsComponent->GetActivePhysicsState<PlayerLocomotionPhysicsState>();

        pLocomotionPhysicsState->m_deltaMovementHack = ( forward * maxLinearVelocityForThisFrame ) + ( right * maxLinearVelocityForThisFrame );

        if ( !pLocomotionPhysicsState->m_deltaMovementHack.IsZero2() )
        {
            pLocomotionPhysicsState->m_deltaRotationHack = Quaternion::FromNormalizedOrientationVector( pLocomotionPhysicsState->m_deltaMovementHack.GetNormalized2() );
        }

        return Status::Running;
    }

    void LocomotionGameplayState::Stop( GameplayStateContext const& ctx, StopReason reason )
    {

    }
}