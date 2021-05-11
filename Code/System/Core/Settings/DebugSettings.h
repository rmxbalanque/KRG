#pragma once

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
    class SettingsRegistry;

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_CORE_API DebugSetting
    {
        friend SettingsRegistry;

        constexpr static char const s_categorySeperatorToken = '/';
        static DebugSetting* s_pHead;
        static DebugSetting* s_pTail;

    public:

        enum class Type
        {
            Bool,
            Int,
            Float,
            String
        };

        inline Type GetType() const { return m_type; }
        inline char const* GetName() const { return m_name; }
        inline char const* GetCategory() const { return m_category; }
        inline char const* GetDescription() const { return m_description; }

    protected:

        DebugSetting( char const* pName, char const* pCategory, char const* pDescription, Type type );
        virtual ~DebugSetting();

    protected:

        DebugSetting*       m_pNext = nullptr;
        char const          m_name[100] = { 0 };
        char const          m_category[156] = { 0 };
        uint32              m_nameHash = 0; // The hash of the combined category and name strings
        Type                m_type;
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

        DebugSettingInt( char const* pName, char const* pCategory, char const* pDescription, int32 initialValue );
        DebugSettingInt( char const* pName, char const* pCategory, char const* pDescription, int32 initialValue, int32 min, int32 max );

        inline DebugSettingInt& operator=( int32 value ) { m_value = Math::Clamp( value, m_min, m_max ); return *this; }
        inline operator int32() const { return m_value; }

        inline int32 GetMin() const { return m_min; }
        inline int32 GetMax() const { return m_max; }
        inline bool HasLimits() const { return m_min != INT_MIN || m_max != INT_MAX; }

    private:

        int32                 m_value = false;
        int32                 m_min = INT_MIN;
        int32                 m_max = INT_MAX;
    };

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_CORE_API DebugSettingFloat : public DebugSetting
    {
        friend SettingsRegistry;

    public:

        DebugSettingFloat( char const* pName, char const* pCategory, char const* pDescription, float initialValue );
        DebugSettingFloat( char const* pName, char const* pCategory, char const* pDescription, float initialValue, float min, float max );

        inline DebugSettingFloat& operator=( float value ) { m_value = Math::Clamp( value, m_min, m_max ); return *this; }
        inline operator float() const { return m_value; }

        inline float GetMin() const { return m_min; }
        inline float GetMax() const { return m_max; }
        inline bool HasLimits() const { return m_min != -FLT_MAX || m_max != FLT_MAX; }

    private:

        float                 m_value = false;
        float                 m_min = -FLT_MAX;
        float                 m_max = FLT_MAX;
    };
}
#endif