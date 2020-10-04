#include "InputDevice_KeyboardMouse.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Input
    {
        void KeyboardMouseInputDevice::UpdateState()
        {
            m_mouseState.Update();
            m_keyboardState.Update();
        }

        void KeyboardMouseInputDevice::ClearFrameState()
        {
            m_mouseState.ResetFrameState();
            m_keyboardState.ResetFrameState();
        }
    }
}