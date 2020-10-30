#pragma once

#include <istream>
#include "System/Core/Types/Containers.h"
#include "System/Core/_Module/API.h"

//-------------------------------------------------------------------------

namespace KRG
{
    //-------------------------------------------------------------------------
    // Memory Stream
    //-------------------------------------------------------------------------
    // Provide iostream access to an existing array
    // Note: does not take ownership of the memory

    class KRG_SYSTEM_CORE_API MemoryStream final : public std::iostream
    {
        struct KRG_SYSTEM_CORE_API Buffer final : public std::streambuf
        {
            inline Buffer( TVector<Byte>& data )
                : m_buffer( data )
            {}

            // Write
            //-------------------------------------------------------------------------

            virtual int_type overflow( int_type c ) override final;
            virtual std::streamsize xsputn( const char* s, std::streamsize num ) override final;

            // Read
            //-------------------------------------------------------------------------

            virtual int_type underflow() override final;
            virtual int_type uflow() override final;
            virtual int_type pbackfail( int_type c ) override final;
            virtual std::streamsize xsgetn( char* s, std::streamsize n ) override final;

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

        Buffer m_buffer;
    };

    //-------------------------------------------------------------------------
    // Memory Stream View
    //-------------------------------------------------------------------------
    // Provide read-only access to a block of memory
    // Note: does not take ownership of the memory

    class KRG_SYSTEM_CORE_API MemoryStreamView final : public std::istream
    {
        struct Buffer final : public std::streambuf
        {
            inline Buffer( char* pData, size_t dataSize )
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

        Buffer m_buffer;
    };
}