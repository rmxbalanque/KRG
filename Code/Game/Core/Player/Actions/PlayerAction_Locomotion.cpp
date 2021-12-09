#include "PlayerAction_Locomotion.h"
#include "Game/Core/Player/PlayerPhysicsController.h"
#include "Game/Core/Player/GraphControllers/PlayerGraphController_Locomotion.h"
#include "Game/Core/Player/Components/Component_MainPlayer.h"
#include "Game/Core/Player/PlayerCameraController.h"
#include "Engine/Physics/Components/Component_PhysicsCharacter.h"
#include "System/Input/InputSystem.h"
#include "System/Core/Math/Vector.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    bool LocomotionAction::TryStartInternal( ActionContext const& ctx )
    {
        ctx.m_pCharacterPhysicsController->EnableGravity();
        return true;
    }

    Action::Status LocomotionAction::UpdateInternal( ActionContext const& ctx )
    {
        auto const pControllerState = ctx.m_pInputSystem->GetControllerState();
        KRG_ASSERT( pControllerState != nullptr );

        // Calculate desired player displacement
        //-------------------------------------------------------------------------
        Vector movementInputs = pControllerState->GetLeftAnalogStickValue();

        auto const& camFwd = ctx.m_pCameraController->GetCameraRelativeForwardVector2D();
        auto const& camRight = ctx.m_pCameraController->GetCameraRelativeRightVector2D();

        // Check for sprint
        float const stickAmplitude = movementInputs.GetLength2();
        float const currentLinearSpeed = ctx.m_pCharacterPhysicsComponent->GetCharacterVelocity().GetLength3();
        if( stickAmplitude > m_sprintStickAmplitude && currentLinearSpeed > 4.8f )
        {
            if( !m_timerStarted )
            {
                m_timer.Start();
                m_timerStarted = true;
            }
            else if( m_timer.GetElapsedTimeSeconds() > m_sprintThreshold )
            {
                ctx.m_pPlayerComponent->m_sprintFlag = true;
            }
        }
        else
        {
            m_timer.Reset();
            m_timerStarted = false;
            ctx.m_pPlayerComponent->m_sprintFlag = false;
        }

        float const Speed = ctx.m_pPlayerComponent->m_sprintFlag ? m_maxSprintLinearSpeed : m_maxWalkLinearSpeed;

        // Use last frame camera orientation
        Vector const forward = camFwd * movementInputs.m_y;
        Vector const right = camRight * movementInputs.m_x;
        Vector const desiredHeadingVelocity = ( forward + right ) * Speed;

        // Run physic Prediction if required
        //-------------------------------------------------------------------------
        // nothing for now
        
        // Update animation controller
        //-------------------------------------------------------------------------

        auto pLocomotionGraphController = ctx.GetAnimSubGraphController<LocomotionGraphController>();
        pLocomotionGraphController->SetLocomotionDesires( ctx.GetDeltaTime(), desiredHeadingVelocity, desiredHeadingVelocity.GetNormalized2() );

        return Status::Running;
    }

    void LocomotionAction::StopInternal( ActionContext const& ctx, StopReason reason )
    {

    }
}