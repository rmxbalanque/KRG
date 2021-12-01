#pragma once

#include "_Module/API.h"
#include "InputDevices/InputDevice_KeyboardMouse.h"
#include "InputDevices/InputDevice_Controller.h"
#include "System/Core/Types/Containers.h"
#include "System/Core/Systems/ISystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Input
    {
        class KRG_SYSTEM_INPUT_API InputSystem : public ISystem
        {
            friend class InputDebugView;

        public:

            KRG_SYSTEM_ID( InputSystem );

        public:

            bool Initialize();
            void Shutdown();
            void Update();
            void ClearFrameState();
            void ForwardInputMessageToInputDevices( GenericMessage const& inputMessage );

            // Keyboard & Mouse
            //-------------------------------------------------------------------------

            inline bool HasConnectedKeyboardAndMouse() { return GetKeyboardMouseDevice() != nullptr; }

            inline MouseInputState const* GetMouseState() const
            {
                if ( auto pDevice = GetKeyboardMouseDevice() )
                {
                    return &pDevice->GetMouseState();
                }

                return &m_emptyMouseState;
            }

            inline KeyboardInputState const* GetKeyboardState() const
            {
                if ( auto pDevice = GetKeyboardMouseDevice() )
                {
                    return &pDevice->GetKeyboardState();
                }

                return &m_emptyKeyboardState;
            }

            // Controllers
            //-------------------------------------------------------------------------

            uint32 GetNumConnectedControllers();

            inline ControllerInputState const* GetControllerState( uint32 controllerIdx = 0 ) const
            {
                if ( auto pDevice = GetControllerDevice( controllerIdx ) )
                {
                    return &pDevice->GetControllerState();
                }

                return &m_emptyControllerState;
            }

        private:

            KeyboardMouseInputDevice const* GetKeyboardMouseDevice() const;
            ControllerInputDevice const* GetControllerDevice( uint32 controllerIdx = 0 ) const;

        private:

            TVector<InputDevice*>       m_inputDevices;
            MouseInputState             m_emptyMouseState;
            KeyboardInputState          m_emptyKeyboardState;
            ControllerInputState        m_emptyControllerState;
        };
    }
}