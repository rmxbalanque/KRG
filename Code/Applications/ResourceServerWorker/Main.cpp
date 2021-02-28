#ifdef _WIN32

#include "Applications/Shared/ApplicationGlobalState.h"
#include "Applications/Shared/cmdParser/krg_cmdparser.h"
#include "ResourceServerWorker.h"
#include "System/Core/Settings/SettingsRegistry.h"
#include "System/Core/FileSystem/FileSystem.h"

#include <windows.h>
#include <iostream>

//-------------------------------------------------------------------------

using namespace KRG;

//-------------------------------------------------------------------------

struct ArgumentParser
{
    ArgumentParser( int argc, char* argv[] )
    {
        cli::Parser cmdParser( argc, argv );
        cmdParser.set_default<bool>( false );
        cmdParser.set_optional<std::string>( "compile", "compile", "", "Compile resource" );
        cmdParser.set_optional<bool>( "debug", "debug", false, "Trigger debug break before execution." );

        if ( cmdParser.run() )
        {
            m_triggerDebugBreak = cmdParser.get<bool>( "debug" );

            // Get compile argument
            DataPath const resourceDataPath( cmdParser.get<std::string>( "compile" ).c_str() );
            if ( resourceDataPath.IsValid() )
            {
                m_resourceID = ResourceID( resourceDataPath );

                if ( m_resourceID.IsValid() )
                {
                    m_isValid = true;
                }
                else
                {
                    KRG_LOG_ERROR( "ResourceServerWorker", "Invalid compile request: %s\n", m_resourceID.ToString().c_str() );
                }

                return;
            }
        }
    }

    bool IsValid() const { return m_isValid; }

public:

    ResourceID          m_resourceID;
    bool                m_triggerDebugBreak = false;
    bool                m_isValid = false;
};

//-------------------------------------------------------------------------

int main( int argc, char* argv[] )
{
    ApplicationGlobalState State;

    //-------------------------------------------------------------------------

    // Read INI settings
    //-------------------------------------------------------------------------

    FileSystemPath const iniPath = FileSystem::GetCurrentProcessPath() + "KRG.ini";

    SettingsRegistry settingsRegistry;
    if ( !settingsRegistry.LoadFromFile( iniPath ) )
    {
        KRG_LOG_ERROR( "ResourceServerWorker", "Failed to read settings from INI file: %s", iniPath.c_str() );
        return 1;
    }

    // Read CMD line arguments and process request
    //-------------------------------------------------------------------------

    ArgumentParser argParser( argc, argv );

    for ( int i = 0; i < argc; i++ )
    {
        std::cout << argv[i] << std::endl;

    }

    if ( argParser.IsValid() )
    {
        Resource::ResourceServerWorker ModuleManager;

        // Debug break
        if ( argParser.m_triggerDebugBreak )
        {
            KRG_HALT();
        }

        Resource::CompilationResult const result = ModuleManager.Compile( argParser.m_resourceID );
        switch ( result )
        {
            case Resource::CompilationResult::Success:
            { 
                return 0;
            }
            break;

            case Resource::CompilationResult::SuccessWithWarnings:
            {
                return 1;
            }
            break;

            case Resource::CompilationResult::Failure:
            default:
            {
                return -1;
            }
            break;
        }
    }
    else
    {
        KRG_LOG_ERROR( "ResourceServerWorker", "Invalid command line arguments" );
        return 1;
    }
}

#endif