#include "PlayerAction_Jump.h"
#include "System/Input/InputSystem.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    bool JumpAction::TryStartInternal( ActionContext const& ctx )
    {
        Seconds holdDuration;
        if ( ctx.m_pInputSystem->GetControllerState()->WasReleased( Input::ControllerButton::FaceButtonDown, &holdDuration ) && holdDuration > 1.0f )
        {
            m_timer.Start();
            return true;
        }

        return false;
    }

    Action::Status JumpAction::UpdateInternal( ActionContext const& ctx )
    {
        if ( m_timer.GetElapsedTimeSeconds() >= 1.0f )
        {
            return Status::Completed;
        }

        return Status::Running;
    }

    void JumpAction::StopInternal( ActionContext const& ctx, StopReason reason )
    {

    }
}