#pragma once

#include "_Module/API.h"
#include "InputDevices/InputDevice_KeyboardMouse.h"
#include "InputDevices/InputDevice_Controller.h"
#include "System/Core/Types/Containers.h"
#include "System/Core/Systems/ISystem.h"

//-------------------------------------------------------------------------

namespace KRG::Input
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

    public:

        MouseInputState const       m_emptyMouseState;
        KeyboardInputState const    m_emptyKeyboardState;
        ControllerInputState const  m_emptyControllerState;

    private:

        TVector<InputDevice*>       m_inputDevices;
    };

    //-------------------------------------------------------------------------
    // Engine Input State
    //-------------------------------------------------------------------------
    // Helper Wrapper allowing for conditional disabling of input in client code

    class InputState
    {
    public:

        InputState( InputSystem const* pInputSystem )
            : m_pInputSystem( pInputSystem )
        {
            KRG_ASSERT( pInputSystem != nullptr );
        }

        bool SetInputEnabled( bool isEnabled ) { m_isEnabled = isEnabled; }
        bool IsInputDisabled() const { return m_isEnabled; }

        KRG_FORCE_INLINE MouseInputState const* GetMouseState() const
        {
            return m_isEnabled ? m_pInputSystem->GetMouseState() : &m_pInputSystem->m_emptyMouseState;
        }

        KRG_FORCE_INLINE KeyboardInputState const* GetKeyboardState() const
        {
            return m_isEnabled ? m_pInputSystem->GetKeyboardState() : &m_pInputSystem->m_emptyKeyboardState;
        }

        KRG_FORCE_INLINE ControllerInputState const* GetControllerState( uint32 controllerIdx = 0 ) const
        {
            return m_isEnabled ? m_pInputSystem->GetControllerState( controllerIdx ) : &m_pInputSystem->m_emptyControllerState;
        }

    private:

        InputSystem const*      m_pInputSystem = nullptr;
        bool                    m_isEnabled = true;
    };
}