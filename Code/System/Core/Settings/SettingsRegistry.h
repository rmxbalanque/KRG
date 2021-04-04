#pragma once

#include "System/Core/Types/Containers.h"
#include "System/Core/Settings/Setting.h"
#include "System/Core/Systems/ISystem.h"
#include "System/Core/FileSystem/FileSystemPath.h"

//-------------------------------------------------------------------------
// Global Settings Registry
//-------------------------------------------------------------------------
// This is the registry for both configuration and debug settings.
// Settings are statically defined as needed (see ConfigSettings.h / DebugSettings.h)
// Settings can be accessed from this registry if absolutely needed but this should be avoided
//-------------------------------------------------------------------------

namespace KRG
{
    class KRG_SYSTEM_CORE_API SettingsRegistry : public ISystem
    {

    public:

        KRG_SYSTEM_ID( SettingsRegistry );

    public:

        bool LoadFromFile( FileSystem::Path const& iniFilePath );
        void SaveToFile( FileSystem::Path const& iniFilePath );

        // Reload (needed to both handle reloading of settings as well as DLL loading which might create new settings )
        void ReloadSettings();

        // Configuration Settings
        THashMap<uint32, Setting*> const& GetAllConfigSettings() const { return m_configSettings; }

        // Debug Settings
        #if KRG_DEVELOPMENT_TOOLS
        THashMap<uint32, Setting*> const& GetAllDebugSettings() const { return m_debugSettings; }
        #endif

    private:

        void GenerateSettingsCaches();

    private:

        // The ini used to populate the values
        FileSystem::Path                m_settingsFilePath;

        // A cache of all settings
        THashMap<uint32, Setting*>      m_configSettings;

        // A cache of all debug settings
        #if KRG_DEVELOPMENT_TOOLS
        THashMap<uint32, Setting*>      m_debugSettings;
        #endif
    };
}