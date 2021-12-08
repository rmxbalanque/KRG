#pragma once

#include "InputState.h"

//-------------------------------------------------------------------------

namespace KRG::Input
{
    enum class ControllerButton : uint16
    {
        DPadUp = 0,
        DPadDown,
        DPadLeft,
        DPadRight,
        ThumbstickLeft,
        ThumbstickRight,
        ShoulderLeft,
        ShoulderRight,
        System0,
        System1,
        FaceButtonDown,
        FaceButtonRight,
        FaceButtonLeft,
        FaceButtonUp,
    };

    //-------------------------------------------------------------------------

    class ControllerInputState : public ButtonStates<14>
    {
        friend class ControllerInputDevice;

        enum Direction
        {
            Left = 0,
            Right = 1,
        };

    public:

        ControllerInputState()
        {
            ClearState();
        }

        // Get the filtered value of the left analog stick once the deadzone has been applied
        inline Float2 GetLeftAnalogStickValue() const { return m_analogInputFiltered[Left]; }

        // Get the filtered value of the right analog stick once the deadzone has been applied
        inline Float2 GetRightAnalogStickValue() const { return m_analogInputFiltered[Right]; }

        // Get the filtered value of the left trigger once the trigger thresholds has been applied
        inline float GetLeftTriggerValue() const { return m_triggerFiltered[Left]; }

        // Get the filtered value of the right trigger once the trigger thresholds has been applied
        inline float GetRightTriggerValue() const { return m_triggerFiltered[Right]; }

        // Get the raw unfiltered value of the left analog stick
        inline Float2 GetLeftAnalogStickRawValue() const { return m_analogInputRaw[Left]; }

        // Get the raw unfiltered value of the right analog stick
        inline Float2 GetRightAnalogStickRawValue() const { return m_analogInputRaw[Right]; }

        // Get the raw unfiltered value of the left trigger
        inline float GetLeftTriggerRawValue() const { return m_triggerRaw[Left]; }

        // Get the raw unfiltered value of the right trigger
        inline float GetRightTriggerRawValue() const { return m_triggerRaw[Right]; }

        //-------------------------------------------------------------------------

        // Was the button just pressed (i.e. went from up to down this frame)
        KRG_FORCE_INLINE bool WasPressed( ControllerButton buttonID ) const { return ButtonStates::WasPressed( (uint32) buttonID ); }

        // Was the button just release (i.e. went from down to up this frame). Also optionally returns how long the button was held for
        KRG_FORCE_INLINE bool WasReleased( ControllerButton buttonID, Seconds* pHeldDownDuration = nullptr ) const { return ButtonStates::WasReleased( (uint32) buttonID, pHeldDownDuration ); }

        // Is the button being held down?
        KRG_FORCE_INLINE bool IsHeldDown( ControllerButton buttonID, Seconds* pHeldDownDuration = nullptr ) const { return ButtonStates::IsHeldDown( (uint32) buttonID, pHeldDownDuration ); }

        // How long has the button been held down for?
        KRG_FORCE_INLINE Seconds GetHeldDuration( ControllerButton buttonID ) const { return ButtonStates::GetHeldDuration( (uint32) buttonID ); }

    private:

        KRG_FORCE_INLINE void Press( ControllerButton buttonID ) { ButtonStates::Press( (uint32) buttonID ); }
        KRG_FORCE_INLINE void Release( ControllerButton buttonID ) { ButtonStates::Release( (uint32) buttonID ); }

        inline void ClearState()
        {
            m_analogInputRaw[0] = m_analogInputRaw[1] = Float2::Zero;
            m_analogInputFiltered[0] = m_analogInputFiltered[1] = Float2::Zero;
            m_triggerRaw[0] = m_triggerRaw[1] = 0.0f;
            m_triggerFiltered[0] = m_triggerFiltered[1] = 0.0f;
            ClearButtonState();
        }

    private:

        Float2                                  m_analogInputRaw[2];
        Float2                                  m_analogInputFiltered[2];
        float                                     m_triggerRaw[2];
        float                                     m_triggerFiltered[2];
    };
}