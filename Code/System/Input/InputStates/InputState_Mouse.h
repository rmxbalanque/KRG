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

            inline Float2 GetMovementDelta() const { return m_movementDelta; }
            inline int16 GetWheelDelta( WheelType wheelType = WheelType::Vertical ) const { return ( wheelType == WheelType::Vertical ) ? m_verticalWheelDelta : m_horizontalWheelDelta; }
            inline Int2 GetCursorPosition() const { return m_position; }

            KRG_FORCE_INLINE bool WasPressed( MouseButton buttonID ) const { return ButtonStates::WasPressed( (uint32) buttonID ); }
            KRG_FORCE_INLINE bool WasReleased( MouseButton buttonID ) const { return ButtonStates::WasReleased( (uint32) buttonID ); }
            KRG_FORCE_INLINE bool IsHeldDown( MouseButton buttonID ) const { return ButtonStates::IsHeldDown( (uint32) buttonID ); }
            KRG_FORCE_INLINE Milliseconds GetHeldDuration( MouseButton buttonID ) const { return ButtonStates::GetHeldDuration( (uint32) buttonID ); }

        private:

            KRG_FORCE_INLINE void Press( MouseButton buttonID ) { ButtonStates::Press( (uint32) buttonID ); }
            KRG_FORCE_INLINE void Release( MouseButton buttonID ) { ButtonStates::Release( (uint32) buttonID ); }

            inline void ResetFrameState( ResetType resetType )
            {
                m_movementDelta = Float2::Zero;
                m_verticalWheelDelta = m_horizontalWheelDelta = 0;

                if ( resetType == ResetType::Full )
                {
                    ClearButtonState();
                }
            }

        private:

            Float2                                  m_movementDelta;
            Int2                                    m_position;
            int16                                   m_verticalWheelDelta = 0;
            int16                                   m_horizontalWheelDelta = 0;
        };
    }
}