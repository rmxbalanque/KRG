#pragma once

#include "InputState.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Input
    {
        enum class MouseButton
        {
            Left = 0,
            Right,
            Middle,
            Back,
            Forward,
            Button5,
            Button6,
            Button7,
            Button8,
            Button9,
        };

        //-------------------------------------------------------------------------

        class MouseInputState : public ButtonStates<10>
        {
            friend class KeyboardMouseInputDevice;

        public:

            enum class WheelType
            {
                Vertical,
                Horizontal
            };

        public:

            inline Int2 GetMovementDelta() const { return m_movementDelta; }
            inline S16 GetWheelDelta( WheelType wheelType = WheelType::Vertical ) const { return ( wheelType == WheelType::Vertical ) ? m_verticalWheelDelta : m_horizontalWheelDelta; }
            inline Int2 GetCursorPosition() const { return m_position; }

            KRG_FORCE_INLINE bool WasPressed( MouseButton buttonID ) const { return ButtonStates::WasPressed( (U32) buttonID ); }
            KRG_FORCE_INLINE bool WasReleased( MouseButton buttonID ) const { return ButtonStates::WasReleased( (U32) buttonID ); }
            KRG_FORCE_INLINE bool IsHeldDown( MouseButton buttonID ) const { return ButtonStates::IsHeldDown( (U32) buttonID ); }
            KRG_FORCE_INLINE Milliseconds GetHeldDuration( MouseButton buttonID ) const { return ButtonStates::GetHeldDuration( (U32) buttonID ); }

        private:

            KRG_FORCE_INLINE void Press( MouseButton buttonID ) { ButtonStates::Press( (U32) buttonID ); }
            KRG_FORCE_INLINE void Release( MouseButton buttonID ) { ButtonStates::Release( (U32) buttonID ); }

            inline void ResetFrameState()
            {
                m_movementDelta = Int2::Zero;
                m_verticalWheelDelta = m_horizontalWheelDelta = 0;
            }

        private:

            Int2                                    m_movementDelta;
            Int2                                    m_position;
            S16                                     m_verticalWheelDelta = 0;
            S16                                     m_horizontalWheelDelta = 0;
        };
    }
}