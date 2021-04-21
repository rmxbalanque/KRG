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
#include <tchar.h>
#include <windows.h>

#if LIVEPP_ENABLED
#include "LPP_API.h"
#endif

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
        , m_editorHost( TFunction<bool( String const& error )>( [this] ( String const& error )-> bool  { return FatalError( error ); } ) )
    {
        // This is hard-coded since this is always the default map for the editor. There is no need for this to change.
        m_editorHost.m_startupMap = DataPath( "data://Editor/EditorMap.map" );
    }

    bool EditorApplication::ReadSettings( int32 argc, char** argv )
    {
        // Get command line settings
        //-------------------------------------------------------------------------

        cli::Parser cmdParser( argc, argv );

        cmdParser.set_optional<std::string>( "mode", "mode", "", "What mode should we start the editor in?" );

        if ( !cmdParser.run() )
        {
            return FatalError( "Invalid command line arguments!" );
        }

        StringID editorModeID;
        std::string const mode = cmdParser.get<std::string>( "mode" );
        if ( !mode.empty() )
        {
            editorModeID = StringID( mode.c_str() );
        }

        // Read configuration settings from ini
        //-------------------------------------------------------------------------

        FileSystem::Path const iniPath = FileSystem::GetCurrentProcessPath().Append( "KRG.ini" );
        if ( !m_editorHost.m_settingsRegistry.LoadFromFile( iniPath ) )
        {
            return FatalError( "Failed to read required settings from INI file" );
        }

        // Create editor
        //-------------------------------------------------------------------------

        auto pEditor = EditorToolkitRegistry::TryCreateEditor( editorModeID );
        if ( pEditor == nullptr )
        {
            return FatalError( String().sprintf( "Couldn't find editor mode: %s", editorModeID.c_str() ) );
        }

        m_editorHost.SetActiveEditor( pEditor );

        // Update application name so that we create the correct ini files
        const_cast<String&>( m_applicationName ) = pEditor->GetName();
        const_cast<String&>( m_applicationNameNoWhitespace ) = StringUtils::StripWhitespace( pEditor->GetName() );

        return true;
    }

    bool EditorApplication::Initialize()
    {
        if ( !EnsureResourceServerIsRunning() )
        {
            return FatalError( "Couldn't start resource server!" );
        }

        // Initialize editor
        //-------------------------------------------------------------------------

        Int2 const windowDimensions( ( m_windowRect.right - m_windowRect.left ), ( m_windowRect.bottom - m_windowRect.top ) );
        if ( !m_editorHost.Initialize( m_applicationNameNoWhitespace, windowDimensions ) )
        {
            return FatalError( "Failed to initialize engine" );
        }

        m_editorHost.SetUserFlags( m_userFlags );

        return true;
    }

    bool EditorApplication::Shutdown()
    {
        m_userFlags = m_editorHost.GetUserFlags();
        return m_editorHost.Shutdown();
    }

    bool EditorApplication::ApplicationLoop()
    {
        return m_editorHost.Update();
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
                uint32 width = LOWORD( lParam );
                uint32 height = HIWORD( lParam );
                if ( width > 0 && height > 0 )
                {
                    m_editorHost.UpdateMainWindowSize( Int2( width, height ) );
                }
            }
            break;

            //-------------------------------------------------------------------------

            case WM_SETFOCUS:
            {
                m_editorHost.m_pInputSystem->ForwardInputMessageToInputDevices( { message, (uintptr_t) wParam, (uintptr_t) lParam } );
            }
            break;

            case WM_KILLFOCUS:
            {
                m_editorHost.m_pInputSystem->ForwardInputMessageToInputDevices( { message, (uintptr_t) wParam, (uintptr_t) lParam } );
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
                m_editorHost.m_pInputSystem->ForwardInputMessageToInputDevices( { message, (uintptr_t) wParam, (uintptr_t) lParam } );
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

        auto const imguiResult = m_editorHost.m_pImguiSystem->ProcessInput( { hWnd, message, wParam, lParam } );
        if ( imguiResult != 0 )
        {
            return imguiResult;
        }

        // Default
        //-------------------------------------------------------------------------

        return DefWindowProc( hWnd, message, wParam, lParam );
    }
}

//-------------------------------------------------------------------------

int APIENTRY _tWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{
    //-------------------------------------------------------------------------
    // Live++ Support
    //-------------------------------------------------------------------------

    #if LIVEPP_ENABLED
    HMODULE livePP = lpp::lppLoadAndRegister( L"../../External/LivePP", "Quickstart" );
    lpp::lppEnableAllCallingModulesSync( livePP );
    #endif

    //-------------------------------------------------------------------------

    KRG::EditorApplication editorApplication( hInstance );
    return editorApplication.Run( __argc, __argv );
}

#endif