#pragma once

#include "System/Core/_Module/API.h"
#include "Containers.h"
#include <EASTL/string.h>
#include <EASTL/fixed_string.h>
#include <cstdio>

//-------------------------------------------------------------------------

namespace KRG
{
    using String = eastl::basic_string<char>;
    template<eastl_size_t S> using InlineString = eastl::fixed_string<char, S, true>;

    //-------------------------------------------------------------------------
    // Additional utility functions for string class
    //-------------------------------------------------------------------------

    inline int32 VPrintf( char* pBuffer, uint32 bufferSize, char const* pMessageFormat, va_list args )
    {
        return vsnprintf( pBuffer, size_t( bufferSize ), pMessageFormat, args );
    }

    inline int32 Printf( char* pBuffer, uint32 bufferSize, char const* pMessageFormat, ... )
    {
        va_list args;
        va_start( args, pMessageFormat );
        int32 const numChars = VPrintf( pBuffer, size_t( bufferSize ), pMessageFormat, args );
        va_end( args );
        return numChars;
    }

    //-------------------------------------------------------------------------

    namespace StringUtils
    {
        inline String ReplaceAllOccurrences( String const& originalString, char const* pSearchString, char const* pReplacement )
        {
            KRG_ASSERT( pSearchString != nullptr );
            int32 const searchLength = (int32) strlen( pSearchString );
            if ( originalString.empty() || searchLength == 0 )
            {
                return originalString;
            }

            String copiedString = originalString;
            auto idx = originalString.find( pSearchString );
            while ( idx != String::npos )
            {
                copiedString.replace( idx, searchLength, pReplacement == nullptr ? "" : pReplacement );
                idx = copiedString.find( pSearchString, idx );
            }

            return copiedString;
        }

        inline String& ReplaceAllOccurrencesInPlace( String& originalString, char const* pSearchString, char const* pReplacement )
        {
            KRG_ASSERT( pSearchString != nullptr );
            int32 const searchLength = (int32) strlen( pSearchString );
            if ( originalString.empty() || searchLength == 0 )
            {
                return originalString;
            }

            auto idx = originalString.find( pSearchString );
            while ( idx != String::npos )
            {
                originalString.replace( idx, searchLength, pReplacement == nullptr ? "" : pReplacement );
                idx = originalString.find( pSearchString, idx );
            }

            return originalString;
        }

        inline String RemoveAllOccurrences( String const& originalString, char const* searchString )
        {
            return ReplaceAllOccurrences( originalString, searchString, "" );
        }

        inline String& RemoveAllOccurrencesInPlace( String& originalString, char const* searchString )
        {
            return ReplaceAllOccurrencesInPlace( originalString, searchString, "" );
        }

        inline String StripWhitespace( String const& originalString )
        {
            String strippedString = originalString;
            strippedString.erase( eastl::remove( strippedString.begin(), strippedString.end(), ' ' ), strippedString.end() );
            return strippedString;
        }

        template<typename T>
        inline void Split( String const& str, T& results, char const* pDelimiters = " ", bool ignoreEmptyStrings = true )
        {
            size_t idx, lastIdx = 0;

            while ( true )
            {
                idx = str.find_first_of( pDelimiters, lastIdx );
                if ( idx == String::npos )
                {
                    idx = str.length();

                    if ( idx != lastIdx || !ignoreEmptyStrings )
                    {
                        results.push_back( String( str.data() + lastIdx, idx - lastIdx ) );
                    }
                    break;
                }
                else
                {
                    if ( idx != lastIdx || !ignoreEmptyStrings )
                    {
                        results.push_back( String( str.data() + lastIdx, idx - lastIdx ) );
                    }
                }

                lastIdx = idx + 1;
            }
        }

        //-------------------------------------------------------------------------

        inline uint32 StrToU32( String const& str )
        {
            char* pEnd;
            auto result = strtoul( str.c_str(), &pEnd, 10 );
            KRG_ASSERT( pEnd != nullptr );
            return result;
        }

        inline uint64 StrToU64( String const& str )
        {
            char* pEnd;
            auto result = strtoull( str.c_str(), &pEnd, 10 );
            KRG_ASSERT( pEnd != nullptr );
            return result;
        }

        inline int32 StrToS32( String const& str )
        {
            char* pEnd;
            auto result = strtol( str.c_str(), &pEnd, 10 );
            KRG_ASSERT( pEnd != nullptr );
            return result;
        }

        inline int64 StrToS64( String const& str )
        {
            char* pEnd;
            auto result = strtoll( str.c_str(), &pEnd, 10 );
            KRG_ASSERT( pEnd != nullptr );
            return result;
        }

        inline float StrToF32( String const& str )
        {
            char* pEnd;
            auto result = strtof( str.c_str(), &pEnd );
            KRG_ASSERT( pEnd != nullptr );
            return result;
        }

        inline double StrToF64( String const& str )
        {
            char* pEnd;
            auto result = strtod( str.c_str(), &pEnd );
            KRG_ASSERT( pEnd != nullptr );
            return result;
        }

        //-------------------------------------------------------------------------

         // Is this a valid hex character (0-9 & A-F)
        inline bool IsValidHexChar( char ch )
        {
            return (bool) isxdigit( ch );
        }

        //-------------------------------------------------------------------------

        // Convert hex character (0-9 & A-F) to byte value
        inline Byte HexCharToByteValue( char ch )
        {
            // 0-9
            if ( ch > 47 && ch < 58 ) return (Byte) ( ch - 48 );

            // a-f
            if ( ch > 96 && ch < 103 ) return (Byte) ( ch - 87 );

            // A-F
            if ( ch > 64 && ch < 71 ) return (Byte) ( ch - 55 );

            return 0;
        }

        //-------------------------------------------------------------------------

        // Convert hex character pair (0-9 & A-F) to byte value
        inline Byte HexCharToByteValue( char a, char b )
        {
            return (Byte) ( HexCharToByteValue( a ) * 16 + HexCharToByteValue( b ) );
        }
    }
}