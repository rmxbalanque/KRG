#include "PlayerGameplayState_Locomotion.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    void LocomotionState::Start( GameplayStateContext const& ctx )
    {

    }

    bool LocomotionState::Update( GameplayStateContext const& ctx )
    {
        //Seconds const deltaTime = ctx.GetDeltaTime();

        //auto pInputSystem = ctx.GetSystem<Input::InputSystem>();
        //KRG_ASSERT( pInputSystem != nullptr );

        //if ( pInputSystem->GetNumConnectedControllers() == 0 )
        //{
        //    m_pCameraComponent->FinalizeCameraPosition();
        //    return;
        //}

        ////-------------------------------------------------------------------------

        //auto const pControllerState = pInputSystem->GetControllerState();
        //KRG_ASSERT( pControllerState != nullptr );

        //if( pControllerState->WasPressed( Input::ControllerButton::FaceButtonDown ) )
        //{
        //    m_verticalVelocity += 55.0f * ctx.GetDeltaTime();
        //}

        //// Update camera rotation
        ////-------------------------------------------------------------------------

        //Vector cameraInputs = pControllerState->GetRightAnalogStickValue();

        //Radians const maxAngularVelocity = Math::Pi;
        //Radians maxAngularVelocityForThisFrame = maxAngularVelocity * ctx.GetDeltaTime();

        //cameraInputs.Normalize2();
        //cameraInputs *= maxAngularVelocityForThisFrame;
        //m_pCameraComponent->AdjustOrbitAngle( cameraInputs.m_x, cameraInputs.m_y );

        //// Move Player
        ////-------------------------------------------------------------------------

        //Float2 movementInputs = pControllerState->GetLeftAnalogStickValue();

        //float const maxLinearVelocity = 12.0f;
        //float maxLinearVelocityForThisFrame = maxLinearVelocity * ctx.GetDeltaTime();

        //auto const camFwd = m_pCameraComponent->GetCameraRelativeForwardVector().GetNormalized2();
        //auto const camRight = m_pCameraComponent->GetCameraRelativeRightVector().GetNormalized2();

        //// Use last frame camera orientation
        //auto forward = camFwd * movementInputs.m_y;
        //auto right = camRight * movementInputs.m_x;
        //auto deltaMovement = ( forward * maxLinearVelocityForThisFrame ) + ( right * maxLinearVelocityForThisFrame );

        //Vector direction;
        //Vector speedVector;
        //deltaMovement.ToDirectionAndLength2( direction, speedVector );
        //float speed = speedVector.ToFloat();
        //if ( speed > maxLinearVelocityForThisFrame )
        //{
        //    deltaMovement = direction * maxLinearVelocityForThisFrame;
        //    speed = maxLinearVelocityForThisFrame;
        //}

        //Vector const startPosition = m_pCapsuleComponent->GetPosition();

        //

        ////-------------------------------------------------------------------------

        //Transform currentRootTransform = m_pRootComponent->GetWorldTransform();
        //if( !deltaMovement.IsZero3() )
        //{
        //    if( !deltaMovement.IsZero2() )
        //    {
        //        Vector const deltaMovement2D = deltaMovement.GetNormalized2();
        //        currentRootTransform.SetRotation( Quaternion::FromOrientationVector( deltaMovement2D ) );
        //    }
        //    currentRootTransform.AddTranslationOffset( deltaMovement );
        //}
        //m_pRootComponent->SetWorldTransform( currentRootTransform );

        return true;
    }

    void LocomotionState::Stop( GameplayStateContext const& ctx )
    {

    }
}