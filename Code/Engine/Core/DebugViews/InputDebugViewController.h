#pragma once

#include "Engine/Core/DebugTools/DebugView.h"
#include "System/Input/InputDevices/InputDevice_Controller.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG
{
    namespace Input
    {
        class ControllerInputDevice;

        //-------------------------------------------------------------------------

        class InputDebugViewController : public Debug::DebugView
        {

        public:

            InputDebugViewController();

        private:

            virtual void DrawWindows( UpdateContext const& context ) override final;

            void DrawControllerState( ControllerInputDevice const& controllerState );

        private:

            TVector<ControllerInputDevice const*>          m_openControllerWindows;
        };
    }
}
#endif