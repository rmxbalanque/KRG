#include "ToolTypeReader.h"
#include "TypeSerializationCommon.h"
#include "Tools/Core/TypeSystem/ToolTypeInstance.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/ITypeHelper.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

using namespace rapidjson;

//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    ToolTypeReader::ToolTypeReader( TypeRegistry const& typeRegistry )
        : m_typeRegistry( typeRegistry )
    {}

    ToolTypeReader::~ToolTypeReader()
    {
        Reset();
    }

    bool ToolTypeReader::ReadFromFile( FileSystem::Path const& filePath )
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

    void ToolTypeReader::Reset()
    {
        m_document.Clear();
        m_numSerializedTypes = 0;
        m_deserializedTypeIdx = 0;

        if ( m_pFileBuffer != nullptr )
        {
            KRG::Free( m_pFileBuffer );
        }
    }

    void ToolTypeReader::DeserializeType( ToolTypeInstance& outType ) const
    {
        KRG_ASSERT( m_deserializedTypeIdx < m_numSerializedTypes );

        rapidjson::Value const& jsonObject = ( m_numSerializedTypes > 1 ) ? m_document[m_deserializedTypeIdx++] : m_document;

        //-------------------------------------------------------------------------

        ToolTypeInstanceDescriptor serializedDynamicType;

        //-------------------------------------------------------------------------

        // Read type ID
        KRG_ASSERT( jsonObject.IsObject() && jsonObject.HasMember( Serialization::TypeSerialization::Key_TypeID ) );
        serializedDynamicType.m_typeID = TypeID( jsonObject[Serialization::TypeSerialization::Key_TypeID].GetString() );
        KRG_ASSERT( serializedDynamicType.m_typeID.IsValid() );

        // Read properties
        for ( auto itr = jsonObject.MemberBegin(); itr != jsonObject.MemberEnd(); ++itr )
        {
            if ( strcmp( itr->name.GetString(), Serialization::TypeSerialization::Key_TypeID ) == 0 )
            {
                continue;
            }

            serializedDynamicType.m_properties.push_back( { PropertyPath( itr->name.GetString() ), itr->value.GetString() } );
        }
    }
}