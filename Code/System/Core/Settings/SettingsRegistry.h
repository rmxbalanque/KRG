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
    class FileSystemPath;

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_CORE_API SettingsRegistry : public ISystem
    {

    public:

        KRG_SYSTEM_ID( SettingsRegistry );

    public:

        SettingsRegistry();

        bool LoadFromFile( FileSystemPath const& iniFilePath );
        void SaveToFile( FileSystemPath const& iniFilePath );

        // Configuration Settings
        THashMap<U32, Setting*> const& GetAllConfigSettings() const { return m_configSettings; }

        // Debug Settings
        #if KRG_DEBUG_INSTRUMENTATION
        THashMap<U32, Setting*> const& GetAllDebugSettings() const { return m_debugSettings; }
        #endif

    private:

        THashMap<U32, Setting*>     m_configSettings;

        #if KRG_DEBUG_INSTRUMENTATION
        THashMap<U32, Setting*>     m_debugSettings;
        #endif
    };
}