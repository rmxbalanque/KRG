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

        static uint32 const StaticTypeID = 'CFG';

    public:

        inline bool IsOptional() const { return m_isOptional; }

    protected:

        ConfigSetting( char const* pName, char const* pCategory, Type type, bool isOptional );
        virtual int32 GetTypeID() const override final { return ConfigSetting::StaticTypeID; }

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

        inline bool GetValue() const { return m_value; }
        inline operator bool() const { return m_value; }

        inline ConfigSettingBool& operator=( bool value ) 
        {
            m_value = value; 
            m_onValueChangedEvent.Execute( m_value );
            return *this; 
        }

        inline TMultiUserEvent<bool> OnValueChangedEvent() { return m_onValueChangedEvent; }

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
        ConfigSettingInt( char const* pName, char const* pCategory, int32 initialValue );
        ConfigSettingInt( char const* pName, char const* pCategory, int32 initialValue, int32 min, int32 max );

        inline int32 GetMin() const { return m_min; }
        inline int32 GetMax() const { return m_max; }
        inline bool HasLimits() const { return m_min != INT_MIN || m_max != INT_MAX; }

        inline int32 GetValue() const { return m_value; }
        inline operator int32() const { return m_value; }

        inline ConfigSettingInt& operator=( int32 value ) 
        { 
            int32 oldValue = m_value;
            m_value = Math::Clamp( value, m_min, m_max ); 
            m_onValueChangedEvent.Execute( oldValue, m_value );
            return *this; 
        }

        inline TMultiUserEvent<int32, int32> OnValueChangedEvent() { return m_onValueChangedEvent; }

    private:

        int32                                     m_value = false;
        int32                                     m_min = INT_MIN;
        int32                                     m_max = INT_MAX;
        TMultiUserEventInternal<int32, int32>       m_onValueChangedEvent;
    };

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_CORE_API ConfigSettingFloat : public ConfigSetting
    {
        friend SettingsRegistry;

    public:

        ConfigSettingFloat( char const* pName, char const* pCategory );
        ConfigSettingFloat( char const* pName, char const* pCategory, float initialValue );
        ConfigSettingFloat( char const* pName, char const* pCategory, float initialValue, float min, float max );

        inline float GetMin() const { return m_min; }
        inline float GetMax() const { return m_max; }
        inline bool HasLimits() const { return m_min != -FLT_MAX || m_max != FLT_MAX; }

        inline float GetValue() const { return m_value; }
        inline operator float() const { return m_value; }

        inline ConfigSettingFloat& operator=( float value )
        { 
            float oldValue = m_value;
            m_value = Math::Clamp( value, m_min, m_max );
            m_onValueChangedEvent.Execute( oldValue, m_value );
            return *this; 
        }

        inline TMultiUserEvent<float, float> OnValueChangedEvent() { return m_onValueChangedEvent; }

    private:

        float                                       m_value = false;
        float                                       m_min = -FLT_MAX;
        float                                       m_max = FLT_MAX;
        TMultiUserEventInternal<float, float>       m_onValueChangedEvent;
    };

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_CORE_API ConfigSettingString : public ConfigSetting
    {
        friend SettingsRegistry;

        constexpr static int32 const BufferSize = 256;

    public:

        ConfigSettingString( char const* pName, char const* pCategory );
        ConfigSettingString( char const* pName, char const* pCategory, char const* pInitialValue );

        inline char const* GetValue() const { return m_value; }
        inline operator char const*() const { return m_value; }
        ConfigSettingString& operator=( char const* pValue );

        inline TMultiUserEvent<String const&, char const*> OnValueChangedEvent() { return m_onValueChangedEvent; }

    private:

        char                                                    m_value[BufferSize];
        TMultiUserEventInternal<String const&, char const*>     m_onValueChangedEvent;
    };
}