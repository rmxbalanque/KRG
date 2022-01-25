#include "System/Input/InputSystem.h"
#include "InputDevices/InputDevice_XBoxController.h"

//-------------------------------------------------------------------------

namespace KRG::Input
{
    MouseInputState const InputSystem::s_emptyMouseState = MouseInputState();
    KeyboardInputState const InputSystem::s_emptyKeyboardState = KeyboardInputState();
    ControllerInputState const InputSystem::s_emptyControllerState = ControllerInputState();

    //-------------------------------------------------------------------------

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

    void InputSystem::Update( Seconds deltaTime )
    {
        for ( auto pInputDevice : m_inputDevices )
        {
            pInputDevice->UpdateState( deltaTime );
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

    uint32 InputSystem::GetNumConnectedControllers() const
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

    void InputSystem::ReflectState( Seconds const deltaTime, float timeScale, InputState& outReflectedState ) const
    {
        outReflectedState.m_mouseState.ReflectFrom( deltaTime, timeScale, *GetMouseState() );
        outReflectedState.m_keyboardState.ReflectFrom( deltaTime, timeScale, *GetKeyboardState() );

        int32 const numControllerStates = (int32) GetNumConnectedControllers();
        if ( outReflectedState.m_controllerStates.size() != numControllerStates )
        {
            outReflectedState.m_controllerStates.resize( numControllerStates );
        }

        for ( int32 i = 0; i < numControllerStates; i++ )
        {
            outReflectedState.m_controllerStates[i].ReflectFrom( deltaTime, timeScale, *GetControllerState( i ) );
        }
    }

    //-------------------------------------------------------------------------

    void InputState::Clear()
    {
        m_mouseState.Clear();
        m_keyboardState.Clear();

        for ( auto& controllerState : m_controllerStates )
        {
            controllerState.Clear();
        }
    }
}