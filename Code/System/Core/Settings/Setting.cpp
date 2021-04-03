#include "Setting.h"
#include "System/Core/Algorithm/Hash.h"

//-------------------------------------------------------------------------

namespace KRG
{
    Setting* Setting::s_pHead = nullptr;
    Setting* Setting::s_pTail = nullptr;

    //-------------------------------------------------------------------------

    Setting::Setting( char const* pName, char const* pCategory, Type type )
        : m_type( type )
    {
        KRG_ASSERT( pName != nullptr && pCategory != nullptr );

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
        KRG_ASSERT( categoryLength > 0 && categoryLength < 155 );

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

    Setting::~Setting()
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
}