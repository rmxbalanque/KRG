#include "StandaloneEditorApplication_Win32.h"
#include "Engine_Win32.h"
#include "Resource.h"
#include "iniparser/krg_ini.h"
#include "Applications/Shared/cmdParser/krg_cmdparser.h"
#include "Applications/Shared/Win32/SharedHelpers_Win32.h"
#include "System/Input/InputSystem.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Time/Timers.h"
#include <tchar.h>
#include <windows.h>

#if LIVEPP_ENABLED
#include "LPP_API.h"
#endif

//-------------------------------------------------------------------------

namespace KRG
{
    StandaloneEditorEngine::~StandaloneEditorEngine()
    {
        KRG::Delete( m_pActiveEditor );
    }

    //-------------------------------------------------------------------------

    StandaloneEditorApplication::StandaloneEditorApplication( HINSTANCE hInstance )
        : Win32Application( hInstance, "Kruger Editor", IDI_EDITOR_ICON )
        , m_editorEngine( TFunction<bool( String const& error )>( [this] ( String const& error )-> bool  { return FatalError( error ); } ) )
    {
        // This is hard-coded since this is always the default map for the editor. There is no need for this to change.
        m_editorEngine.m_startupMap = DataPath( "data://Editor/EditorMap.map" );
    }

    bool StandaloneEditorApplication::ReadSettings( int32 argc, char** argv )
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
        if ( !m_editorEngine.m_settingsRegistry.LoadFromFile( iniPath ) )
        {
            return FatalError( "Failed to read required settings from INI file" );
        }

        // Create editor
        //-------------------------------------------------------------------------

        m_editorEngine.m_pActiveEditor = StandaloneEditorFactory::TryCreateEditor( editorModeID );
        if ( m_editorEngine.m_pActiveEditor == nullptr )
        {
            return FatalError( String().sprintf( "Couldn't find editor mode: %s", editorModeID.c_str() ) );
        }

        // Update application name so that we create the correct ini files to save the imgui layout
        const_cast<String&>( m_applicationName ) = m_editorEngine.m_pActiveEditor->GetName();
        const_cast<String&>( m_applicationNameNoWhitespace ) = StringUtils::StripWhitespace( m_editorEngine.m_pActiveEditor->GetName() );

        return true;
    }

    bool StandaloneEditorApplication::Initialize()
    {
        if ( !EnsureResourceServerIsRunning( m_editorEngine.m_resourceSettings.m_resourceServerExecutablePath ) )
        {
            return FatalError( "Couldn't start resource server!" );
        }

        // Initialize editor
        //-------------------------------------------------------------------------

        Int2 const windowDimensions( ( m_windowRect.right - m_windowRect.left ), ( m_windowRect.bottom - m_windowRect.top ) );
        if ( !m_editorEngine.Initialize( m_applicationNameNoWhitespace, windowDimensions ) )
        {
            return FatalError( "Failed to initialize engine" );
        }

        m_editorEngine.SetUserFlags( m_userFlags );

        return true;
    }

    bool StandaloneEditorApplication::Shutdown()
    {
        m_userFlags = m_editorEngine.GetUserFlags();
        return m_editorEngine.Shutdown();
    }

    bool StandaloneEditorApplication::ApplicationLoop()
    {
        ImGuiX::Theme::ApplyTheme();

        return m_editorEngine.Update();
    }

    //-------------------------------------------------------------------------

    LRESULT StandaloneEditorApplication::WndProcess( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
    {
        return DefaultEngineWindowProcessor( &m_editorEngine, hWnd, message, wParam, lParam );
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

    KRG::ApplicationGlobalState globalState;
    KRG::StandaloneEditorApplication editorApplication( hInstance );
    return editorApplication.Run( __argc, __argv );
}