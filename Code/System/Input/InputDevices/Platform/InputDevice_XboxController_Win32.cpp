#ifdef _WIN32
#include "System/Input/InputDevices/InputDevice_XBoxController.h"
#include "System/Core/Math/Vector.h"
#include "System/Core/Types/Containers.h"
#include "System/Core/Logging/Log.h"
#include <windows.h>
#include <XInput.h>

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Input
    {
        static F32 const g_maxThumbstickRange = 32767.0f;
        static F32 const g_maxTriggerRange = 255.0f;

        //-------------------------------------------------------------------------

        void XBoxControllerInputDevice::Initialize()
        {
            XINPUT_STATE state;
            DWORD result = XInputGetState( m_hardwareControllerIdx, &state );
            m_isConnected = ( result == ERROR_SUCCESS );

            m_settings.m_leftStickInnerDeadzone = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE / g_maxThumbstickRange;
            m_settings.m_leftStickOuterDeadzone = 0.0f;
            m_settings.m_leftTriggerThreshold = XINPUT_GAMEPAD_TRIGGER_THRESHOLD / g_maxTriggerRange;

            m_settings.m_rightStickInnerDeadzone = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE / g_maxThumbstickRange;
            m_settings.m_rightStickOuterDeadzone = 0.0f;
            m_settings.m_rightTriggerThreshold = XINPUT_GAMEPAD_TRIGGER_THRESHOLD / g_maxTriggerRange;
        }

        void XBoxControllerInputDevice::Shutdown()
        {
            m_isConnected = false;
        }

        void XBoxControllerInputDevice::UpdateState()
        {
            Ticks const currentTime = GetSystemTime();

            XINPUT_STATE state;
            DWORD result = XInputGetState( m_hardwareControllerIdx, &state );
            m_isConnected = ( result == ERROR_SUCCESS );
            if ( m_isConnected )
            {
                // Set stick and trigger raw normalized values
                SetTriggerValues( state.Gamepad.bLeftTrigger / g_maxTriggerRange, state.Gamepad.bRightTrigger / g_maxTriggerRange );
                SetAnalogStickValues( Float2( state.Gamepad.sThumbLX / g_maxThumbstickRange, state.Gamepad.sThumbLY / g_maxThumbstickRange ), Float2( state.Gamepad.sThumbRX / g_maxThumbstickRange, state.Gamepad.sThumbRY / g_maxThumbstickRange ) );

                // Set button state
                UpdateButtonState( Input::ControllerButton::DPadUp, ( state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP ) > 0 );
                UpdateButtonState( Input::ControllerButton::DPadDown, ( state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN ) > 0 );
                UpdateButtonState( Input::ControllerButton::DPadLeft, ( state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT ) > 0 );
                UpdateButtonState( Input::ControllerButton::DPadRight, ( state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ) > 0 );
                UpdateButtonState( Input::ControllerButton::System0, ( state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK ) > 0);
                UpdateButtonState( Input::ControllerButton::System1, ( state.Gamepad.wButtons & XINPUT_GAMEPAD_START ) > 0 );
                UpdateButtonState( Input::ControllerButton::ThumbstickLeft, ( state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB ) > 0 );
                UpdateButtonState( Input::ControllerButton::ThumbstickRight, ( state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB ) > 0 );
                UpdateButtonState( Input::ControllerButton::ShoulderLeft, ( state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER ) > 0 );
                UpdateButtonState( Input::ControllerButton::ShoulderRight, ( state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER ) > 0 );
                UpdateButtonState( Input::ControllerButton::FaceButtonDown, ( state.Gamepad.wButtons & XINPUT_GAMEPAD_A ) > 0 );
                UpdateButtonState( Input::ControllerButton::FaceButtonRight, ( state.Gamepad.wButtons & XINPUT_GAMEPAD_B ) > 0 );
                UpdateButtonState( Input::ControllerButton::FaceButtonLeft, ( state.Gamepad.wButtons & XINPUT_GAMEPAD_X ) > 0 );
                UpdateButtonState( Input::ControllerButton::FaceButtonUp, ( state.Gamepad.wButtons & XINPUT_GAMEPAD_Y ) > 0 );
            }
            else
            {
                ClearControllerState();
            }
        }
    }
}
#endif