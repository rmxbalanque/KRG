#include "SettingsRegistry.h"
#include "System/Core/Settings/ConfigSettings.h"
#include "System/Core/Settings/DebugSettings.h"
#include "System/Core/Algorithm/Hash.h"
#include "System/Core/ThirdParty/iniparser/krg_ini.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG
{
    SettingsRegistry::SettingsRegistry()
    {
        Setting* pSetting = Setting::s_pHead;
        while ( pSetting != nullptr )
        {
            THashMap<uint32, Setting*>* pSettingsList = nullptr;

            #if KRG_DEVELOPMENT_TOOLS
            if ( pSetting->GetTypeID() == DebugSetting::StaticTypeID )
            {
                pSettingsList = &m_debugSettings;
            }
            else if ( pSetting->GetTypeID() == ConfigSetting::StaticTypeID )
            {
                pSettingsList = &m_configSettings;
            }
            else
            {
                KRG_HALT();
            }
            #else
            KRG_ASSERT( pSetting->GetTypeID() == ConfigSetting::StaticTypeID );
            pSettingsList = &m_configSettings;
            #endif

            KRG_ASSERT( pSettingsList != nullptr );

            //-------------------------------------------------------------------------

            // Add to global list
            pSettingsList->insert( TPair<uint32, Setting*>( pSetting->m_nameHash, pSetting ) );

            // Move onto next setting
            pSetting = pSetting->m_pNext;
        }
    }

    //-------------------------------------------------------------------------

    bool SettingsRegistry::LoadFromFile( FileSystem::Path const& iniFilePath )
    {
        IniFile iniParser( iniFilePath );
        if ( !iniParser.IsValid() )
        {
            KRG_LOG_FATAL_ERROR( "System", "Failed to load settings from INI file: %s", iniFilePath.c_str() );
            return false;
        }

        //-------------------------------------------------------------------------
        // Only try to load configuration settings
        // Debug settings are never saved!
        //-------------------------------------------------------------------------

        bool allRequiredSettingsRead = true;

        String iniKey;
        for ( auto const& settingPair : m_configSettings )
        {
            auto pSetting = static_cast<ConfigSetting*>( settingPair.second );

            iniKey.sprintf( "%s:%s", pSetting->GetCategory(), pSetting->GetName() );

            bool valueWasFoundAndRead = false;

            switch ( pSetting->GetType() )
            {
                case Setting::Type::Bool:
                {
                    bool value;
                    if ( iniParser.TryGetBool( iniKey.c_str(), value ) )
                    {
                        auto pRealSetting = static_cast<ConfigSettingBool*>( pSetting );
                        *pRealSetting = value;
                        valueWasFoundAndRead = true;
                    }
                }
                break;

                case Setting::Type::Int:
                {
                    int32 value;
                    if ( iniParser.TryGetInt( iniKey.c_str(), value ) )
                    {
                        auto pRealSetting = static_cast<ConfigSettingInt*>( pSetting );
                        *pRealSetting = value;
                        valueWasFoundAndRead = true;
                    }
                }
                break;

                case Setting::Type::Float:
                {
                    float value;
                    if ( iniParser.TryGetFloat( iniKey.c_str(), value ) )
                    {
                        auto pRealSetting = static_cast<ConfigSettingFloat*>( pSetting );
                        *pRealSetting = value;
                        valueWasFoundAndRead = true;
                    }
                }
                break;

                case Setting::Type::String:
                {
                    String value;
                    if ( iniParser.TryGetString( iniKey.c_str(), value ) )
                    {
                        auto pRealSetting = static_cast<ConfigSettingString*>( pSetting );
                        *pRealSetting = value.c_str();
                        valueWasFoundAndRead = true;
                    }
                }
                break;
            }

            // Log error
            if ( !valueWasFoundAndRead && !pSetting->IsOptional() )
            {
                KRG_LOG_FATAL_ERROR( "System", "Failed to read required configuration value: %s", iniKey.c_str() );
                allRequiredSettingsRead = false;
            }
        }

        return allRequiredSettingsRead;
    }

    void SettingsRegistry::SaveToFile( FileSystem::Path const& iniFilePath )
    {
        KRG_UNIMPLEMENTED_FUNCTION();
    }
}