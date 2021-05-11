#include "ResourceSettings.h"
#include "System/Core/ThirdParty/iniparser/krg_ini.h"
#include "System/Core/Logging/Log.h"
#include "System/Core/FileSystem/FileSystem.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    bool Settings::ReadSettings( IniFile const& ini )
    {
        #if KRG_DEVELOPMENT_TOOLS
        {
            String s;
            m_workingDirectoryPath = FileSystem::GetCurrentProcessPath();

            if ( ini.TryGetString( "Paths:SourceDataPath", s ) )
            {
                m_sourceDataPath = m_workingDirectoryPath + s;
                if ( !m_sourceDataPath.IsValid() )
                {
                    KRG_LOG_ERROR( "Engine", "Invalid source data path: %s", m_compiledDataPath.c_str() );
                    return false;
                }
            }
            else
            {
                KRG_LOG_ERROR( "Engine", "Failed to read source data path from ini file" );
                return false;
            }

            //-------------------------------------------------------------------------

            if ( ini.TryGetString( "Paths:CompiledDataPath", s ) )
            {
                m_compiledDataPath = m_workingDirectoryPath + s;
                if ( !m_compiledDataPath.IsValid() )
                {
                    KRG_LOG_ERROR( "Engine", "Invalid compiled data path: %s", m_compiledDataPath.c_str() );
                    return false;
                }
            }
            else
            {
                KRG_LOG_ERROR( "Engine", "Failed to read compiled data path from ini file" );
                return false;
            }

            //-------------------------------------------------------------------------

            if ( ini.TryGetString( "Resource:CompiledResourceDatabaseName", s ) )
            {
                m_compiledResourceDatabasePath = m_workingDirectoryPath + s;
                if ( !m_compiledResourceDatabasePath.IsValid() )
                {
                    KRG_LOG_ERROR( "Engine", "Invalid compiled resource database path: %s", m_compiledResourceDatabasePath.c_str() );
                    return false;
                }
            }
            else
            {
                KRG_LOG_ERROR( "Engine", "Failed to read compiled resource database path from ini file" );
                return false;
            }

            // Resource Compiler
            //-------------------------------------------------------------------------

            if ( ini.TryGetString( "Resource:ResourceCompilerExecutablePath", s ) )
            {
                m_resourceCompilerExecutablePath = m_workingDirectoryPath + s;
                if ( !m_resourceCompilerExecutablePath.IsValid() )
                {
                    KRG_LOG_ERROR( "Engine", "Invalid resource compiler path: %s", m_resourceCompilerExecutablePath.c_str() );
                    return false;
                }
            }
            else
            {
                KRG_LOG_ERROR( "Engine", "Failed to read resource compiler exe path from ini file" );
                return false;
            }

            // Resource Server
            //-------------------------------------------------------------------------

            if ( ini.TryGetString( "Resource:ResourceServerExecutablePath", s ) )
            {
                m_resourceServerExecutablePath = m_workingDirectoryPath + s;
                if ( !m_resourceServerExecutablePath.IsValid() )
                {
                    KRG_LOG_ERROR( "Engine", "Invalid resource server path: %s", m_resourceServerExecutablePath.c_str() );
                    return false;
                }
            }
            else
            {
                KRG_LOG_ERROR( "Engine", "Failed to read resource server exe path from ini file" );
                return false;
            }

            if ( !ini.TryGetString( "Resource:ResourceServerAddress", m_resourceServerNetworkAddress ) )
            {
                KRG_LOG_ERROR( "Engine", "Failed to read ResourceServerAddress from ini file" );
                return false;
            }

            if ( !ini.TryGetUInt( "Resource:ResourceServerPort", m_resourceServerPort ) )
            {
                KRG_LOG_ERROR( "Engine", "Failed to read ResourceServerPort from ini file" );
                return false;
            }
        }
        #endif

        return true;
    }
}