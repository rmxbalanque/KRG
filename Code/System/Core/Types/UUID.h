#pragma once

#include "System/Core/_Module/API.h"
#include "System/Core/Serialization/Serialization.h"
#include "String.h"

//-------------------------------------------------------------------------

namespace KRG
{
    // Universally Unique ID - 128bit Variant 1 ID
    //-------------------------------------------------------------------------
    // Guaranteed to be unique

    class KRG_SYSTEM_CORE_API UUID
    {
        KRG_SERIALIZE_MEMBERS( KRG_NVP( m_data.m_U64 ) );

        union Data
        {
            uint64         m_U64[2];
            uint32         m_U32[4];
            uint8          m_U8[16];
        };

    public:

        static UUID GenerateID();
        static bool IsValidUUIDString( char const* pString );

    public:

        inline UUID() { Memory::MemsetZero( &m_data ); }
        inline UUID( uint64 v0, uint64 v1 ) { m_data.m_U64[0] = v0; m_data.m_U64[1] = v1; }
        inline UUID( uint32 v0, uint32 v1, uint32 v2, uint32 v3 ) { m_data.m_U32[0] = v0; m_data.m_U32[1] = v1; m_data.m_U32[2] = v2; m_data.m_U32[3] = v3; }
        inline UUID( String const& str ) : UUID( str.c_str() ) {}
        UUID( char const* pString );

        inline String ToString() const
        {
            return String().sprintf( "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x", m_data.m_U8[0], m_data.m_U8[1], m_data.m_U8[2], m_data.m_U8[3], m_data.m_U8[4], m_data.m_U8[5], m_data.m_U8[6], m_data.m_U8[7], m_data.m_U8[8], m_data.m_U8[9], m_data.m_U8[10], m_data.m_U8[11], m_data.m_U8[12], m_data.m_U8[13], m_data.m_U8[14], m_data.m_U8[15] );
        }

        inline bool IsValid() const { return m_data.m_U64[0] != 0 && m_data.m_U64[1] != 0; }
        inline void Reset() { Memory::MemsetZero( &m_data ); }

        inline uint8 GetValueU8( uint32 idx ) const { KRG_ASSERT( idx < 16 ); return m_data.m_U8[idx]; }
        inline uint32 GetValueU32( uint32 idx ) const { KRG_ASSERT( idx < 4 ); return m_data.m_U32[idx]; }
        inline uint64 GetValueU64( uint32 idx ) const { KRG_ASSERT( idx < 2 ); return m_data.m_U64[idx]; }

        KRG_FORCE_INLINE bool operator==( UUID const& RHS ) const { return m_data.m_U64[0] == RHS.m_data.m_U64[0] && m_data.m_U64[1] == RHS.m_data.m_U64[1]; }
        KRG_FORCE_INLINE bool operator!=( UUID const& RHS ) const { return m_data.m_U64[0] != RHS.m_data.m_U64[0] || m_data.m_U64[1] != RHS.m_data.m_U64[1]; }

    private:

        Data m_data;
    };
}

//-------------------------------------------------------------------------

namespace eastl
{
    // Specialization for eastl::hash<UUID>
    // This uses the C# algorithm for generating a hash from a ID
    template <>
    struct hash<KRG::UUID>
    {
        eastl_size_t operator()( KRG::UUID const& ID ) const
        {
            struct GUIDData
            {
                KRG::uint32     m_a;
                KRG::uint16     m_b;
                KRG::uint16     m_c;
                KRG::uint8      m_d[8];
            };

            GUIDData const& tmp = reinterpret_cast<GUIDData const&>( ID );
            eastl_size_t hash = tmp.m_a ^ ( tmp.m_b << 16 | ( KRG::uint8 ) tmp.m_c ) ^ ( tmp.m_d[2] << 24 | tmp.m_d[7] );
            return hash;
        }
    };
}