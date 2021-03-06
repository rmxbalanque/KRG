#include "UUID.h"

//-------------------------------------------------------------------------

namespace KRG
{
    static_assert( sizeof( UUID ) == 16, "Size of KRG UUID must be 16 bytes" );

    //-------------------------------------------------------------------------

    bool UUID::IsValidUUIDString( char const* pString )
    {
        size_t const strLength = strlen( pString );
        if ( strLength != 36 )
        {
            return false;
        }

        for ( auto i = 0u; i < strLength; i++ )
        {
            char const ch = pString[i];
            if ( ch == '-' )
            {
                if ( i != 8 && i != 13 && i != 18 && i != 23 )
                {
                    return false;
                }
            }
            else if ( !StringUtils::IsValidHexChar( ch ) )
            {
                return false;
            }
        }

        return true;
    }

    UUID::UUID( char const* pString )
    {
        char char0 = '\0';
        char char1 = '\0';
        bool lookingForFirstChar = true;

        size_t const strLength = strlen( pString );
        KRG_ASSERT( strLength == 36 );
        uint32 byteIdx = 0;

        for ( auto i = 0u; i < strLength; i++ )
        {
            char const ch = pString[i];
            if ( ch == '-' )
            {
                KRG_ASSERT( i == 8 || i == 13 || i == 18 || i == 23 );
                continue;
            }

            KRG_ASSERT( byteIdx < 16 && StringUtils::IsValidHexChar( ch ) );

            if ( lookingForFirstChar )
            {
                char0 = ch;
                lookingForFirstChar = false;
            }
            else
            {
                char1 = ch;
                m_data.m_U8[byteIdx++] = StringUtils::HexCharToByteValue( char0, char1 );
                lookingForFirstChar = true;
            }
        }

        KRG_ASSERT( byteIdx == 16 );
    }
}