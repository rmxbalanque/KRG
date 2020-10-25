#pragma once

#include <istream>
#include "Containers.h"

//-------------------------------------------------------------------------

namespace KRG
{
    //-------------------------------------------------------------------------
    // Memory Stream
    //-------------------------------------------------------------------------
    // Provide iostream access to an existing array
    // Note: does not take ownership of the memory

    class MemoryStream final : public std::iostream
    {
        struct MemoryBuffer final : public std::streambuf
        {
            inline MemoryBuffer( TVector<Byte>& data )
                : m_buffer( data )
            {}

            // Write
            //-------------------------------------------------------------------------

            int_type overflow( int_type c ) override final
            {
                if ( c != EOF ) m_buffer.push_back( static_cast<char>( c ) );
                return c;
            }

            std::streamsize xsputn( const char* s, std::streamsize num ) override final
            {
                m_buffer.insert( m_buffer.end(), s, s + num );
                return num;
            }

            // Read
            //-------------------------------------------------------------------------

            virtual int_type underflow() override final
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

            virtual int_type uflow() override final
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

            virtual int_type pbackfail( int_type c ) override final
            {
                // if they are trying to push back a character that they didn't read last
                // that is an error
                eastl_size_t const prev = m_bufferPos - 1;
                if ( c != EOF && prev < m_buffer.size() && c != static_cast<unsigned char>( m_buffer[prev] ) )
                {
                    return EOF;
                }

                m_bufferPos = prev;
                return 1;
            }

            virtual std::streamsize xsgetn( char* s, std::streamsize n ) override final
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

        private:

            TVector<Byte>&      m_buffer;
            eastl_size_t        m_bufferPos = 0;
        };

    public:

        MemoryStream( TVector<Byte>& data )
            : std::iostream( &m_buffer )
            , m_buffer( data )
        {}

    private:

        MemoryBuffer m_buffer;
    };

    //-------------------------------------------------------------------------
    // Memory Stream View
    //-------------------------------------------------------------------------
    // Provide read-only access to a block of memory
    // Note: does not take ownership of the memory

    class MemoryStreamView final : public std::istream
    {
        struct MemoryBuffer final : public std::streambuf
        {
            inline MemoryBuffer( char* pData, size_t dataSize )
                : m_pData( pData )
                , m_size( dataSize )
            {
                KRG_ASSERT( pData != nullptr && dataSize > 0 );
                this->setg( m_pData, m_pData, m_pData + m_size );
            }

        private:

            char*       m_pData = nullptr;
            size_t      m_size = 0;
        };

    public:

        MemoryStreamView( Byte const* pData, size_t dataSize )
            : std::istream( static_cast<std::streambuf*>( &m_buffer ) )
            , m_buffer( const_cast<char*>( (char const*) pData ), dataSize )
        {}

    private:

        MemoryBuffer m_buffer;
    };
}