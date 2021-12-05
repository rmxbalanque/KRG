#ifdef _WIN32
#pragma once

#include "System/Core/Types/IntegralTypes.h"
#include "System/Core/Types/String.h"
#include "ApplicationGlobalState.h"
#include "System/Core/Math/Math.h"
#include <windows.h>

//-------------------------------------------------------------------------

namespace KRG
{
    class Win32Application
    {
    public:

        Win32Application( HINSTANCE hInstance, char const* applicationName, int32 iconResourceID );
        virtual ~Win32Application();

        int Run( int32 argc, char** argv );

        inline bool IsInitialized() const { return m_initialized; }
        inline void RequestExit() { m_exitRequested = true; }

        // Win32 Window process
        virtual LRESULT WndProcess( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) = 0;

        // Called just before destroying the window
        void OnWindowDestruction();

    protected:

        bool FatalError( String const& error );

        // Window creation
        bool TryCreateWindow();

        // This function allows the application to read all commandline settings and load all ini settings. Will be called before initialize.
        virtual bool ReadSettings( int32 argc, char** argv ) = 0;

        // These function allows the application to read/write any layout/positioning specific settings it needs
        virtual void WriteLayoutSettings();
        virtual void ReadLayoutSettings();

        // Initialize/Shutdown
        virtual bool Initialize() = 0;
        virtual bool Shutdown() = 0;

        // The actual application loop
        virtual bool ApplicationLoop() = 0;

    protected:

        String const                    m_applicationName;
        String const                    m_applicationNameNoWhitespace;
        int32                           m_applicationIconResourceID = -1;
        WNDCLASSEX                      m_windowClass;
        HINSTANCE                       m_pInstance = nullptr;
        HWND                            m_windowHandle = nullptr;
        RECT                            m_windowRect = { 0, 0, 640, 480 };
        MSG                             m_message;

        // Custom flags that user applications can set to specify what modes were enabled or what windows were open (saved in the layout.ini)
        uint64                          m_userFlags = 0; 

    private:

        bool                            m_startMaximized = false; // Read from the layout settings
        bool                            m_initialized = false;
        bool                            m_exitRequested = false;
    };
}

#endif