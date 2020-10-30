#include "BinaryArchive.h"
#include "System/Core/Memory/Memory.h"
#include <fstream>

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Serialization
    {
        BinaryFileArchive::BinaryFileArchive( Mode mode, FileSystemPath const& filePath )
            : m_filePath( filePath )
            , m_mode( mode )
        {
            KRG_ASSERT( m_filePath.IsFilePath() );

            if ( mode == Mode::Read )
            {
                auto pFileStream = KRG::New<std::ifstream>();
                pFileStream->open( m_filePath, std::ios::in | std::ios::binary );
                m_pStream = pFileStream;

                if ( IsValid() )
                {
                    m_pArchive = KRG::New<cereal::BinaryInputArchive>( *pFileStream );
                }
            }
            else // Write
            {
                auto pFileStream = KRG::New<std::ofstream>();
                pFileStream->open( m_filePath, std::ios::out | std::ios::trunc | std::ios::binary );
                m_pStream = pFileStream;

                if ( IsValid() )
                {
                    m_pArchive = KRG::New<cereal::BinaryOutputArchive>( *pFileStream );
                }
            }
        }

        BinaryFileArchive::~BinaryFileArchive()
        {
            if ( m_pArchive != nullptr )
            {
                if ( m_mode == Mode::Read )
                {
                    auto pInputArchive = reinterpret_cast<cereal::BinaryInputArchive*>( m_pArchive );
                    KRG::Delete( pInputArchive );
                }
                else // Write
                {
                    auto pOutputArchive = reinterpret_cast<cereal::BinaryOutputArchive*>( m_pArchive );
                    KRG::Delete( pOutputArchive );
                }
            }

            //-------------------------------------------------------------------------

            if ( m_pStream != nullptr )
            {
                if ( m_mode == Mode::Read )
                {
                    auto pFileStream = reinterpret_cast<std::ifstream*>( m_pStream );
                    KRG::Delete( pFileStream );
                }
                else // Write
                {
                    auto pFileStream = reinterpret_cast<std::ofstream*>( m_pStream );
                    KRG::Delete( pFileStream );
                }
            }
        }

        bool BinaryFileArchive::IsValid() const
        {
            if ( m_mode == Mode::Read )
            {
                auto pFileStream = reinterpret_cast<std::ifstream*>( m_pStream );
                return pFileStream != nullptr && !pFileStream->fail();
            }
            else // Write
            {
                auto pFileStream = reinterpret_cast<std::ofstream*>( m_pStream );
                return pFileStream != nullptr && !pFileStream->fail();
            }
        }

        //-------------------------------------------------------------------------

        BinaryMemoryArchive::BinaryMemoryArchive( Mode mode, TVector<Byte>& data )
            : m_mode( mode )
            , m_stream( data )
        {
            // Read
            if ( mode == Mode::Read )
            {
                if ( IsValid() )
                {
                    m_pArchive = KRG::New<cereal::BinaryInputArchive>( m_stream );
                }
            }
            else // Write
            {
                data.clear();

                if ( IsValid() )
                {
                    m_pArchive = KRG::New<cereal::BinaryOutputArchive>( m_stream );
                }
            }
        }

        BinaryMemoryArchive::~BinaryMemoryArchive()
        {
            if ( m_pArchive != nullptr )
            {
                if ( m_mode == Mode::Read )
                {
                    auto pInputArchive = reinterpret_cast<cereal::BinaryInputArchive*>( m_pArchive );
                    KRG::Delete( pInputArchive );
                }
                else // Write
                {
                    auto pOutputArchive = reinterpret_cast<cereal::BinaryOutputArchive*>( m_pArchive );
                    KRG::Delete( pOutputArchive );
                }
            }
        }
    }
}
