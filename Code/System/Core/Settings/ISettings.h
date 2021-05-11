#pragma once

#include "System/Core/_Module/API.h"
#include "System/Core/Algorithm/Hash.h"

//-------------------------------------------------------------------------
// Defines an interface for a group of settings
//-------------------------------------------------------------------------
// 
// We can define a set of settings to be read from an INI file. 
// These settings need to derive from ISettings and be registered with the settings registry
//
// This is done so that we can directly access any set of settings directly from the settings registry
// 
//-------------------------------------------------------------------------

namespace KRG
{
    class IniFile;

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_CORE_API ISettings
    {
        friend class SettingsRegistry;

        static ISettings* s_pHead;
        static ISettings* s_pTail;

    public:

        virtual uint32 GetSettingsID() const = 0;

    protected:

        virtual bool ReadSettings( IniFile const& ini ) = 0;
    };
}

//-------------------------------------------------------------------------

#define KRG_SETTINGS_ID( TypeName ) \
constexpr static uint32 const s_settingsID = Hash::FNV1a::GetHash32( #TypeName ); \
virtual uint32 GetSettingsID() const override final { return TypeName::s_settingsID; }