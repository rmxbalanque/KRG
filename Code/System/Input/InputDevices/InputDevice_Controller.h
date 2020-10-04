#pragma once

#include "InputDevice.h"
#include "System\Input\InputStates\InputState_Controller.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Input
    {
        // Base class for all the various controller types and APIs
        //-------------------------------------------------------------------------

        class ControllerInputDevice : public InputDevice
        {

        public:

            struct Settings
            {

            public:

                F32     m_leftStickInnerDeadzone = 0.0f;
                F32     m_leftStickOuterDeadzone = 0.0f;
                F32     m_leftTriggerThreshold = 0.0f;

                F32     m_rightStickInnerDeadzone = 0.0f;
                F32     m_rightStickOuterDeadzone = 0.0f;
                F32     m_rightTriggerThreshold = 0.0f;

                bool    m_leftStickInvertY = false;
                bool    m_rightStickInvertY = false;
            };

        public:

            ControllerInputDevice() = default;

            inline Settings const& GetSettings() const { return m_settings; }
            inline ControllerInputState const& GetControllerState() const { return m_controllerState; }

            inline bool IsConnected() const { return m_isConnected; }

        protected:

            // Update the controller button state
            inline void UpdateButtonState( ControllerButton buttonID, bool isDown )
            {
                // Do we need to generate a pressed signal?
                if ( isDown )
                {
                    if ( !m_controllerState.IsHeldDown( buttonID ) )
                    {
                        m_controllerState.Press( buttonID );
                    }
                }
                else // If the button is up, check if we need to generate a release signal
                {
                    if ( m_controllerState.IsHeldDown( buttonID ) )
                    {
                        m_controllerState.Release( buttonID );
                    }
                }
            }

            void SetTriggerValues( F32 leftRawValue, F32 rightRawValue );
            void SetAnalogStickValues( Float2 const& leftRawValue, Float2 const& rightRawValue );
            void ClearControllerState() { m_controllerState.ClearState(); }

        private:

            virtual DeviceCategory GetDeviceCategory() const override final { return DeviceCategory::Controller; }

        protected:

            Settings                                    m_settings;
            ControllerInputState                        m_controllerState;
            bool                                        m_isConnected = false;
        };
    }
}