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
    class StringID_CustomAllocator;

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_CORE_API StringID
    {

    public:

        using CachedString = eastl::basic_string<char, StringID_CustomAllocator>;
        using StringCache = eastl::hash_map<uint32, CachedString, eastl::hash<uint32>, eastl::equal_to<uint32>, StringID_CustomAllocator>;

        static StringCache const s_stringCache;
        static StringID const InvalidID;

    public:

        StringID() = default;
        explicit StringID( char const* pStr );
        explicit StringID( uint32 ID ) : m_ID( ID ) {}
        inline explicit StringID( String const& str ) : StringID( str.c_str() ) {}

        inline bool IsValid() const { return m_ID != 0; }
        inline uint32 GetID() const { return m_ID; }
        inline operator uint32() const { return m_ID; }

        inline void Clear() { m_ID = 0; }

        char const* ToString() const;
        inline char const* c_str() const { return ToString(); }

        inline bool operator==( StringID const& rhs ) const { return m_ID == rhs.m_ID; }
        inline bool operator!=( StringID const& rhs ) const { return m_ID != rhs.m_ID; }

    private:

        uint32 m_ID = 0;
    };
}

//-------------------------------------------------------------------------

namespace eastl
{
    template <>
    struct hash<KRG::StringID>
    {
        eastl_size_t operator()( KRG::StringID const& ID ) const { return (KRG::uint32) ID; }
    };
}