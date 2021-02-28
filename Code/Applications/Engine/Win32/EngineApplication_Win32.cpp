#ifdef _WIN32
#include "EngineApplication_win32.h"
#include "iniparser/krg_ini.h"
#include "System/Input/InputSystem.h"
#include "System/Render/RenderDevice/RenderDeviceSettings.h"
#include "System/Core/Settings/ConfigSettings.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Platform/Platform_Win32.h"
#include "System/Core/Time/Timers.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Settings
    {
        static ConfigSettingString     g_resourceServerExecutablePath( "ResourceServerExecutablePath", "Resource", "" );
    }

    //-------------------------------------------------------------------------

    EngineApplication_Win32* g_applicationInstance = nullptr;

    //-------------------------------------------------------------------------

    static RECT ReadWindowRectDimensionsFromIni()
    {
        RECT outRect = { 0, 0, 640, 480 };

        KRG::IniFile userIni( "User.ini" );
        if ( !userIni.IsValid() )
        {
            return outRect;
        }

        KRG::S32 v = 0;
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

    static void WriteWindowRectDimensionsToIni( RECT windowRect )
    {
        KRG::IniFile userIni;
        if ( userIni.IsValid() )
        {
            userIni.CreateSection( "WindowSettings" );
            userIni.SetInt( "WindowSettings:Left", ( KRG::S32 ) windowRect.left );
            userIni.SetInt( "WindowSettings:Right", ( KRG::S32 ) windowRect.right );
            userIni.SetInt( "WindowSettings:Top", ( KRG::S32 ) windowRect.top );
            userIni.SetInt( "WindowSettings:Bottom", ( KRG::S32 ) windowRect.bottom );
            userIni.SaveToFile( KRG::FileSystemPath( "User.ini" ) );
        }
    }

    static LRESULT CALLBACK wndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
    {
        return g_applicationInstance->WndProcess( hWnd, message, wParam, lParam );
    }

    //-------------------------------------------------------------------------
    // ENGINE
    //-------------------------------------------------------------------------

    EngineApplication_Win32::EngineApplication_Win32( HINSTANCE hInstance, char const* applicationName, int iconResourceID )
        : EngineApplication()
    {
        KRG_ASSERT( g_applicationInstance == nullptr );
        g_applicationInstance = this;

        KRG::Memory::MemsetZero( &m_message, sizeof( m_message ) );
        TryCreateWindow( hInstance, applicationName, iconResourceID );
    }

    EngineApplication_Win32::~EngineApplication_Win32()
    {
        ::DestroyWindow( m_windowHandle );
        ::UnregisterClass( m_windowClass.lpszClassName, m_windowClass.hInstance );
    }

    bool EngineApplication_Win32::StartDependencies()
    {
        if ( !EnsureResourceServerIsRunning() )
        {
            return FatalError( "Couldn't start resource server!" );
        }

        return true;
    }

    bool EngineApplication_Win32::Initialize()
    {
        // Update window position
        auto pWindow = GetActiveWindow();
        RECT windowRect = { 0, 0, Render::Settings::g_resolutionX, Render::Settings::g_resolutionY };
        AdjustWindowRectEx( &windowRect, WS_OVERLAPPEDWINDOW, FALSE, 0 );
        SetWindowPos( pWindow, 0, 0, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE );

        //-------------------------------------------------------------------------

        if ( !StartDependencies() )
        {
            return FatalError( "Failed to start engine dependencies" );
        }

        if ( !EngineApplication::Initialize() )
        {
            return FatalError( "Failed to initialize engine" );
        }

        return true;
    }

    int EngineApplication_Win32::Run( S32 argc, char** argv )
    {
        if ( m_windowHandle == nullptr )
        {
            return -1;
        }

        // Read Settings
        //-------------------------------------------------------------------------

        if ( !ReadSettings( argc, argv ) )
        {
            return FatalError( "Failed to load engine settings" );
        }

        // Initialization
        //-------------------------------------------------------------------------

        if ( !Initialize() )
        {
            Shutdown();
            return -1;
        }

        LoadStartupMap();

        // Application loop
        //-------------------------------------------------------------------------

        bool updateResult = true;
        while ( updateResult )
        {
            if ( PeekMessage( &m_message, NULL, 0, 0, PM_REMOVE ) == TRUE )
            {
                TranslateMessage( &m_message );
                DispatchMessage( &m_message );

                // Handle quit messages
                if ( m_message.message == WM_QUIT )
                {
                    RequestExit();
                }
            }
            else // Run the application update
            {
                updateResult = Update();
            }

            GetWindowRect( m_windowHandle, &m_windowRect );
        }

        // Shutdown engine
        //-------------------------------------------------------------------------

        WriteWindowRectDimensionsToIni( m_windowRect );

        auto shutdownResult = Shutdown();
        KRG::Log::SaveToFile( "EngineLog.txt" );
        if ( !shutdownResult )
        {
            return -1;
        }

        //-------------------------------------------------------------------------

        WriteSettings();

        //-------------------------------------------------------------------------

        return (int) m_message.wParam;
    }

    bool EngineApplication_Win32::EnsureResourceServerIsRunning()
    {
        bool shouldStartResourceServer = false;

        // If the resource server is not running then start it
        auto resourceServerProcessID = Platform::Win32::GetProcessID( Settings::g_resourceServerExecutablePath );
        shouldStartResourceServer = ( resourceServerProcessID == 0 );
        
        // Ensure we are running the correct build of the resource server
        if ( !shouldStartResourceServer )
        {
            String const resourceServerPath = Platform::Win32::GetProcessPath( resourceServerProcessID );
            if ( !resourceServerPath.empty() )
            {
                FileSystemPath const resourceServerProcessPath = FileSystemPath( resourceServerPath ).GetParentDirectory();
                FileSystemPath const applicationFolderPath = FileSystemPath( Platform::Win32::GetCurrentModulePath() ).GetParentDirectory();

                if ( resourceServerProcessPath != applicationFolderPath )
                {
                    Platform::Win32::KillProcess( resourceServerProcessID );
                    shouldStartResourceServer = true;
                }
            }
            else
            {
                return FatalError( "Cant read the resource server EXE path from the currently running process!" );
            }
        }

        // Try to start the resource server
        if ( shouldStartResourceServer )
        {
            FileSystemPath const applicationFolderPath = FileSystemPath( Platform::Win32::GetCurrentModulePath() ).GetParentDirectory();
            FileSystemPath const resourceServerExecutableFullPath = applicationFolderPath + Settings::g_resourceServerExecutablePath;
            return Platform::Win32::StartProcess( resourceServerExecutableFullPath.c_str() ) != 0;
        }

        return true;
    }

    bool EngineApplication_Win32::FatalError( String const& error )
    {
        MessageBox( GetActiveWindow(), error.c_str(), "Fatal Error Occurred!", MB_OK | MB_ICONERROR );
        return false;
    }

    //-------------------------------------------------------------------------

    void EngineApplication_Win32::TryCreateWindow( HINSTANCE hInstance, char const* windowName, int iconResourceID )
    {
        RECT desiredWindowRect = ReadWindowRectDimensionsFromIni();

        //-------------------------------------------------------------------------

        m_windowClass.cbSize = sizeof( WNDCLASSEX );
        m_windowClass.style = CS_HREDRAW | CS_VREDRAW;
        m_windowClass.lpfnWndProc = wndProc;
        m_windowClass.cbClsExtra = 0;
        m_windowClass.cbWndExtra = 0;
        m_windowClass.hInstance = hInstance;
        m_windowClass.hIcon = LoadIcon( hInstance, MAKEINTRESOURCE( iconResourceID ) );
        m_windowClass.hCursor = nullptr;
        m_windowClass.hbrBackground = (HBRUSH) ( COLOR_WINDOW + 1 );
        m_windowClass.lpszMenuName = 0;
        m_windowClass.lpszClassName = windowName;
        m_windowClass.hIconSm = 0;
        RegisterClassEx( &m_windowClass );

        //-------------------------------------------------------------------------

        auto const style = WS_OVERLAPPEDWINDOW;

        // calculate the actual window size
        RECT windowRect = { desiredWindowRect.left, desiredWindowRect.top, desiredWindowRect.right, desiredWindowRect.bottom };
        AdjustWindowRectEx( &windowRect, style, FALSE, 0 );

        long const leftOffset = desiredWindowRect.left - windowRect.left;
        long const topOffset = desiredWindowRect.top - windowRect.top;

        windowRect.left += leftOffset;
        windowRect.right += leftOffset;
        windowRect.top += topOffset;
        windowRect.bottom += topOffset;

        //-------------------------------------------------------------------------

        // Create the window from the class defined above
        m_windowHandle = CreateWindow( m_windowClass.lpszClassName,
                                       m_windowClass.lpszClassName,
                                       style,
                                       windowRect.left,
                                       windowRect.top,
                                       windowRect.right - windowRect.left,
                                       windowRect.bottom - windowRect.top,
                                       NULL,
                                       NULL,
                                       hInstance,
                                       NULL );

        // If window creation was successful
        if ( m_windowHandle != nullptr )
        {
            ShowWindow( m_windowHandle, SW_SHOW );
            UpdateWindow( m_windowHandle );
        }
    }

    //-------------------------------------------------------------------------

    LRESULT EngineApplication_Win32::WndProcess( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
    {
        if ( !m_initialized )
        {
            return DefWindowProc( hWnd, message, wParam, lParam );
        }

        //-------------------------------------------------------------------------

        switch ( message )
        {
            case WM_SIZE:
            {
                KRG::U32 width = LOWORD( lParam );
                KRG::U32 height = HIWORD( lParam );
                if ( width > 0 && height > 0 )
                {
                    ResizeMainWindow( Int2( width, height ) );
                }
            }
            break;

            //-------------------------------------------------------------------------

            case WM_SETFOCUS:
            {
                m_pInputSystem->ForwardInputMessageToInputDevices( { message, (uintptr_t) wParam, (uintptr_t) lParam } );
            }
            break;

            case WM_KILLFOCUS:
            {
                m_pInputSystem->ForwardInputMessageToInputDevices( { message, (uintptr_t) wParam, (uintptr_t) lParam } );
            }
            break;

            //-------------------------------------------------------------------------

            // Needed to restore focus to the engine within the editor
            case WM_MBUTTONDOWN:
            case WM_RBUTTONDOWN:
            case WM_LBUTTONDOWN:
            {
                SetFocus( hWnd );
            }

            // Forward input messages to the input system
            case WM_INPUT:
            case WM_KEYDOWN:
            case WM_KEYUP:
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_CHAR:
            case WM_MOUSEMOVE:
            {
                m_pInputSystem->ForwardInputMessageToInputDevices( { message, (uintptr_t) wParam, (uintptr_t) lParam } );
            }
            break;

            //-------------------------------------------------------------------------

            case WM_DISPLAYCHANGE:
            {
            }
            break;

            //-------------------------------------------------------------------------

            case WM_DESTROY:
            {
                PostQuitMessage( 0 );
            }
            break;
        }

        // ImGui specific message processing
        //-------------------------------------------------------------------------

        auto const imguiResult = m_pImguiSystem->ImguiWndProcess( hWnd, message, wParam, lParam );
        if ( imguiResult != 0 )
        {
            return imguiResult;
        }

        // Default
        //-------------------------------------------------------------------------

        return DefWindowProc( hWnd, message, wParam, lParam );
    }
}

#endif