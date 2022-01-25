#pragma once

#include "InputDevice_Controller.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Input
    {
        class XBoxControllerInputDevice : public ControllerInputDevice
        {
            friend class InputSystem;

        public:

            XBoxControllerInputDevice( uint32 hardwareControllerIdx ) : m_hardwareControllerIdx( hardwareControllerIdx ) {}

        private:

            virtual void Initialize() override final;
            virtual void Shutdown() override final;
            virtual void UpdateState( Seconds deltaTime ) override final;

        private:

            uint32 m_hardwareControllerIdx = 0xFFFFFFFF;
        };
    }
}