#include "TypeInstanceModelReader.h"
#include "TypeSerializationCommon.h"
#include "Tools/Core/TypeSystem/TypeInstanceModel.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/ITypeHelper.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

using namespace rapidjson;

//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    TypeInstanceModelReader::TypeInstanceModelReader( TypeRegistry const& typeRegistry )
        : m_typeRegistry( typeRegistry )
    {}

    TypeInstanceModelReader::~TypeInstanceModelReader()
    {
        Reset();
    }

    bool TypeInstanceModelReader::ReadFromFile( FileSystem::Path const& filePath )
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

    void TypeInstanceModelReader::Reset()
    {
        m_document.Clear();
        m_numSerializedTypes = 0;
        m_deserializedTypeIdx = 0;

        if ( m_pFileBuffer != nullptr )
        {
            KRG::Free( m_pFileBuffer );
        }
    }

    //-------------------------------------------------------------------------

    bool TypeInstanceModelReader::DeserializeType( rapidjson::Value const& jsonValue, TVector<PropertyInstanceDescriptor>& outPropertyValues, String const& propertyPathPrefix ) const
    {
        if ( !jsonValue.HasMember( Serialization::TypeSerialization::Key_TypeID ) )
        {
            return false;
        }

        // Get type info
        //-------------------------------------------------------------------------

        auto const typeID = TypeID( jsonValue[Serialization::TypeSerialization::Key_TypeID].GetString() );
        auto const pTypeInfo = m_typeRegistry.GetTypeInfo( typeID );
        if ( pTypeInfo == nullptr )
        {
            return false;
        }

        // Read properties
        //-------------------------------------------------------------------------

        for ( auto itr = jsonValue.MemberBegin(); itr != jsonValue.MemberEnd(); ++itr )
        {
            StringID const propertyID( itr->name.GetString() );
            if ( !pTypeInfo->GetPropertyInfo( propertyID ) )
            {
                continue;
            }

            if ( itr->value.IsArray() )
            {
                String const newPrefix = String().sprintf( "%s%s/", propertyPathPrefix.c_str(), itr->name.GetString() );
                DeserializePropertyArray( itr->value, outPropertyValues, newPrefix );
            }
            else if ( itr->value.IsObject() )
            {
                String const newPrefix = String().sprintf( "%s%s/", propertyPathPrefix.c_str(), itr->name.GetString() );
                if ( !DeserializeType( itr->value, outPropertyValues, newPrefix ) )
                {
                    return false;
                }
            }
            else // Regular core type property
            {
                String const propertyPath = String().sprintf( "%s%s", propertyPathPrefix.c_str(), itr->name.GetString() );
                outPropertyValues.push_back( { PropertyPath( propertyPath ), itr->value.GetString() } );
            }
        }

        return true;
    }

    bool TypeInstanceModelReader::DeserializePropertyArray( rapidjson::Value const& arrayValue, TVector<PropertyInstanceDescriptor>& outPropertyValues, String const& propertyPathPrefix ) const
    {
        KRG_ASSERT( arrayValue.IsArray() );

        int32 const numElements = (int32) arrayValue.Size();
        for ( int32 i = 0; i < numElements; i++ )
        {
            if ( arrayValue[i].IsArray() )
            {
                // We dont support arrays of arrays
                return false;
            }
            else if ( arrayValue[i].IsObject() )
            {
                String const newPrefix = String().sprintf( "%s%d/", propertyPathPrefix.c_str(), i );
                if ( !DeserializeType( arrayValue[i], outPropertyValues, newPrefix ) )
                {
                    return false;
                }
            }
            else // Add regular property value
            {
                String const propertyPath = String().sprintf( "%s%d", propertyPathPrefix.c_str(), i );
                outPropertyValues.push_back( { PropertyPath( propertyPath ), arrayValue[i].GetString() } );
            }
        }

        return true;
    }

    bool TypeInstanceModelReader::DeserializeType( TypeInstanceModel& outType ) const
    {
        KRG_ASSERT( m_deserializedTypeIdx < m_numSerializedTypes );

        rapidjson::Value const& jsonObject = ( m_document.IsArray() ) ? m_document[m_deserializedTypeIdx++] : m_document;

        //-------------------------------------------------------------------------

        if ( jsonObject.IsObject() && jsonObject.HasMember( Serialization::TypeSerialization::Key_TypeID ) )
        {
            TypeInstanceDescriptor serializedDynamicType;
            if ( DeserializeType( jsonObject, serializedDynamicType.m_properties, String() ) )
            {
                serializedDynamicType.m_typeID = jsonObject[Serialization::TypeSerialization::Key_TypeID].GetString();
                outType = TypeInstanceModel( m_typeRegistry, serializedDynamicType );
                return true;
            }
        }
        else
        {
            KRG_LOG_ERROR( "TypeSystem", "Failed to deserialize type since the json file is malformed" );
        }

        outType = TypeInstanceModel();
        return false;
    }
}