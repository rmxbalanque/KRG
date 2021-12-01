#include "PlayerGameplayState_Jump.h"
#include "System/Input/InputSystem.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    bool JumpGameplayState::TryStart( GameplayStateContext const& ctx )
    {
        auto pInputSystem = ctx.GetSystem<Input::InputSystem>();
        KRG_ASSERT( pInputSystem != nullptr );

        Seconds holdDuration;
        if ( pInputSystem->GetControllerState()->WasReleased( Input::ControllerButton::FaceButtonDown, &holdDuration ) && holdDuration > 1.0f )
        {
            m_timer.Start();
            return true;
        }

        return false;
    }

    GameplayState::Status JumpGameplayState::Update( GameplayStateContext const& ctx )
    {
        if ( m_timer.GetElapsedTimeSeconds() >= 1.0f )
        {
            return Status::Completed;
        }

        return Status::Running;
    }

    void JumpGameplayState::Stop( GameplayStateContext const& ctx, StopReason reason )
    {

    }
}