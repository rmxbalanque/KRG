#ifdef _WIN32
#include "Resource.h"
#include "EditorApplication_win32.h"
#include "iniparser/krg_ini.h"
#include "Applications/Shared/cmdParser/krg_cmdparser.h"
#include "System/Input/InputSystem.h"
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

    EditorApplication::EditorApplication( HINSTANCE hInstance )
        : Win32Application( hInstance, "Kruger Editor", IDI_EDITOR_ICON )
        , m_editor( TFunction<bool( KRG::String const& error )>( [this] ( String const& error )-> bool  { return FatalError( error ); } ) )
    {}

    bool EditorApplication::ReadSettings( int32 argc, char** argv )
    {
        // Get command line settings
        //-------------------------------------------------------------------------
        {
            cli::Parser cmdParser( argc, argv );
            // TODO: REMOVE THIS - editor startup map needs to be handled differently
            cmdParser.set_optional<std::string>( "map", "map", "", "The startup map." );

            if ( !cmdParser.run() )
            {
                return FatalError( "Invalid command line arguments!" );
            }

            std::string const map = cmdParser.get<std::string>( "map" );
            if ( !map.empty() )
            {
                m_editor.m_startupMap = map.c_str();
            }
        }

        // Read configuration settings from ini
        //-------------------------------------------------------------------------

        {
            FileSystemPath const iniPath = FileSystem::GetCurrentProcessPath().Append( "KRG.ini" );
            if ( !m_editor.m_settingsRegistry.LoadFromFile( iniPath ) )
            {
                return FatalError( "Failed to read required settings from INI file" );
            }
        }

        return true;
    }

    bool EditorApplication::Initialize()
    {
        if ( !EnsureResourceServerIsRunning() )
        {
            return FatalError( "Couldn't start resource server!" );
        }

        Int2 const windowDimensions( ( m_windowRect.right - m_windowRect.left ), ( m_windowRect.bottom - m_windowRect.top ) );
        if ( !m_editor.Initialize( m_applicationNameNoWhitespace, windowDimensions ) )
        {
            return FatalError( "Failed to initialize engine" );
        }

        return true;
    }

    bool EditorApplication::Shutdown()
    {
        return m_editor.Shutdown();
    }

    bool EditorApplication::ApplicationLoop()
    {
        return m_editor.Update();
    }

    //-------------------------------------------------------------------------

    bool EditorApplication::EnsureResourceServerIsRunning()
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

    //-------------------------------------------------------------------------

    LRESULT EditorApplication::WndProcess( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
    {
        if ( !IsInitialized() )
        {
            return DefWindowProc( hWnd, message, wParam, lParam );
        }

        //-------------------------------------------------------------------------

        switch ( message )
        {
            case WM_SIZE:
            {
                KRG::uint32 width = LOWORD( lParam );
                KRG::uint32 height = HIWORD( lParam );
                if ( width > 0 && height > 0 )
                {
                    m_editor.UpdateMainWindowSize( Int2( width, height ) );
                }
            }
            break;

            //-------------------------------------------------------------------------

            case WM_SETFOCUS:
            {
                m_editor.m_pInputSystem->ForwardInputMessageToInputDevices( { message, (uintptr_t) wParam, (uintptr_t) lParam } );
            }
            break;

            case WM_KILLFOCUS:
            {
                m_editor.m_pInputSystem->ForwardInputMessageToInputDevices( { message, (uintptr_t) wParam, (uintptr_t) lParam } );
            }
            break;

            //-------------------------------------------------------------------------

            // Forward input messages to the input system
            case WM_INPUT:
            case WM_KEYDOWN:
            case WM_KEYUP:
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_CHAR:
            case WM_MOUSEMOVE:
            {
                m_editor.m_pInputSystem->ForwardInputMessageToInputDevices( { message, (uintptr_t) wParam, (uintptr_t) lParam } );
            }
            break;

            //-------------------------------------------------------------------------

            case WM_CLOSE:
            case WM_QUIT:
            {
                RequestExit();
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

        auto const imguiResult = m_editor.m_pImguiSystem->ProcessInput( { hWnd, message, wParam, lParam } );
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