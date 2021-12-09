#include "PlayerCameraController.h"
#include "Engine/Core/Components/Component_Cameras.h"
#include "Engine/Core/Entity/EntityUpdateContext.h"
#include "System/Input/InputSystem.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    CameraController::CameraController( OrbitCameraComponent* pCamera )
        : m_pCamera( pCamera )
    {
        KRG_ASSERT( pCamera != nullptr );
    }

    void CameraController::UpdateCamera( EntityUpdateContext const& ctx )
    {
        auto pInputSystem = ctx.GetSystem<Input::InputSystem>();
        auto const pControllerState = pInputSystem->GetControllerState();

        // Update camera rotation
        //-------------------------------------------------------------------------

        Vector cameraInputs = pControllerState->GetRightAnalogStickValue();
        Radians const maxAngularVelocityForThisFrame = Math::Pi * ctx.GetDeltaTime();
        cameraInputs *= (float) maxAngularVelocityForThisFrame;
        m_pCamera->AdjustOrbitAngle( cameraInputs.m_x, cameraInputs.m_y );

        //-------------------------------------------------------------------------

        m_cameraRelativeForwardVector = m_pCamera->GetCameraRelativeForwardVector();
        m_cameraRelativeRightVector = m_pCamera->GetCameraRelativeRightVector();
        m_cameraRelativeForwardVector2D = m_cameraRelativeForwardVector.GetNormalized2();
        m_cameraRelativeRightVector2D = m_cameraRelativeRightVector.GetNormalized2();
    }

    void CameraController::FinalizeCamera()
    {
        m_pCamera->FinalizeCameraPosition();
    }
}