#pragma once

#include "_Module/API.h"
#include "System/Core/Settings/ISettings.h"
#include "System/Core/Math/Math.h"
#include "System/Core/FileSystem/FileSystemPath.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    class KRG_SYSTEM_RESOURCE_API Settings : public ISettings
    {
    public:

        KRG_SETTINGS_ID( KRG::Resource::Settings );

    protected:

        virtual bool ReadSettings( IniFile const& ini ) override;

    public:

        #if KRG_DEVELOPMENT_TOOLS
        String                  m_resourceServerNetworkAddress;
        uint16                  m_resourceServerPort;
        FileSystem::Path        m_workingDirectoryPath;
        FileSystem::Path        m_sourceDataPath;
        FileSystem::Path        m_compiledDataPath;
        FileSystem::Path        m_compiledResourceDatabasePath;
        FileSystem::Path        m_resourceServerExecutablePath;
        FileSystem::Path        m_resourceCompilerExecutablePath;
        #endif
    };
}