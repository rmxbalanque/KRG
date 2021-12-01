#pragma once

#include "InputState.h"

//-------------------------------------------------------------------------

namespace KRG::Input
{
    enum class KeyboardButton : uint16
    {
        Key_A = 0,
        Key_B,
        Key_C,
        Key_D,
        Key_E,
        Key_F,
        Key_G,
        Key_H,
        Key_I,
        Key_J,
        Key_K,
        Key_L,
        Key_M,
        Key_N,
        Key_O,
        Key_P,
        Key_Q,
        Key_R,
        Key_S,
        Key_T,
        Key_U,
        Key_V,
        Key_W,
        Key_X,
        Key_Y,
        Key_Z,
        Key_0,
        Key_1,
        Key_2,
        Key_3,
        Key_4,
        Key_5,
        Key_6,
        Key_7,
        Key_8,
        Key_9,
        Key_Comma,
        Key_Period,
        Key_ForwardSlash,
        Key_SemiColon,
        Key_Quote,
        Key_LBracket,
        Key_RBracket,
        Key_BackSlash,
        Key_Underscore,
        Key_Minus = Key_Underscore,
        Key_Equals,
        Key_Plus = Key_Equals,
        Key_Backspace,
        Key_Tilde,
        Key_Tab,
        Key_CapsLock,
        Key_Enter,
        Key_Escape,
        Key_Space,
        Key_Left,
        Key_Up,
        Key_Right,
        Key_Down,
        Key_NumLock,
        Key_Numpad0,
        Key_Numpad1,
        Key_Numpad2,
        Key_Numpad3,
        Key_Numpad4,
        Key_Numpad5,
        Key_Numpad6,
        Key_Numpad7,
        Key_Numpad8,
        Key_Numpad9,
        Key_NumpadEnter,
        Key_NumpadMultiply,
        Key_NumpadPlus,
        Key_NumpadMinus,
        Key_NumpadPeriod,
        Key_NumpadDivide,
        Key_F1,
        Key_F2,
        Key_F3,
        Key_F4,
        Key_F5,
        Key_F6,
        Key_F7,
        Key_F8,
        Key_F9,
        Key_F10,
        Key_F11,
        Key_F12,
        Key_F13,
        Key_F14,
        Key_F15,
        Key_PrintScreen,
        Key_ScrollLock,
        Key_Pause,
        Key_Insert,
        Key_Delete,
        Key_Home,
        Key_End,
        Key_PageUp,
        Key_PageDown,
        Key_Application,
        Key_LShift,
        Key_RShift,
        Key_LCtrl,
        Key_RCtrl,
        Key_LAlt,
        Key_RAlt,

        NumButtons,
    };

    //-------------------------------------------------------------------------

    class KeyboardInputState : public ButtonStates<105>
    {
        friend class KeyboardMouseInputDevice;

    public:

        inline uint8 GetCharKeyPressed() const { return m_charKeyPressed; }

        // Get the controller button states
        KRG_FORCE_INLINE bool WasPressed( KeyboardButton buttonID ) const { return ButtonStates::WasPressed( (uint32) buttonID ); }
        KRG_FORCE_INLINE bool WasReleased( KeyboardButton buttonID ) const { return ButtonStates::WasReleased( (uint32) buttonID ); }
        KRG_FORCE_INLINE bool IsHeldDown( KeyboardButton buttonID ) const { return ButtonStates::IsHeldDown( (uint32) buttonID ); }
        KRG_FORCE_INLINE Milliseconds GetHeldDuration( KeyboardButton buttonID ) const { return ButtonStates::GetHeldDuration( (uint32) buttonID ); }

        // Syntactic Sugar
        KRG_FORCE_INLINE bool WasEnterPressed() const { return WasPressed( KeyboardButton::Key_Enter ) || WasPressed( KeyboardButton::Key_NumpadEnter ); }
        KRG_FORCE_INLINE bool WasEnterReleased() const { return WasReleased( KeyboardButton::Key_Enter ) || WasReleased( KeyboardButton::Key_NumpadEnter ); }
        KRG_FORCE_INLINE bool IsEnterPressed() const { return IsHeldDown( KeyboardButton::Key_Enter ) || IsHeldDown( KeyboardButton::Key_NumpadEnter ); }
        KRG_FORCE_INLINE Milliseconds GetEnterPressedDuration() const { return Math::Max( GetHeldDuration( KeyboardButton::Key_Enter ), GetHeldDuration( KeyboardButton::Key_NumpadEnter ) ); }

        KRG_FORCE_INLINE bool WasShiftPressed() const { return WasPressed( KeyboardButton::Key_LShift ) || WasPressed( KeyboardButton::Key_RShift ); }
        KRG_FORCE_INLINE bool WasShiftReleased() const { return WasReleased( KeyboardButton::Key_LShift ) || WasReleased( KeyboardButton::Key_RShift ); }
        KRG_FORCE_INLINE bool IsShiftHeldDown() const { return IsHeldDown( KeyboardButton::Key_LShift ) || IsHeldDown( KeyboardButton::Key_RShift ); }
        KRG_FORCE_INLINE Milliseconds GetShiftPressedDuration() const { return Math::Max( GetHeldDuration( KeyboardButton::Key_LShift ), GetHeldDuration( KeyboardButton::Key_RShift ) ); }

        KRG_FORCE_INLINE bool WasCtrlPressed() const { return WasPressed( KeyboardButton::Key_LCtrl ) || WasPressed( KeyboardButton::Key_RCtrl ); }
        KRG_FORCE_INLINE bool WasCtrlReleased() const { return WasReleased( KeyboardButton::Key_LCtrl ) || WasReleased( KeyboardButton::Key_RCtrl ); }
        KRG_FORCE_INLINE bool IsCtrlHeldDown() const { return IsHeldDown( KeyboardButton::Key_LCtrl ) || IsHeldDown( KeyboardButton::Key_RCtrl ); }
        KRG_FORCE_INLINE Milliseconds GetCtrlPressedDuration() const { return Math::Max( GetHeldDuration( KeyboardButton::Key_LCtrl ), GetHeldDuration( KeyboardButton::Key_RCtrl ) ); }

        KRG_FORCE_INLINE bool WasAltPressed() const { return WasPressed( KeyboardButton::Key_LAlt ) || WasPressed( KeyboardButton::Key_RAlt ); }
        KRG_FORCE_INLINE bool WasAltReleased() const { return WasReleased( KeyboardButton::Key_LAlt ) || WasReleased( KeyboardButton::Key_RAlt ); }
        KRG_FORCE_INLINE bool IsAltHeldDown() const { return IsHeldDown( KeyboardButton::Key_LAlt ) || IsHeldDown( KeyboardButton::Key_RAlt ); }
        KRG_FORCE_INLINE Milliseconds GetAltPressedDuration() const { return Math::Max( GetHeldDuration( KeyboardButton::Key_LAlt ), GetHeldDuration( KeyboardButton::Key_RAlt ) ); }

    private:

        KRG_FORCE_INLINE void Press( KeyboardButton buttonID ) { ButtonStates::Press( (uint32) buttonID ); }
        KRG_FORCE_INLINE void Release( KeyboardButton buttonID ) { ButtonStates::Release( (uint32) buttonID ); }

        inline void ResetFrameState( ResetType resetType )
        {
            m_charKeyPressed = 0;

            if ( resetType == ResetType::Full )
            {
                ClearButtonState();
            }
        }

    private:

        uint8  m_charKeyPressed;
    };
}