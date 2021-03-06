#include "System/Input/InputSystem.h"
#include "InputDevices/InputDevice_XBoxController.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Input
    {
        bool InputSystem::Initialize()
        {
            // Create a keyboard and mouse device
            m_inputDevices.emplace_back( KRG::New<KeyboardMouseInputDevice>() );

            // Create a single controller for now
            //-------------------------------------------------------------------------
            // TODO: build a controller manager that can detect and automatically create controllers

            m_inputDevices.emplace_back( KRG::New<XBoxControllerInputDevice>( 0 ) );

            //-------------------------------------------------------------------------

            for ( auto pDevice : m_inputDevices )
            {
                pDevice->Initialize();
            }

            return true;
        }

        void InputSystem::Shutdown()
        {
            for ( auto& pInputDevice : m_inputDevices )
            {
                pInputDevice->Shutdown();
                KRG::Delete( pInputDevice );
            }

            m_inputDevices.clear();
        }

        void InputSystem::Update()
        {
            for ( auto pInputDevice : m_inputDevices )
            {
                pInputDevice->UpdateState();
            }
        }

        void InputSystem::ClearFrameState()
        {
            for ( auto pInputDevice : m_inputDevices )
            {
                pInputDevice->ClearFrameState();
            }
        }

        void InputSystem::ForwardInputMessageToInputDevices( GenericMessage const& inputMessage )
        {
            for ( auto pInputDevice : m_inputDevices )
            {
                pInputDevice->ProcessMessage( inputMessage );
            }
        }

        //-------------------------------------------------------------------------

        KeyboardMouseInputDevice const* InputSystem::GetKeyboardMouseDevice() const
        {
            for ( auto pDevice : m_inputDevices )
            {
                if ( pDevice->GetDeviceCategory() == DeviceCategory::KeyboardMouse )
                {
                    return static_cast<KeyboardMouseInputDevice*>( pDevice );
                }
            }

            return nullptr;
        }

        //-------------------------------------------------------------------------

        uint32 InputSystem::GetNumConnectedControllers()
        {
            uint32 numControllers = 0;
            for ( auto pDevice : m_inputDevices )
            {
                if ( pDevice->GetDeviceCategory() == DeviceCategory::Controller )
                {
                    auto pController = static_cast<ControllerInputDevice*>( pDevice );
                    if ( pController->IsConnected() )
                    {
                        numControllers++;
                    }
                }
            }

            return numControllers;
        }

        ControllerInputDevice const* InputSystem::GetControllerDevice( uint32 controllerIdx ) const
        {
            uint32 currentControllerIdx = 0;
            for ( auto pDevice : m_inputDevices )
            {
                if ( pDevice->GetDeviceCategory() == DeviceCategory::Controller )
                {
                    auto pController = static_cast<ControllerInputDevice*>( pDevice );
                    if ( pController->IsConnected() )
                    {
                        if ( controllerIdx == currentControllerIdx )
                        {
                            return static_cast<ControllerInputDevice*>( pDevice );
                        }
                        else
                        {
                            controllerIdx++;
                        }
                    }
                }
            }

            return nullptr;
        }
    }
}