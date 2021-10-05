#include "SettingsRegistry.h"
#include "System/Core/Algorithm/Hash.h"
#include "System/Core/ThirdParty/iniparser/krg_ini.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG
{
    SettingsRegistry::SettingsRegistry()
    {
        #if KRG_DEVELOPMENT_TOOLS
        GenerateDebugSettingsMap();
        #endif
    }

    bool SettingsRegistry::LoadFromFile( FileSystem::Path const& iniFilePath )
    {
        IniFile iniFile( iniFilePath );
        if ( !iniFile.IsValid() )
        {
            KRG_LOG_FATAL_ERROR( "System", "Failed to load settings from INI file: %s", iniFilePath.c_str() );
            return false;
        }

        //-------------------------------------------------------------------------

        m_settingsFilePath = iniFilePath;

        //-------------------------------------------------------------------------
        // Only try to load configuration settings
        // Debug settings are never saved!
        //-------------------------------------------------------------------------

        for ( auto const pSettings : m_settings )
        {
            if ( !pSettings->ReadSettings( iniFile ) )
            {
                return false;
            }
        }

        return true;
    }

    void SettingsRegistry::SaveToFile( FileSystem::Path const& iniFilePath )
    {
        KRG_UNIMPLEMENTED_FUNCTION();
    }

    void SettingsRegistry::ReloadSettings()
    {
        KRG_ASSERT( m_settingsFilePath.IsExistingFile() );
        LoadFromFile( m_settingsFilePath );
    }

    //-------------------------------------------------------------------------

    void SettingsRegistry::RegisterSettings( ISettings* pSettings )
    {
        KRG_ASSERT( pSettings != nullptr );
        KRG_ASSERT( !VectorContains( m_settings, pSettings ) );
        m_settings.emplace_back( pSettings );
    }

    void SettingsRegistry::UnregisterSettings( ISettings* pSettings )
    {
        KRG_ASSERT( pSettings != nullptr );
        KRG_ASSERT( VectorContains( m_settings, pSettings ) );
        m_settings.erase_first_unsorted( pSettings );
    }
}
 
//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG
{
    void SettingsRegistry::GenerateDebugSettingsMap()
    {
        m_debugSettings.clear();

        //-------------------------------------------------------------------------

        DebugSetting* pSetting = DebugSetting::s_pHead;
        while ( pSetting != nullptr )
        {
            // Add to global list - settings are expected to be unique!
            KRG_ASSERT( m_debugSettings.find( pSetting->m_nameHash ) == m_debugSettings.end() );
            m_debugSettings.insert( TPair<uint32, DebugSetting*>( pSetting->m_nameHash, pSetting ) );

            // Move onto next setting
            pSetting = pSetting->m_pNext;
        }
    }

    //-------------------------------------------------------------------------

    DebugSetting const* SettingsRegistry::TryGetDebugSetting( char const* pCategoryName, char const* pSettingName ) const
    {
        KRG_ASSERT( pCategoryName != nullptr && pSettingName != nullptr );
        InlineString<256> combinedName( pCategoryName );
        combinedName += pSettingName;

        uint32 const combinedNameHash = Hash::GetHash32( combinedName.c_str() );

        auto settingsIter = m_debugSettings.find( combinedNameHash );
        if ( settingsIter != m_debugSettings.end() )
        {
            return settingsIter->second;
        }

        return nullptr;
    }
}
#endif