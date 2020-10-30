#include "MemoryStreamHelpers.h"

//-------------------------------------------------------------------------

namespace KRG
{
    std::iostream::int_type MemoryStream::Buffer::overflow( int_type c )
    {
        if ( c != EOF )
        {
            m_buffer.push_back( static_cast<char>( c ) );
        }
        return c;
    }

    std::streamsize MemoryStream::Buffer::xsputn( const char* s, std::streamsize num )
    {
        m_buffer.insert( m_buffer.end(), s, s + num );
        return num;
    }

    //-------------------------------------------------------------------------

    std::iostream::int_type MemoryStream::Buffer::underflow()
    {
        if ( m_bufferPos < m_buffer.size() )
        {
            return static_cast<unsigned char>( m_buffer[m_bufferPos] );
        }
        else
        {
            return EOF;
        }
    }

    std::iostream::int_type MemoryStream::Buffer::uflow()
    {
        if ( m_bufferPos < m_buffer.size() )
        {
            return static_cast<unsigned char>( m_buffer[m_bufferPos++] );
        }
        else
        {
            return EOF;
        }
    }

    std::iostream::int_type MemoryStream::Buffer::pbackfail( std::iostream::int_type c )
    {
        // If they are trying to push back a character that they didn't read last that is an error
        eastl_size_t const prev = m_bufferPos - 1;
        if ( c != EOF && prev < m_buffer.size() && c != static_cast<unsigned char>( m_buffer[prev] ) )
        {
            return EOF;
        }

        m_bufferPos = prev;
        return 1;
    }

    std::streamsize MemoryStream::Buffer::xsgetn( char* s, std::streamsize n )
    {
        if ( m_bufferPos < m_buffer.size() )
        {
            const long long num = std::min<long long>( n, m_buffer.size() - m_bufferPos );
            std::memcpy( s, &m_buffer[m_bufferPos], num );
            m_bufferPos += num;
            return num;
        }
        return 0;
    }
}