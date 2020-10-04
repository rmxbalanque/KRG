#pragma once

#include "System/Core/_Module/API.h"
#include "Containers.h"
#include "String.h"

//-------------------------------------------------------------------------
// String ID
//-------------------------------------------------------------------------
// Deterministic numeric ID generated from a string
// StringIDs are CASE-SENSITIVE!

namespace KRG
{
    class KRG_SYSTEM_CORE_API StringID
    {

    public:

        static THashMap<U32, char*> const StringIDMap;
        static StringID const InvalidID;
        static void Initialize();
        static void Shutdown();

    public:

        StringID() = default;
        explicit StringID( char const* pStr );
        explicit StringID( U32 ID );
        inline explicit StringID( String const& str ) : StringID( str.c_str() ) {}

        inline bool operator==( StringID const& rhs ) const { return m_ID == rhs.m_ID; }
        inline bool operator!=( StringID const& rhs ) const { return m_ID != rhs.m_ID; }

        inline bool IsValid() const { return m_ID != 0; }
        inline operator U32() const { return m_ID; }

        char const* ToString() const;
        inline char const* c_str() const { return ToString(); }

    private:

        U32 m_ID = 0;
    };
}

//-------------------------------------------------------------------------

namespace eastl
{
    template <>
    struct hash<KRG::StringID>
    {
        eastl_size_t operator()( KRG::StringID const& ID ) const { return (KRG::U32) ID; }
    };
}