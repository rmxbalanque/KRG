#include "ResourceID.h"

//-------------------------------------------------------------------------

namespace KRG
{
    bool ResourceID::IsValidResourceIDString( char const* pStr )
    {
        KRG_ASSERT( pStr != nullptr );

        size_t const length = strlen( pStr );
        if ( length == 0 )
        {
            return false;
        }

        // Ensure this is a valid data path
        if ( !ResourcePath::IsValidPath( pStr ) )
        {
            return false;
        }

        // Check that we have a resource ID extension
        char const* pFoundExtensionStart = strrchr( pStr, '.' );
        if ( pFoundExtensionStart == nullptr )
        {
            return false;
        }

        size_t const lastCharIdx = length - 1;
        size_t const resourceExtensionStartIdx = pFoundExtensionStart - pStr;

        // Only fourCC extensions allowed
        if( lastCharIdx - resourceExtensionStartIdx > 4 )
        {
            return false;
        }

        return true;
    }

    //-------------------------------------------------------------------------

    void ResourceID::OnPathChanged()
    {
        if ( m_path.IsValid() )
        {
            String const& pathString = m_path.GetString();
            size_t const lastCharIdx = pathString.length() - 1;
            size_t const delimIdx = m_path.GetString().find_last_of( '.' );
            if ( delimIdx != String::npos && lastCharIdx - delimIdx <= 4 )
            {
                char buffer[5] = { 0, 0, 0, 0, 0 };
                for( int32 i = (int32) delimIdx + 1, c = 0; i <= lastCharIdx; i++, c++ )
                {
                    buffer[c] = pathString[i];
                }

                m_type = ResourceTypeID( buffer );
            }
            else
            {
                m_path.Clear();
                m_type = ResourceTypeID::Unknown;
            }
        }
        else
        {
            m_type = ResourceTypeID::Unknown;
        }
    }
}