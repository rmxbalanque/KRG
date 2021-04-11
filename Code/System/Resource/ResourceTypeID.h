#pragma once

#include "_Module/API.h"
#include "System/Core/Types/String.h"
#include "System/Core/Serialization/Serialization.h"

//-------------------------------------------------------------------------
// Unique ID for a resource - Used for resource look up and dependencies

namespace KRG
{
    class KRG_SYSTEM_RESOURCE_API ResourceTypeID
    {
        KRG_SERIALIZE_MEMBERS( KRG_NVP( m_ID ) );

    public:

        static ResourceTypeID const Unknown;

    public:

        inline ResourceTypeID() : m_ID( Unknown ) {}
        inline ResourceTypeID( uint32 ID ) : m_ID( ID ) {}
        explicit ResourceTypeID( char const* pStr );
        inline explicit ResourceTypeID( String const& str ) : ResourceTypeID( str.c_str() ) {}

        inline operator uint32() const { return m_ID; }
        inline operator uint32&() { return m_ID; }

        inline bool IsValid() const { return m_ID != 0 && m_ID != Unknown; }

        //-------------------------------------------------------------------------

        inline void GetString( char outStr[5] ) const
        {
            int32 idx = 0;

            outStr[idx] = (uint8) ( m_ID >> 24 );
            if ( outStr[idx] != 0 )
            {
                idx++;
            }

            outStr[idx] = (uint8) ( ( m_ID & 0x00FF0000 ) >> 16 );
            if ( outStr[idx] != 0 )
            {
                idx++;
            }

            outStr[idx] = (uint8) ( ( m_ID & 0x0000FF00 ) >> 8 );
            if ( outStr[idx] != 0 )
            {
                idx++;
            }

            outStr[idx] = (uint8) ( ( m_ID & 0x000000FF ) );
            if ( outStr[idx] != 0 )
            {
                idx++;
            }

            outStr[idx] = 0;
        }

        inline String ToString() const
        {
            char str[5];
            GetString( str );
            return String( str );
        }

    public:

        uint32                 m_ID;
    };
}

//-------------------------------------------------------------------------
// Support for THashMap

namespace eastl
{
    template <>
    struct hash<KRG::ResourceTypeID>
    {
        eastl_size_t operator()( KRG::ResourceTypeID const& ID ) const
        {
            return ( KRG::uint32 ) ID;
        }
    };
}