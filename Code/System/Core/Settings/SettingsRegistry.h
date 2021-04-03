#pragma once

#include "System/Core/Types/Containers.h"
#include "System/Core/Settings/Setting.h"
#include "System/Core/Systems/ISystem.h"

//-------------------------------------------------------------------------
// Global Settings Registry
//-------------------------------------------------------------------------
// This is the registry for both configuration and debug settings.
// Settings are statically defined as needed (see ConfigSettings.h / DebugSettings.h)
// Settings can be accessed from this registry if absolutely needed but this should be avoided
//-------------------------------------------------------------------------

namespace KRG
{
    namespace FileSystem { class Path; }

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_CORE_API SettingsRegistry : public ISystem
    {

    public:

        KRG_SYSTEM_ID( SettingsRegistry );

    public:

        SettingsRegistry();

        bool LoadFromFile( FileSystem::Path const& iniFilePath );
        void SaveToFile( FileSystem::Path const& iniFilePath );

        // Configuration Settings
        THashMap<uint32, Setting*> const& GetAllConfigSettings() const { return m_configSettings; }

        // Debug Settings
        #if KRG_DEVELOPMENT_TOOLS
        THashMap<uint32, Setting*> const& GetAllDebugSettings() const { return m_debugSettings; }
        #endif

    private:

        THashMap<uint32, Setting*>     m_configSettings;

        #if KRG_DEVELOPMENT_TOOLS
        THashMap<uint32, Setting*>     m_debugSettings;
        #endif
    };
}