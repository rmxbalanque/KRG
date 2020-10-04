#include "ResourceTypeID.h"

//-------------------------------------------------------------------------

namespace KRG
{
    ResourceTypeID const ResourceTypeID::Unknown( 'UNKN' );

    //-------------------------------------------------------------------------

    ResourceTypeID::ResourceTypeID( char const* pStr )
    {
        m_ID = 0;

        size_t const numChars = strlen( pStr );
        if ( numChars > 4 )
        {
            m_ID = ResourceTypeID::Unknown;
        }
        else
        {
            for ( size_t x = 0; x < numChars; x++ )
            {
                m_ID |= (U32) toupper( pStr[x] ) << ( numChars - 1 - x ) * 8;
            }
        }
    }

    String ResourceTypeID::ToString() const
    {
        String value;
        U8 const byte0 = ( U8) ( m_ID >> 24 );
        U8 const byte1 = ( U8) ( ( m_ID & 0x00FF0000 ) >> 16 );
        U8 const byte2 = ( U8) ( ( m_ID & 0x0000FF00 ) >> 8 );
        U8 const byte3 = ( U8) ( ( m_ID & 0x000000FF ) );

        if ( byte0 != 0 ) value += byte0;
        if ( byte1 != 0 ) value += byte1;
        if ( byte2 != 0 ) value += byte2;
        if ( byte3 != 0 ) value += byte3;

        return value;
    }
}