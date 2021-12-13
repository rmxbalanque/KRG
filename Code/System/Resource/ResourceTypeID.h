#pragma once

#include "_Module/API.h"
#include "System/Core/Types/String.h"
#include "System/Core/Serialization/Serialization.h"

//-------------------------------------------------------------------------
// Resource Type ID
//-------------------------------------------------------------------------
// Unique ID for a resource - Used for resource look up and dependencies
// Resource type IDs are uppercase FourCCs i.e. can only contain upper case ASCII letters and digits

namespace KRG
{
    class KRG_SYSTEM_RESOURCE_API ResourceTypeID
    {
        KRG_SERIALIZE_MEMBERS( KRG_NVP( m_ID ) );

    public:

        inline ResourceTypeID() : m_ID( 0 ) {}
        inline ResourceTypeID( uint32 ID ) : m_ID( ID ) { KRG_ASSERT( IsValidFourCC() ); }
        explicit ResourceTypeID( char const* pStr );
        inline explicit ResourceTypeID( String const& str ) : ResourceTypeID( str.c_str() ) {}

        inline bool IsValid() const { return m_ID != 0; }
        void Clear() { m_ID = 0; }

        inline operator uint32() const { return m_ID; }
        inline operator uint32&() { return m_ID; }

        //-------------------------------------------------------------------------

        inline void GetString( char outStr[5] ) const
        {
            KRG_ASSERT( IsValidFourCC() );

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

        inline InlineString<5> ToString() const
        {
            InlineString<5> str;
            str.resize( 5 );
            GetString( str.data() );
            return str;
        }

    private:

        // Expensive verification to ensure that a resource type ID FourCC only contains uppercase or numeric chars
        inline bool IsValidFourCC() const
        {
            char const str[4] =
            {
                (char) ( m_ID >> 24 ),
                (char) ( ( m_ID & 0x00FF0000 ) >> 16 ),
                (char) ( ( m_ID & 0x0000FF00 ) >> 8 ),
                (char) ( ( m_ID & 0x000000FF ) ),
            };

            // All zero values is not allowed
            if ( str[0] == 0 && str[1] == 0 && str[2] == 0 && str[3] == 0 )
            {
                return false;
            }

            // Ensure that we dont have a zero value between set values
            bool nonZeroFound = false;
            for ( auto i = 0; i < 4; i++ )
            {
                if ( str[i] != 0 )
                {
                    nonZeroFound = true;
                }

                if ( str[i] == 0 && nonZeroFound )
                {
                    return false;
                }
            }

            // Ensure all characters are uppercase ascii or digits
            return
                ( str[0] == 0 || std::isupper( str[0] ) || std::isdigit( str[0] ) ) &&
                ( str[1] == 0 || std::isupper( str[1] ) || std::isdigit( str[1] ) ) &&
                ( str[2] == 0 || std::isupper( str[2] ) || std::isdigit( str[2] ) ) &&
                ( str[3] == 0 || std::isupper( str[3] ) || std::isdigit( str[3] ) );
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