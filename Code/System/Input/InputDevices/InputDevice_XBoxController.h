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

            XBoxControllerInputDevice( U32 hardwareControllerIdx ) : m_hardwareControllerIdx( hardwareControllerIdx ) {}

        private:

            virtual void Initialize() override final;
            virtual void Shutdown() override final;
            virtual void UpdateState() override final;

        private:

            U32 m_hardwareControllerIdx = 0xFFFFFFFF;
        };
    }
}