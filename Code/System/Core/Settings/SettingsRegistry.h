#pragma once

#include "ISettings.h"
#include "System/Core/Types/Containers.h"
#include "System/Core/Settings/DebugSettings.h"
#include "System/Core/Systems/ISystem.h"
#include "System/Core/FileSystem/FileSystemPath.h"

//-------------------------------------------------------------------------
// Global Settings Registry
//-------------------------------------------------------------------------
// This is the registry for both configuration and debug settings.
// Configuration settings are explicitly defined via an interface and registered
// Debugs settings are statically defined as needed (see DebugSettings.h)
//-------------------------------------------------------------------------

namespace KRG
{
    class KRG_SYSTEM_CORE_API SettingsRegistry : public ISystem
    {

    public:

        KRG_SYSTEM_ID( SettingsRegistry );

    public:

        SettingsRegistry();

        bool LoadFromFile( FileSystem::Path const& iniFilePath );
        void SaveToFile( FileSystem::Path const& iniFilePath );
        void ReloadSettings();

        // Registered Settings
        //-------------------------------------------------------------------------

        void RegisterSettings( ISettings* pSettings );
        void UnregisterSettings( ISettings* pSettings );

        template<typename T>
        T const* GetSettings() const
        {
            static_assert( std::is_base_of<ISettings, T>::value, "T must derive from ISettings" );

            T const* pFoundSettings = nullptr;
            for ( auto const& pSettings : m_settings )
            {
                if ( pSettings->GetSettingsID() == T::s_settingsID )
                {
                    pFoundSettings = static_cast<T const*>( pSettings );
                    break;
                }
            }

            return pFoundSettings;
        }

        // Debug Settings
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        THashMap<uint32, DebugSetting*> const& GetAllDebugSettings() const { return m_debugSettings; }
        DebugSetting const* TryGetDebugSetting( char const* pCategoryName, char const* pSettingName ) const;
        #endif

    private:

        #if KRG_DEVELOPMENT_TOOLS
        void GenerateDebugSettingsMap();
        #endif

    private:

        // The ini used to populate the values
        FileSystem::Path                        m_settingsFilePath;

        // The list of registered settings objects
        TVector<ISettings*>                     m_settings;

        // A cache of all debug settings
        #if KRG_DEVELOPMENT_TOOLS
        THashMap<uint32, DebugSetting*>         m_debugSettings;
        #endif
    };
}