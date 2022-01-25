#include "ResourceSettings.h"
#include "System/Core/ThirdParty/iniparser/krg_ini.h"
#include "System/Core/Logging/Log.h"
#include "System/Core/FileSystem/FileSystem.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    bool Settings::ReadSettings( IniFile const& ini )
    {
        String s;
        m_workingDirectoryPath = FileSystem::GetCurrentProcessPath();

        // Runtime settings
        //-------------------------------------------------------------------------

        {
            if ( ini.TryGetString( "Paths:CompiledResourcePath", s ) )
            {
                m_compiledResourcePath = m_workingDirectoryPath + s;
                if ( !m_compiledResourcePath.IsValid() )
                {
                    KRG_LOG_ERROR( "Engine", "Invalid compiled data path: %s", m_compiledResourcePath.c_str() );
                    return false;
                }
            }
            else
            {
                KRG_LOG_ERROR( "Engine", "Failed to read compiled data path from ini file" );
                return false;
            }
        }

        // Development only settings
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        {
            if ( ini.TryGetString( "Paths:RawResourcePath", s ) )
            {
                m_rawResourcePath = m_workingDirectoryPath + s;
                if ( !m_rawResourcePath.IsValid() )
                {
                    KRG_LOG_ERROR( "Engine", "Invalid source data path: %s", m_compiledResourcePath.c_str() );
                    return false;
                }
            }
            else
            {
                KRG_LOG_ERROR( "Engine", "Failed to read source data path from ini file" );
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

            uint32 tempValue;
            if ( ini.TryGetUInt( "Resource:ResourceServerPort", tempValue ) )
            {
                m_resourceServerPort = (uint16) tempValue;
            }
            else
            {
                KRG_LOG_ERROR( "Engine", "Failed to read ResourceServerPort from ini file" );
                return false;
            }
        }
        #endif

        return true;
    }
}