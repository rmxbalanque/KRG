#pragma once

#include <istream>
#include "Containers.h"

//-------------------------------------------------------------------------
// Helper to allow c++ stream access to a fixed block of memory
//-------------------------------------------------------------------------
// Note: does not take ownership of the memory buffer, itr only acts as a view on the data

namespace KRG
{
    struct MemoryBuffer : std::streambuf
    {
        inline MemoryBuffer( TVector<Byte>& data )
            : m_pData( (char*) data.data() )
            , m_size( data.size() )
        {
            KRG_ASSERT( !data.empty() );
            this->setg( m_pData, m_pData, m_pData + m_size );
        }

        inline MemoryBuffer( void* pData, size_t dataSize )
            : m_pData( (char*)  pData )
            , m_size( dataSize )
        {
            KRG_ASSERT( pData != nullptr && dataSize > 0 );
            this->setg( m_pData, m_pData, m_pData + m_size );
        }

    private:

        char*           m_pData = nullptr;
        size_t          m_size = 0;
    };

    //-------------------------------------------------------------------------

    struct MemoryStream : virtual MemoryBuffer, std::istream
    {
        MemoryStream( TVector<Byte>& data )
            : MemoryBuffer( data )
            , std::istream( static_cast<std::streambuf*>( this ) )
        {}

        MemoryStream( void* pData, size_t dataSize )
            : MemoryBuffer( pData, dataSize )
            , std::istream( static_cast<std::streambuf*>( this ) )
        {}
    };
}