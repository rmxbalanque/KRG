#include "BinaryArchive.h"
#include "System/Core/Memory/Memory.h"
#include "System/Core/Memory/MemoryStreamHelpers.h"
#include <fstream>

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Serialization
    {
        BinaryArchive::BinaryArchive( Mode mode, FileSystemPath const& filePath )
            : m_filePath( filePath )
            , m_mode( mode )
        {
            KRG_ASSERT( m_filePath.IsFilePath() );
            m_stream.m_pInStream = nullptr;
            m_archive.m_pInputArchive = nullptr;

            if ( mode == Mode::Read )
            {
                auto pFileStream = KRG::New<std::ifstream>();
                pFileStream->open( m_filePath, std::ios::in | std::ios::binary );
                m_stream.m_pInStream = pFileStream;

                if ( IsValid() )
                {
                    m_archive.m_pInputArchive = KRG::New<cereal::BinaryInputArchive>( *m_stream.m_pInStream );
                }
            }
            else // Write
            {
                auto pFileStream = KRG::New<std::ofstream>();
                pFileStream->open( m_filePath, std::ios::out | std::ios::trunc | std::ios::binary );
                m_stream.m_pOutStream = pFileStream;

                if ( IsValid() )
                {
                    m_archive.m_pOutputArchive = KRG::New<cereal::BinaryOutputArchive>( *m_stream.m_pOutStream );
                }
            }
        }

        BinaryArchive::BinaryArchive( Mode mode, TVector<Byte>& data )
            : m_mode( mode )
        {
            m_stream.m_pInStream = nullptr;
            m_archive.m_pInputArchive = nullptr;

            if ( mode == Mode::Read )
            {
                m_stream.m_pInStream = KRG::New<MemoryStream>( data );

                if ( IsValid() )
                {
                    m_archive.m_pInputArchive = KRG::New<cereal::BinaryInputArchive>( *m_stream.m_pInStream );
                }
            }
            else // Write
            {
                data.clear();
                m_stream.m_pOutStream = KRG::New<MemoryStream>( data );

                if ( IsValid() )
                {
                    m_archive.m_pOutputArchive = KRG::New<cereal::BinaryOutputArchive>( *m_stream.m_pOutStream );
                }
            }
        }

        BinaryArchive::~BinaryArchive()
        {
            if ( IsValid() )
            {
                if ( m_mode == Mode::Read )
                {
                    KRG::Delete( m_archive.m_pInputArchive );
                    KRG::Delete( m_stream.m_pInStream );
                }
                else // Write
                {
                    KRG::Delete( m_archive.m_pOutputArchive );
                    KRG::Delete( m_stream.m_pOutStream );
                }
            }
        }

        bool BinaryArchive::IsValid() const
        {
            if ( m_mode == Mode::Read )
            {
                return m_stream.m_pInStream != nullptr && !m_stream.m_pInStream->fail();
            }
            else // Write
            {
                return m_stream.m_pOutStream != nullptr && !m_stream.m_pOutStream->fail();
            }
        }
    }
}
