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
        if ( numChars <= 4 )
        {
            for ( size_t x = 0; x < numChars; x++ )
            {
                m_ID |= (uint32) toupper( pStr[x] ) << ( numChars - 1 - x ) * 8;
            }
        }
    }
}