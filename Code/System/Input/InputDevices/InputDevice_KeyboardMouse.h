#pragma once

#include "InputDevice.h"
#include "System/Input/InputStates/InputState_Mouse.h"
#include "System/Input/InputStates/InputState_Keyboard.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Input
    {
        class KeyboardMouseInputDevice : public InputDevice
        {

        public:

            KeyboardMouseInputDevice() = default;

            inline MouseInputState const& GetMouseState() const { return m_mouseState; }
            inline KeyboardInputState const& GetKeyboardState() const { return m_keyboardState; }
     
        private:

            virtual DeviceCategory GetDeviceCategory() const override final { return DeviceCategory::KeyboardMouse; }

            virtual void Initialize() override final;
            virtual void Shutdown() override final;

            virtual void UpdateState() override final;
            virtual void ClearFrameState() override final;
            virtual void ProcessMessage( GenericMessage const& message ) override final;

        private:

            MouseInputState                                         m_mouseState;
            KeyboardInputState                                      m_keyboardState;
        };
    }
}