#include "ResourceID.h"

//-------------------------------------------------------------------------

namespace KRG
{
    bool ResourceID::IsValidResourceIDString( char const* pStr )
    {
        KRG_ASSERT( pStr != nullptr );

        if ( strlen( pStr ) == 0 )
        {
            return false;
        }

        // Ensure this is a valid data path
        if ( !DataPath::IsValidDataPath( pStr ) )
        {
            return false;
        }
        
        StringView stringView( pStr );

        // Check that we have a resource ID extension
        size_t const resourceExtensionStartIdx = stringView.find_last_of( '.' );
        if ( resourceExtensionStartIdx == String::npos )
        {
            return false;
        }

        // Only fourCC extensions allowed
        size_t const lastCharIdx = stringView.length() - 1;
        if( lastCharIdx - resourceExtensionStartIdx > 4 )
        {
            return false;
        }

        return true;
    }

    //-------------------------------------------------------------------------

    void ResourceID::OnDataPathChanged()
    {
        if ( m_dataPath.IsValid() )
        {
            String const& pathString = m_dataPath.ToString();
            size_t const lastCharIdx = pathString.length() - 1;
            size_t const delimIdx = m_dataPath.ToString().find_last_of( '.' );
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
                m_dataPath.Clear();
                m_type = ResourceTypeID::Unknown;
            }
        }
        else
        {
            m_type = ResourceTypeID::Unknown;
        }
    }
}