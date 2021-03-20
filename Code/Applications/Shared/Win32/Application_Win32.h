#ifdef _WIN32
#pragma once

#include "System/Core/Core/IntegralTypes.h"
#include "System/Core/Types/String.h"
#include "ApplicationGlobalState.h"
#include <windows.h>

//-------------------------------------------------------------------------

namespace KRG
{
    class Win32Application
    {
    public:

        Win32Application( HINSTANCE hInstance, char const* applicationName, int iconResourceID );
        virtual ~Win32Application();

        int Run( int32 argc, char** argv );

        // Win32 Window process
        virtual LRESULT WndProcess( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) = 0;

    protected:

        inline bool IsInitialized() const { return m_initialized; }
        inline void RequestExit() { m_exitRequested = true; }
        bool FatalError( String const& error );

        // Win32 window functions
        void TryCreateWindow( int iconResourceID );

        // This function allows the application to read all commandline settings and load all ini settings. Will be called before initialize.
        virtual bool ReadSettings( int32 argc, char** argv ) { return true; };
        
        // This function allows the application to save any settings it needs
        virtual void WriteSettings() {};

        // Initialize/Shutdown
        virtual bool Initialize() = 0;
        virtual bool Shutdown() = 0;

        // The actual application loop
        virtual bool ApplicationLoop() = 0;

    protected:

        String const                    m_applicationName;
        String const                    m_applicationNameNoWhitespace;
        ApplicationGlobalState          m_applicationGlobalState;
        WNDCLASSEX                      m_windowClass;
        HINSTANCE                       m_pInstance = nullptr;
        HWND                            m_pWindow = nullptr;
        RECT                            m_windowRect = { 0, 0, 640, 480 };
        MSG                             m_message;

    private:

        bool                            m_initialized = false;
        bool                            m_exitRequested = false;
    };
}

#endif