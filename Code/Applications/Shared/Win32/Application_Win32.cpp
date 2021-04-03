#ifdef _WIN32
#include "Application_Win32.h"
#include "iniparser/krg_ini.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG
{
    inline static RECT ReadWindowRectDimensionsFromIni( char const* pSettingsFile )
    {
        KRG_ASSERT( pSettingsFile != nullptr );

        RECT outRect = { 0, 0, 640, 480 };

        KRG::IniFile userIni( pSettingsFile );
        if ( !userIni.IsValid() )
        {
            return outRect;
        }

        KRG::int32 v = 0;
        if ( userIni.TryGetInt( "WindowSettings:Left", v ) )
        {
            outRect.left = v;
        }

        if ( userIni.TryGetInt( "WindowSettings:Right", v ) )
        {
            outRect.right = v;
        }

        if ( userIni.TryGetInt( "WindowSettings:Top", v ) )
        {
            outRect.top = v;
        }

        if ( userIni.TryGetInt( "WindowSettings:Bottom", v ) )
        {
            outRect.bottom = v;
        }

        return outRect;
    }

    inline static void WriteWindowRectDimensionsToIni( char const* pSettingsFile, RECT windowRect )
    {
        KRG_ASSERT( pSettingsFile != nullptr );

        KRG::IniFile userIni;
        if ( userIni.IsValid() )
        {
            userIni.CreateSection( "WindowSettings" );
            userIni.SetInt( "WindowSettings:Left", (KRG::int32) windowRect.left );
            userIni.SetInt( "WindowSettings:Right", (KRG::int32) windowRect.right );
            userIni.SetInt( "WindowSettings:Top", (KRG::int32) windowRect.top );
            userIni.SetInt( "WindowSettings:Bottom", (KRG::int32) windowRect.bottom );
            userIni.SaveToFile( KRG::FileSystem::Path( pSettingsFile ) );
        }
    }

    //-------------------------------------------------------------------------

    namespace
    {
        static Win32Application* g_pApplicationInstance = nullptr;

        static LRESULT CALLBACK wndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
        {
            if ( g_pApplicationInstance != nullptr )
            {
                return g_pApplicationInstance->WndProcess( hWnd, message, wParam, lParam );
            }

            return DefWindowProc( hWnd, message, wParam, lParam );
        }
    }

    //-------------------------------------------------------------------------

    Win32Application::Win32Application( HINSTANCE hInstance, char const* applicationName, int iconResourceID )
        : m_pInstance( hInstance )
        , m_applicationName( applicationName )
        , m_applicationNameNoWhitespace( StringUtils::StripWhitespace( applicationName ) )
    {
        KRG::Memory::MemsetZero( &m_message, sizeof( m_message ) );
        TryCreateWindow( iconResourceID );

        //-------------------------------------------------------------------------

        KRG_ASSERT( g_pApplicationInstance == nullptr );
        g_pApplicationInstance = this;
    }

    Win32Application::~Win32Application()
    {
        g_pApplicationInstance = nullptr;

        //-------------------------------------------------------------------------

        ::DestroyWindow( m_pWindow );
        ::UnregisterClass( m_windowClass.lpszClassName, m_windowClass.hInstance );
    }

    bool Win32Application::FatalError( String const& error )
    {
        MessageBox( GetActiveWindow(), error.c_str(), "Fatal Error Occurred!", MB_OK | MB_ICONERROR );
        return false;
    }

    void Win32Application::TryCreateWindow( int iconResourceID )
    {
        String const WindowSettingsIni = m_applicationNameNoWhitespace + ".windows.ini";
        RECT const desiredWindowRect = ReadWindowRectDimensionsFromIni( WindowSettingsIni.c_str() );

        //-------------------------------------------------------------------------

        m_windowClass.cbSize = sizeof( WNDCLASSEX );
        m_windowClass.style = CS_HREDRAW | CS_VREDRAW;
        m_windowClass.lpfnWndProc = wndProc;
        m_windowClass.cbClsExtra = 0;
        m_windowClass.cbWndExtra = 0;
        m_windowClass.hInstance = m_pInstance;
        m_windowClass.hIcon = LoadIcon( m_pInstance, MAKEINTRESOURCE( iconResourceID ) );
        m_windowClass.hCursor = nullptr;
        m_windowClass.hbrBackground = (HBRUSH) ( COLOR_WINDOW + 1 );
        m_windowClass.lpszMenuName = 0;
        m_windowClass.lpszClassName = m_applicationName.c_str();
        m_windowClass.hIconSm = 0;
        RegisterClassEx( &m_windowClass );

        //-------------------------------------------------------------------------

        auto const style = WS_OVERLAPPEDWINDOW;

        // Calculate the actual window size
        m_windowRect = { desiredWindowRect.left, desiredWindowRect.top, desiredWindowRect.right, desiredWindowRect.bottom };
        AdjustWindowRectEx( &m_windowRect, style, FALSE, 0 );

        long const leftOffset = desiredWindowRect.left - m_windowRect.left;
        long const topOffset = desiredWindowRect.top - m_windowRect.top;

        m_windowRect.left += leftOffset;
        m_windowRect.right += leftOffset;
        m_windowRect.top += topOffset;
        m_windowRect.bottom += topOffset;

        //-------------------------------------------------------------------------

        // Create the window from the class defined above
        m_pWindow = CreateWindow( m_windowClass.lpszClassName,
                                  m_windowClass.lpszClassName,
                                  style,
                                  m_windowRect.left,
                                  m_windowRect.top,
                                  m_windowRect.right - m_windowRect.left,
                                  m_windowRect.bottom - m_windowRect.top,
                                  NULL,
                                  NULL,
                                  m_pInstance,
                                  NULL );

        // If window creation was successful
        if ( m_pWindow != nullptr )
        {
            ShowWindow( m_pWindow, SW_SHOW );
            UpdateWindow( m_pWindow );
            GetClientRect( m_pWindow, &m_windowRect );
        }
    }

    void Win32Application::SetWindowTitle( char const* pNewTitle )
    {
        SetWindowText( m_pWindow, pNewTitle );
    }

    int Win32Application::Run( int32 argc, char** argv )
    {
        if ( m_pWindow == nullptr )
        {
            return -1;
        }

        // Read Settings
        //-------------------------------------------------------------------------

        if ( !ReadSettings( argc, argv ) )
        {
            return FatalError( "Application failed to read settings correctly!" );
        }

        // Initialization
        //-------------------------------------------------------------------------

        if ( !Initialize() )
        {
            Shutdown();
            return FatalError( "Application failed to initialize correctly!" );
        }
        else
        {
            m_initialized = true;
        }

        // Application loop
        //-------------------------------------------------------------------------

        while ( !m_exitRequested )
        {
            if ( PeekMessage( &m_message, NULL, 0, 0, PM_REMOVE ) == TRUE )
            {
                TranslateMessage( &m_message );
                DispatchMessage( &m_message );
            }
            else // Run the application update
            {
                m_exitRequested = !ApplicationLoop();
            }

            GetWindowRect( m_pWindow, &m_windowRect );
        }

        // Shutdown
        //-------------------------------------------------------------------------

        bool const shutdownResult = Shutdown();
        m_initialized = false;

        String const LogName = m_applicationNameNoWhitespace + "Log.txt";
        Log::SaveToFile( LogName );

        // Write Settings
        //-------------------------------------------------------------------------

        String const WindowSettingsIni = m_applicationNameNoWhitespace + ".windows.ini";
        WriteWindowRectDimensionsToIni( WindowSettingsIni.c_str(), m_windowRect );

        WriteSettings();

        //-------------------------------------------------------------------------

        if ( !shutdownResult )
        {
            return FatalError( "Application failed to shutdown correctly!" );
        }

        return (int) m_message.wParam;
    }
}
#endif