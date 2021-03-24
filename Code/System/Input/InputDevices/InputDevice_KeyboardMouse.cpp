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

        void KeyboardMouseInputDevice::ClearFrameState( ResetType resetType )
        {
            m_mouseState.ResetFrameState( resetType);
            m_keyboardState.ResetFrameState( resetType );
        }
    }
}