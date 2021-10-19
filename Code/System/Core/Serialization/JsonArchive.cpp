#include "JsonArchive.h"
#include "System/Core/Memory/Memory.h"
#include <fstream>

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Serialization
    {
        JsonFileArchive::JsonFileArchive( Mode mode, FileSystem::Path const& filePath )
            : m_filePath( filePath )
            , m_mode( mode )
        {
            KRG_ASSERT( m_filePath.IsFile() );

            if ( mode == Mode::Read )
            {
                auto pFileStream = KRG::New<std::ifstream>();
                pFileStream->open( m_filePath, std::ios::in | std::ios::binary );
                m_pStream = pFileStream;

                if ( IsValid() )
                {
                    m_pArchive = KRG::New<cereal::JSONInputArchive>( *pFileStream );
                }
            }
            else // Write
            {
                auto pFileStream = KRG::New<std::ofstream>();
                pFileStream->open( m_filePath, std::ios::out | std::ios::trunc | std::ios::binary );
                m_pStream = pFileStream;

                if ( IsValid() )
                {
                    m_pArchive = KRG::New<cereal::JSONOutputArchive>( *pFileStream );
                }
            }
        }

        JsonFileArchive::~JsonFileArchive()
        {
            if ( m_pArchive != nullptr )
            {
                if ( m_mode == Mode::Read )
                {
                    auto pInputArchive = reinterpret_cast<cereal::JSONInputArchive*>( m_pArchive );
                    KRG::Delete( pInputArchive );
                }
                else // Write
                {
                    auto pOutputArchive = reinterpret_cast<cereal::JSONOutputArchive*>( m_pArchive );
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

        bool JsonFileArchive::IsValid() const
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

        JsonMemoryArchive::JsonMemoryArchive( Mode mode, TVector<Byte>& data )
            : m_mode( mode )
            , m_stream( data )
        {
            // Read
            if ( mode == Mode::Read )
            {
                if ( IsValid() )
                {
                    m_pArchive = KRG::New<cereal::JSONInputArchive>( m_stream );
                }
            }
            else // Write
            {
                data.clear();

                if ( IsValid() )
                {
                    m_pArchive = KRG::New<cereal::JSONOutputArchive>( m_stream );
                }
            }
        }

        JsonMemoryArchive::~JsonMemoryArchive()
        {
            if ( m_pArchive != nullptr )
            {
                if ( m_mode == Mode::Read )
                {
                    auto pInputArchive = reinterpret_cast<cereal::JSONInputArchive*>( m_pArchive );
                    KRG::Delete( pInputArchive );
                }
                else // Write
                {
                    auto pOutputArchive = reinterpret_cast<cereal::JSONOutputArchive*>( m_pArchive );
                    KRG::Delete( pOutputArchive );
                }
            }
        }
    }
}
