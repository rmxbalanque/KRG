#pragma once

#include "System/Core/_Module/API.h"
#include "System/Core/Types/String.h"

//-----------------------------------------------------------------------------

namespace KRG
{
    namespace Hash
    {
        // XXHash
        //-------------------------------------------------------------------------
        // This is the default hash algorithm for the engine

        namespace XXHash
        {
            KRG_SYSTEM_CORE_API uint32 GetHash32( void const* pData, size_t size );

            inline uint32 GetHash32( String const& string )
            {
                return GetHash32( string.c_str(), string.length() );
            }

            inline uint32 GetHash32( char const* pString )
            {
                return GetHash32( pString, strlen( pString ) );
            }

            inline uint32 GetHash32( TVector<Byte> const& data )
            {
                return GetHash32( data.data(), data.size() );
            }

            //-------------------------------------------------------------------------

            KRG_SYSTEM_CORE_API uint64 GetHash64( void const* pData, size_t size );

            inline uint64 GetHash64( String const& string )
            {
                return GetHash64( string.c_str(), string.length() );
            }

            inline uint64 GetHash64( char const* pString )
            {
                return GetHash64( pString, strlen( pString ) );
            }

            inline uint64 GetHash64( TVector<Byte> const& data )
            {
                return GetHash64( data.data(), data.size() );
            }
        }

        // FNV1a
        //-------------------------------------------------------------------------
        // This is a const expression hash
        // Should not be used for anything other than code only features i.e. custom RTTI etc...

        namespace FNV1a
        {
            constexpr uint32 const constValue32 = 0x811c9dc5;
            constexpr uint32 const defaultOffsetBasis32 = 0x1000193;
            constexpr uint64 const constValue64 = 0xcbf29ce484222325;
            constexpr uint64 const defaultOffsetBasis64 = 0x100000001b3;

            constexpr static inline uint32 GetHash32( char const* const str, const uint32 val = constValue32 )
            {
                return ( str[0] == '\0' ) ? val : GetHash32( &str[1], ( (uint64) val ^ uint32( str[0] ) ) * defaultOffsetBasis32 );
            }

            constexpr static inline uint64 GetHash64( char const* const str, const uint64 val = constValue64 )
            {
                return ( str[0] == '\0' ) ? val : GetHash64( &str[1], ( (uint64) val ^ uint64( str[0] ) ) * defaultOffsetBasis64 );
            }
        }

        //-------------------------------------------------------------------------

        inline uint32 GetHash32( String const& string )
        {
            return XXHash::GetHash32( string.c_str(), string.length() );
        }

        inline uint32 GetHash32( char const* pString )
        {
            return XXHash::GetHash32( pString, strlen( pString ) );
        }

        inline uint32 GetHash32( TVector<Byte> const& data )
        {
            return XXHash::GetHash32( data.data(), data.size() );
        }

        inline uint64 GetHash64( String const& string )
        {
            return XXHash::GetHash64( string.c_str(), string.length() );
        }

        inline uint64 GetHash64( char const* pString )
        {
            return XXHash::GetHash64( pString, strlen( pString ) );
        }

        inline uint64 GetHash64( TVector<Byte> const& data )
        {
            return XXHash::GetHash64( data.data(), data.size() );
        }
    }
}