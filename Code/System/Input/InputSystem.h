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

            #if KRG_DEVELOPMENT_TOOLS
            void SetEnabled( bool enabled );
            #endif

            // Keyboard & Mouse
            //-------------------------------------------------------------------------

            inline bool HasConnectedKeyboardAndMouse() { return GetKeyboardMouseDevice() != nullptr; }

            inline MouseInputState const* GetMouseState() const 
            {
                auto pDevice = GetKeyboardMouseDevice();
                KRG_ASSERT( pDevice != nullptr );
                return &pDevice->GetMouseState(); 
            }

            inline KeyboardInputState const* GetKeyboardState() const
            {
                auto pDevice = GetKeyboardMouseDevice();
                KRG_ASSERT( pDevice != nullptr );
                return &pDevice->GetKeyboardState();
            }

            // Controllers
            //-------------------------------------------------------------------------

            uint32 GetNumConnectedControllers();

            inline ControllerInputState const* GetControllerState( uint32 controllerIdx = 0 ) const
            {
                auto pDevice = GetControllerDevice( controllerIdx );
                KRG_ASSERT( pDevice != nullptr );
                return &pDevice->GetControllerState();
            }

        private:

            KeyboardMouseInputDevice const* GetKeyboardMouseDevice() const;
            ControllerInputDevice const* GetControllerDevice( uint32 controllerIdx = 0 ) const;

        private:

            TVector<InputDevice*>   m_inputDevices;

            #if KRG_DEVELOPMENT_TOOLS
            bool                    m_disableInput = false;
            #endif
        };
    }
}