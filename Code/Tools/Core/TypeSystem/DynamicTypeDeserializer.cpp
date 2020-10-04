#include "DynamicTypeDeserializer.h"
#include "DynamicTypeInstance.h"
#include "DynamicTypeSerialization.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/ITypeHelper.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

using namespace rapidjson;
using namespace KRG::TypeSystem;

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Serialization
    {
        DynamicTypeDeserializer::DynamicTypeDeserializer( TypeSystem::TypeRegistry const& typeRegistry )
            : m_typeRegistry( typeRegistry )
        {}

        DynamicTypeDeserializer::~DynamicTypeDeserializer()
        {
            Reset();
        }

        bool DynamicTypeDeserializer::ReadFromFile( FileSystemPath const& filePath )
        {
            KRG_ASSERT( filePath.IsFilePath() );
            Reset();

            //-------------------------------------------------------------------------

            if ( filePath.Exists() )
            {
                FILE* pFile = fopen( filePath, "r" );

                if ( pFile == nullptr )
                {
                    return false;
                }

                fseek( pFile, 0, SEEK_END );
                size_t filesize = (size_t) ftell( pFile );
                fseek( pFile, 0, SEEK_SET );

                m_pFileBuffer = (Byte*) KRG::Alloc( filesize + 1 );
                size_t readLength = fread( m_pFileBuffer, 1, filesize, pFile );
                m_pFileBuffer[readLength] = '\0';
                fclose( pFile );

                m_document.ParseInsitu( (char*) m_pFileBuffer );

                bool const isValidJsonFile = ( m_document.GetParseError() == kParseErrorNone );
                if ( isValidJsonFile )
                {
                    if ( m_document.IsArray() )
                    {
                        m_numSerializedTypes = m_document.Size();
                    }
                    else
                    {
                        m_numSerializedTypes = m_document.IsObject() ? 1 : 0;
                    }
                }

                return isValidJsonFile;
            }

            return true;
        }

        void DynamicTypeDeserializer::Reset()
        {
            m_document.Clear();
            m_numSerializedTypes = 0;
            m_deserializedTypeIdx = 0;

            if ( m_pFileBuffer != nullptr )
            {
                KRG::Free( m_pFileBuffer );
            }
        }

        void DynamicTypeDeserializer::DeserializeType( DynamicTypeInstance& outType ) const
        {
            KRG_ASSERT( m_deserializedTypeIdx < m_numSerializedTypes );

            if ( m_numSerializedTypes > 1 )
            {
                KRG_ASSERT( m_document.IsArray() );
                ReadSerializedDynamicTypeFromJSON( m_typeRegistry, m_document[m_deserializedTypeIdx++], outType );
            }
            else
            {
                ReadSerializedDynamicTypeFromJSON( m_typeRegistry, m_document, outType );
            }
        }
    }
}