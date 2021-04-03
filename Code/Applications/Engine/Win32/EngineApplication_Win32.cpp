#ifdef _WIN32
#include "Resource.h"
#include "EngineApplication_win32.h"
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

    EngineApplication::EngineApplication( HINSTANCE hInstance )
        : Win32Application( hInstance, "Kruger Engine", IDI_ENGINE_ICON )
        , m_engine( TFunction<bool( KRG::String const& error )>( [this] ( String const& error )-> bool  { return FatalError( error ); } ) )
    {}

    bool EngineApplication::ReadSettings( int32 argc, char** argv )
    {
        // Get command line settings
        //-------------------------------------------------------------------------

        {
            cli::Parser cmdParser( argc, argv );
            cmdParser.set_optional<std::string>( "map", "map", "", "The startup map." );

            if ( !cmdParser.run() )
            {
                return FatalError( "Invalid command line arguments!" );
            }

            std::string const map = cmdParser.get<std::string>( "map" );
            if ( !map.empty() )
            {
                m_engine.m_startupMap = map.c_str();
            }
        }

        // Read configuration settings from ini
        //-------------------------------------------------------------------------

        {
            FileSystem::Path const iniPath = FileSystem::GetCurrentProcessPath().Append( "KRG.ini" );
            if ( !m_engine.m_settingsRegistry.LoadFromFile( iniPath ) )
            {
                return FatalError( "Failed to read required settings from INI file" );
            }
        }

        return true;
    }

    bool EngineApplication::Initialize()
    {
        if ( !EnsureResourceServerIsRunning() )
        {
            return FatalError( "Couldn't start resource server!" );
        }

        Int2 const windowDimensions( ( m_windowRect.right - m_windowRect.left ), ( m_windowRect.bottom - m_windowRect.top ) );
        if ( !m_engine.Initialize( m_applicationNameNoWhitespace, windowDimensions ) )
        {
            return FatalError( "Failed to initialize engine" );
        }

        return true;
    }

    bool EngineApplication::Shutdown()
    {
        return m_engine.Shutdown();
    }

    bool EngineApplication::ApplicationLoop()
    {
        return m_engine.Update();
    }

    //-------------------------------------------------------------------------

    bool EngineApplication::EnsureResourceServerIsRunning()
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
                FileSystem::Path const resourceServerProcessPath = FileSystem::Path( resourceServerPath ).GetParentDirectory();
                FileSystem::Path const applicationDirectoryPath = FileSystem::Path( Platform::Win32::GetCurrentModulePath() ).GetParentDirectory();

                if ( resourceServerProcessPath != applicationDirectoryPath )
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
            FileSystem::Path const applicationDirectoryPath = FileSystem::Path( Platform::Win32::GetCurrentModulePath() ).GetParentDirectory();
            FileSystem::Path const resourceServerExecutableFullPath = applicationDirectoryPath + Settings::g_resourceServerExecutablePath;
            return Platform::Win32::StartProcess( resourceServerExecutableFullPath.c_str() ) != 0;
        }

        return true;
    }

    //-------------------------------------------------------------------------

    LRESULT EngineApplication::WndProcess( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
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
                    m_engine.UpdateMainWindowSize( Int2( width, height ) );
                }
            }
            break;

            //-------------------------------------------------------------------------

            case WM_SETFOCUS:
            {
                m_engine.m_pInputSystem->ForwardInputMessageToInputDevices( { message, (uintptr_t) wParam, (uintptr_t) lParam } );
            }
            break;

            case WM_KILLFOCUS:
            {
                m_engine.m_pInputSystem->ForwardInputMessageToInputDevices( { message, (uintptr_t) wParam, (uintptr_t) lParam } );
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
                m_engine.m_pInputSystem->ForwardInputMessageToInputDevices( { message, (uintptr_t) wParam, (uintptr_t) lParam } );
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

        auto const imguiResult = m_engine.m_pImguiSystem->ProcessInput( { hWnd, message, wParam, lParam } );
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