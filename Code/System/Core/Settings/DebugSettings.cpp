#include "DebugSettings.h"
#include "System/Core/Algorithm/Hash.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG
{
    DebugSetting* DebugSetting::s_pHead = nullptr;
    DebugSetting* DebugSetting::s_pTail = nullptr;

    //-------------------------------------------------------------------------

    DebugSetting::DebugSetting( char const* pName, char const* pCategory, char const* pDescription, Type type )
        : m_type( type )
    {
        // Name Validation
        //-------------------------------------------------------------------------

        size_t const nameLength = strlen( pName );
        KRG_ASSERT( nameLength > 0 && nameLength < 100 );

        for ( auto i = 0; i < nameLength - 1; i++ )
        {
            KRG_ASSERT( isalnum( pName[i] ) || pName[i] == ' ' );
        }

        // Basic Category Validation
        //-------------------------------------------------------------------------
        // Derived types are expected to provide more detailed validation

        size_t const categoryLength = strlen( pCategory );
        for ( auto i = 0; i < categoryLength - 1; i++ )
        {
            KRG_ASSERT( isalnum( pCategory[i] ) || pCategory[i] == ' ' || pCategory[i] == s_categorySeperatorToken );
        }

        // Description Validation
        //-------------------------------------------------------------------------

        size_t const descriptionLength = ( pDescription == nullptr ) ? 0 : strlen( pDescription );
        KRG_ASSERT( descriptionLength < 256 );

        if ( pDescription != nullptr )
        {
            strcpy( const_cast<char*>( m_description ), pDescription );
        }

        // Set Internal State
        //-------------------------------------------------------------------------

        // Copy strings
        strcpy( const_cast<char*>( m_name ), pName );
        strcpy( const_cast<char*>( m_category ), pCategory );

        // Generate name hash
        char fullNameBuffer[256] = { 0 };
        memcpy( fullNameBuffer, pCategory, categoryLength );
        memcpy( fullNameBuffer + categoryLength, pName, nameLength );
        m_nameHash = Hash::GetHash32( fullNameBuffer );

        // Add to global list
        if ( s_pHead != nullptr )
        {
            s_pTail->m_pNext = this;
            s_pTail = this;
        }
        else
        {
            s_pHead = this;
            s_pTail = this;
        }
    }

    DebugSetting::~DebugSetting()
    {
        // Remove from global list
        //-------------------------------------------------------------------------

        // If we are the head of the list, just change the head to our next sibling
        if ( s_pHead == this )
        {
            s_pHead = m_pNext;

            // If we are also the tail, then empty the list
            if ( s_pTail == this )
            {
                s_pTail = nullptr;
            }
        }
        else // Find our previous sibling
        {
            auto pPrevious = s_pHead;
            while ( pPrevious != nullptr )
            {
                if ( pPrevious->m_pNext == this )
                {
                    break;
                }

                pPrevious = pPrevious->m_pNext;
            }

            KRG_ASSERT( pPrevious != nullptr );

            // Remove ourselves from the list
            pPrevious->m_pNext = m_pNext;

            // Update the tail of the list if needed
            if ( s_pTail == this )
            {
                s_pTail = pPrevious;
            }
        }
    }

    //-------------------------------------------------------------------------

    DebugSettingBool::DebugSettingBool( char const* pName, char const* pCategory, char const* pDescription, bool initialValue )
        : DebugSetting( pName, pCategory, pDescription, Type::Bool )
        , m_value( initialValue )
    {}

    //-------------------------------------------------------------------------

    DebugSettingInt::DebugSettingInt( char const* pName, char const* pCategory, char const* pDescription, int32 initialValue )
        : DebugSetting( pName, pCategory, pDescription, Type::Int )
        , m_value( initialValue )
    {}

    DebugSettingInt::DebugSettingInt( char const* pName, char const* pCategory, char const* pDescription, int32 initialValue, int32 min, int32 max )
        : DebugSetting( pName, pCategory, pDescription, Type::Int )
        , m_value( initialValue )
        , m_min( min )
        , m_max( max )
    {}

    //-------------------------------------------------------------------------

    DebugSettingFloat::DebugSettingFloat( char const* pName, char const* pCategory, char const* pDescription, float initialValue )
        : DebugSetting( pName, pCategory, pDescription, Type::Float )
        , m_value( initialValue )
    {}

    DebugSettingFloat::DebugSettingFloat( char const* pName, char const* pCategory, char const* pDescription, float initialValue, float min, float max )
        : DebugSetting( pName, pCategory, pDescription, Type::Float )
        , m_value( initialValue )
        , m_min( min )
        , m_max( max )
    {}
}
#endif