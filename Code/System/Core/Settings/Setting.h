#pragma once

#include "../_Module/API.h"
#include "System/Core/Core.h"

//-------------------------------------------------------------------------
// Settings
//-------------------------------------------------------------------------
// Settings are expected to be statically allocated, we do not support settings created at runtime
//
// Each setting has a name and a category.
// Category formats are defined by each setting type (i.e. config settings have different constraints than debug settings)
//
// Notes:
//  * Special characters are not allowed in names and category names
//-------------------------------------------------------------------------

namespace KRG
{
    class SettingsRegistry;

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_CORE_API Setting
    {
        friend SettingsRegistry;

        static Setting* Head;
        static Setting* Tail;

    public:

        enum class Type
        {
            Bool,
            Int,
            Float,
            String
        };

    public:

        inline Type GetType() const { return m_type; }
        inline char const* GetName() const { return m_name; }
        inline char const* GetCategory() const { return m_category; }

    protected:

        Setting( const char* pName, const char* pCategory, Type type );
        virtual ~Setting();

        virtual S32 GetTypeID() const = 0;

    private:

        Setting( Setting const& ) = delete;
        Setting& operator=( Setting const& ) = delete;

    protected:

        Setting*            m_pNext = nullptr;
        char const          m_name[100] = { 0 };
        char const          m_category[156] = { 0 };
        U32                 m_nameHash = 0; // The hash of the combined category and name strings
        Type                m_type;
    };
}