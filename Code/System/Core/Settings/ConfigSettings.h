#pragma once

#include "Setting.h"
#include "System/Core/Math/Math.h"
#include "System/Core/Types/String.h"
#include "System/Core/Types/Event.h"

//-------------------------------------------------------------------------
// Config Settings
//-------------------------------------------------------------------------
// These are non-debug configuration settings that are stored and read from ini files
//
// Config settings DO NOT support nested categories
//
// Usage:
// static ConfigSettingInt g_setting( "Name", "Category", -1 );
//
//-------------------------------------------------------------------------

namespace KRG
{
    class KRG_SYSTEM_CORE_API ConfigSetting : public Setting
    {

    public:

        static U32 const StaticTypeID = 'CFG';

    public:

        inline bool IsOptional() const { return m_isOptional; }

    protected:

        ConfigSetting( char const* pName, char const* pCategory, Type type, bool isOptional );
        virtual S32 GetTypeID() const override final { return ConfigSetting::StaticTypeID; }

    protected:

        bool m_isOptional = true;   // Is this config setting optional, i.e. can we define a default value?
    };

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_CORE_API ConfigSettingBool : public ConfigSetting
    {
        friend SettingsRegistry;

    public:

        ConfigSettingBool( char const* pName, char const* pCategory );
        ConfigSettingBool( char const* pName, char const* pCategory, bool initialValue );

        inline operator bool() const { return m_value; }

        inline ConfigSettingBool& operator=( bool value ) 
        {
            m_value = value; 
            m_onValueChangedEvent.Execute( m_value );
            return *this; 
        }

        inline TMultiUserEvent<bool>& OnValueChangedEvent() { return m_onValueChangedEvent; }

    private:

        bool                                    m_value = false;
        TMultiUserEventInternal<bool>           m_onValueChangedEvent;
    };

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_CORE_API ConfigSettingInt : public ConfigSetting
    {
        friend SettingsRegistry;

    public:

        ConfigSettingInt( char const* pName, char const* pCategory );
        ConfigSettingInt( char const* pName, char const* pCategory, S32 initialValue );
        ConfigSettingInt( char const* pName, char const* pCategory, S32 initialValue, S32 min, S32 max );

        inline S32 GetMin() const { return m_min; }
        inline S32 GetMax() const { return m_max; }
        inline bool HasLimits() const { return m_min != INT_MIN || m_max != INT_MAX; }

        inline operator S32() const { return m_value; }

        inline ConfigSettingInt& operator=( S32 value ) 
        { 
            S32 oldValue = m_value;
            m_value = Math::Clamp( value, m_min, m_max ); 
            m_onValueChangedEvent.Execute( oldValue, m_value );
            return *this; 
        }

        inline TMultiUserEvent<S32, S32>& OnValueChangedEvent() { return m_onValueChangedEvent; }

    private:

        S32                                     m_value = false;
        S32                                     m_min = INT_MIN;
        S32                                     m_max = INT_MAX;
        TMultiUserEventInternal<S32, S32>       m_onValueChangedEvent;
    };

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_CORE_API ConfigSettingFloat : public ConfigSetting
    {
        friend SettingsRegistry;

    public:

        ConfigSettingFloat( char const* pName, char const* pCategory );
        ConfigSettingFloat( char const* pName, char const* pCategory, F32 initialValue );
        ConfigSettingFloat( char const* pName, char const* pCategory, F32 initialValue, F32 min, F32 max );

        inline F32 GetMin() const { return m_min; }
        inline F32 GetMax() const { return m_max; }
        inline bool HasLimits() const { return m_min != -FLT_MAX || m_max != FLT_MAX; }

        inline operator F32() const { return m_value; }

        inline ConfigSettingFloat& operator=( F32 value )
        { 
            F32 oldValue = m_value;
            m_value = Math::Clamp( value, m_min, m_max );
            m_onValueChangedEvent.Execute( oldValue, m_value );
            return *this; 
        }

        inline TMultiUserEvent<F32, F32>& OnValueChangedEvent() { return m_onValueChangedEvent; }

    private:

        F32                                     m_value = false;
        F32                                     m_min = -FLT_MAX;
        F32                                     m_max = FLT_MAX;
        TMultiUserEventInternal<F32, F32>       m_onValueChangedEvent;
    };

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_CORE_API ConfigSettingString : public ConfigSetting
    {
        friend SettingsRegistry;

        constexpr static S32 const BufferSize = 256;

    public:

        ConfigSettingString( char const* pName, char const* pCategory );
        ConfigSettingString( char const* pName, char const* pCategory, char const* pInitialValue );

        inline operator char const*() const { return m_value; }
        ConfigSettingString& operator=( char const* pValue );

        inline TMultiUserEvent<String const&, char const*>& OnValueChangedEvent() { return m_onValueChangedEvent; }

    private:

        char                                                    m_value[BufferSize];
        TMultiUserEventInternal<String const&, char const*>     m_onValueChangedEvent;
    };
}