#pragma once

#include "Setting.h"
#include "System/Core/Math/Math.h"

//-------------------------------------------------------------------------
// Debug Settings
//-------------------------------------------------------------------------
// These are debug-only settings that only exists in debug builds and are usually set via console or IMGUI
// 
// Debug settings support adding description text
// Debug settings support nested categories via the '/' delimiter
//
// Usage:
// static DebugSettingInt g_setting( "Name", "Category/SubCategory/etc...", "Description", -1 );
//
//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG
{
    class KRG_SYSTEM_CORE_API DebugSetting : public Setting
    {
        constexpr static char const CategorySeperatorToken = '/';

    public:

        static U32 const StaticTypeID = 'DBG';

    protected:

        DebugSetting( char const* pName, char const* pCategory, Type type );
        DebugSetting( char const* pName, char const* pCategory, char const* pDescription, Type type );

        inline char const* GetDescription() const { return m_description; }
        virtual S32 GetTypeID() const override final { return DebugSetting::StaticTypeID; }

    protected:

        char const          m_description[256] = { 0 };
    };

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_CORE_API DebugSettingBool : public DebugSetting
    {
        friend SettingsRegistry;

    public:

        DebugSettingBool( char const* pName, char const* pCategory, char const* pDescription, bool initialValue );

        inline DebugSettingBool& operator=( bool value ) { m_value = value; return *this; }
        inline operator bool() const { return m_value; }

    private:

        bool                m_value = false;
    };

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_CORE_API DebugSettingInt : public DebugSetting
    {
        friend SettingsRegistry;

    public:

        DebugSettingInt( char const* pName, char const* pCategory, char const* pDescription, S32 initialValue );
        DebugSettingInt( char const* pName, char const* pCategory, char const* pDescription, S32 initialValue, S32 min, S32 max );

        inline DebugSettingInt& operator=( S32 value ) { m_value = Math::Clamp( value, m_min, m_max ); return *this; }
        inline operator S32() const { return m_value; }

        inline S32 GetMin() const { return m_min; }
        inline S32 GetMax() const { return m_max; }
        inline bool HasLimits() const { return m_min != INT_MIN || m_max != INT_MAX; }

    private:

        S32                 m_value = false;
        S32                 m_min = INT_MIN;
        S32                 m_max = INT_MAX;
    };

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_CORE_API DebugSettingFloat : public DebugSetting
    {
        friend SettingsRegistry;

    public:

        DebugSettingFloat( char const* pName, char const* pCategory, char const* pDescription, F32 initialValue );
        DebugSettingFloat( char const* pName, char const* pCategory, char const* pDescription, F32 initialValue, F32 min, F32 max );

        inline DebugSettingFloat& operator=( F32 value ) { m_value = Math::Clamp( value, m_min, m_max ); return *this; }
        inline operator F32() const { return m_value; }

        inline F32 GetMin() const { return m_min; }
        inline F32 GetMax() const { return m_max; }
        inline bool HasLimits() const { return m_min != -FLT_MAX || m_max != FLT_MAX; }

    private:

        F32                 m_value = false;
        F32                 m_min = -FLT_MAX;
        F32                 m_max = FLT_MAX;
    };
}
#endif