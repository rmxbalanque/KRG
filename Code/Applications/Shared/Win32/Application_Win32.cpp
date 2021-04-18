#ifdef _WIN32
#include "Application_Win32.h"
#include "iniparser/krg_ini.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG
{
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

    Win32Application::Win32Application( HINSTANCE hInstance, char const* applicationName, int32 iconResourceID )
        : m_pInstance( hInstance )
        , m_applicationName( applicationName )
        , m_applicationNameNoWhitespace( StringUtils::StripWhitespace( applicationName ) )
        , m_applicationIconResourceID( iconResourceID )
    {
        Memory::MemsetZero( &m_message, sizeof( m_message ) );

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

    //-------------------------------------------------------------------------

    void Win32Application::ReadLayoutSettings()
    {
        FileSystem::Path const layoutIniFilePath = FileSystem::Path( m_applicationNameNoWhitespace + ".layout.ini" );
        IniFile layoutIni( layoutIniFilePath );
        if ( !layoutIni.IsValid() )
        {
            return;
        }

        //-------------------------------------------------------------------------

        int32 v = 0;
        if ( layoutIni.TryGetInt( "WindowSettings:Left", v ) )
        {
            m_windowRect.left = v;
        }

        if ( layoutIni.TryGetInt( "WindowSettings:Right", v ) )
        {
            m_windowRect.right = v;
        }

        if ( layoutIni.TryGetInt( "WindowSettings:Top", v ) )
        {
            m_windowRect.top = v;
        }

        if ( layoutIni.TryGetInt( "WindowSettings:Bottom", v ) )
        {
            m_windowRect.bottom = v;
        }

        KRG_ASSERT( ( m_windowRect.right - m_windowRect.left ) > 0 );
        KRG_ASSERT( ( m_windowRect.bottom - m_windowRect.top ) > 0 );

        //-------------------------------------------------------------------------

        uint32 flags = 0;

        layoutIni.TryGetUInt( "Layout:UserFlags0", flags );
        m_userFlags = flags;

        layoutIni.TryGetUInt( "Layout:UserFlags1", flags );
        m_userFlags |= uint64( flags ) << 32;
    }

    void Win32Application::WriteLayoutSettings()
    {
        IniFile layoutIni;
        if ( layoutIni.IsValid() )
        {
            // Save window rect
            layoutIni.CreateSection( "WindowSettings" );
            layoutIni.SetInt( "WindowSettings:Left", (int32) m_windowRect.left );
            layoutIni.SetInt( "WindowSettings:Right", (int32) m_windowRect.right );
            layoutIni.SetInt( "WindowSettings:Top", (int32) m_windowRect.top );
            layoutIni.SetInt( "WindowSettings:Bottom", (int32) m_windowRect.bottom );

            // Save user flags
            layoutIni.CreateSection( "Layout" );
            layoutIni.SetInt( "Layout:UserFlags0", (uint32) m_userFlags );
            layoutIni.SetInt( "Layout:UserFlags1", (uint32) ( m_userFlags >> 32 ) );

            FileSystem::Path const layoutIniFilePath = FileSystem::Path( m_applicationNameNoWhitespace + ".layout.ini" );
            layoutIni.SaveToFile( layoutIniFilePath );
        }
    }

    //-------------------------------------------------------------------------

    bool Win32Application::TryCreateWindow()
    {
        KRG_ASSERT( ( m_windowRect.right - m_windowRect.left ) > 0 );
        KRG_ASSERT( ( m_windowRect.bottom - m_windowRect.top ) > 0 );

        // Create the window
        //-------------------------------------------------------------------------

        m_windowClass.cbSize = sizeof( WNDCLASSEX );
        m_windowClass.style = CS_HREDRAW | CS_VREDRAW;
        m_windowClass.lpfnWndProc = wndProc;
        m_windowClass.cbClsExtra = 0;
        m_windowClass.cbWndExtra = 0;
        m_windowClass.hInstance = m_pInstance;
        m_windowClass.hIcon = LoadIcon( m_pInstance, MAKEINTRESOURCE( m_applicationIconResourceID ) );
        m_windowClass.hCursor = nullptr;
        m_windowClass.hbrBackground = (HBRUSH) ( COLOR_WINDOW + 1 );
        m_windowClass.lpszMenuName = 0;
        m_windowClass.lpszClassName = m_applicationName.c_str();
        m_windowClass.hIconSm = 0;
        RegisterClassEx( &m_windowClass );

        long const windowDesiredWidth = Math::Max( m_windowRect.right - m_windowRect.left, 100l );
        long const windowDesiredHeight = Math::Max( m_windowRect.bottom - m_windowRect.top, 40l );

        m_pWindow = CreateWindow( m_windowClass.lpszClassName,
                                  m_windowClass.lpszClassName,
                                  WS_OVERLAPPEDWINDOW,
                                  m_windowRect.left,
                                  m_windowRect.top,
                                  windowDesiredWidth,
                                  windowDesiredHeight,
                                  NULL,
                                  NULL,
                                  m_pInstance,
                                  NULL );

        if ( m_pWindow == nullptr )
        {
            return false;
        }

        //-------------------------------------------------------------------------

        ShowWindow( m_pWindow, SW_SHOW );
        UpdateWindow( m_pWindow );
        GetClientRect( m_pWindow, &m_windowRect );
        return true;
    }

    //-------------------------------------------------------------------------

    int Win32Application::Run( int32 argc, char** argv )
    {
        // Read Settings
        //-------------------------------------------------------------------------

        if ( !ReadSettings( argc, argv ) )
        {
            return FatalError( "Application failed to read settings correctly!" );
        }

        ReadLayoutSettings();

        // Window
        //-------------------------------------------------------------------------

        if ( !TryCreateWindow() )
        {
            return FatalError( "Application failed to create window!" );
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

        FileSystem::Path const LogFilePath( m_applicationNameNoWhitespace + "Log.txt" );
        Log::SaveToFile( LogFilePath );

        // Write Settings
        //-------------------------------------------------------------------------

        WriteLayoutSettings();

        //-------------------------------------------------------------------------

        if ( !shutdownResult )
        {
            return FatalError( "Application failed to shutdown correctly!" );
        }

        return (int) m_message.wParam;
    }
}
#endif