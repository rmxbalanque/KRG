#include "Tag.h"

//-------------------------------------------------------------------------

namespace KRG
{
    Tag Tag::FromTagFormatString( String const& tagFormatString )
    {
        Tag tag;

        //-------------------------------------------------------------------------

        String tempString;
        size_t idx, lastIdx = 0;
        for ( int32 i = 0; i < 4; i++ )
        {
            idx = tagFormatString.find_first_of( '.', lastIdx );
            if ( idx == String::npos )
            {
                idx = tagFormatString.length();

                if ( idx != lastIdx )
                {
                    tempString.assign( tagFormatString.data() + lastIdx, idx - lastIdx );
                    tag.m_c[i] = StringID( tempString );
                }
                break;
            }
            else
            {
                if ( idx != lastIdx )
                {
                    tempString.assign( tagFormatString.data() + lastIdx, idx - lastIdx );
                    tag.m_c[i] = StringID( tempString );
                }
            }

            lastIdx = idx + 1;
        }

        return tag;
    }

    String Tag::ToString() const
    {
        String result;

        uint32 const state = GetTagState();
        switch ( state )
        {
            case HasValidCategories_0:
            {
                result.sprintf( "%s", m_c[0].c_str() );
            }
            break;

            case HasValidCategories_01:
            {
                result.sprintf( "%s.%s", m_c[0].c_str(), m_c[1].c_str() );
            }
            break;

            case HasValidCategories_012:
            {
                result.sprintf( "%s.%s.%s", m_c[0].c_str(), m_c[1].c_str(), m_c[2].c_str() );
            }
            break;

            case HasValidCategories_0123:
            {
                result.sprintf( "%s.%s.%s.%s", m_c[0].c_str(), m_c[1].c_str(), m_c[2].c_str(), m_c[3].c_str() );
            }
            break;

            default:
            KRG_UNREACHABLE_CODE(); // Invalid tag
            break;
        }

        return result;
    }
}