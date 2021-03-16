#if _WIN32
#include "NativeTypeReader.h"
#include "TypeSerializationCommon.h"
#include "System/TypeSystem/CoreTypeConversions.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/ITypeHelper.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    namespace
    {
        template<typename T>
        void SetPropertyValue( void* pAddress, T value )
        {
            *( (T*) pAddress ) = value;
        }

        static void ReadCoreType( TypeRegistry const& typeRegistry, PropertyInfo const& propInfo, rapidjson::Value const& typeValue, void* pPropertyDataAddress )
        {
            KRG_ASSERT( pPropertyDataAddress != nullptr );

            if ( typeValue.IsString() )
            {
                String const valueString = String( typeValue.GetString() );
                Conversion::ConvertStringToNativeType( typeRegistry, propInfo, valueString, pPropertyDataAddress );
            }
            else if ( typeValue.IsBool() )
            {
                KRG_ASSERT( propInfo.m_typeID == CoreTypes::Bool );
                SetPropertyValue( pPropertyDataAddress, typeValue.GetBool() );
            }
            else if ( typeValue.IsInt64() || typeValue.IsUint64() )
            {
                if ( propInfo.m_typeID == CoreTypes::Uint8 )
                {
                    SetPropertyValue( pPropertyDataAddress, (uint8) typeValue.GetUint64() );
                }
                else if ( propInfo.m_typeID == CoreTypes::Int8 )
                {
                    SetPropertyValue( pPropertyDataAddress, (int8) typeValue.GetInt64() );
                }
                else if ( propInfo.m_typeID == CoreTypes::Uint16 )
                {
                    SetPropertyValue( pPropertyDataAddress, (uint16) typeValue.GetUint64() );
                }
                else if ( propInfo.m_typeID == CoreTypes::Int16 )
                {
                    SetPropertyValue( pPropertyDataAddress, (int16) typeValue.GetInt64() );
                }
                else if ( propInfo.m_typeID == CoreTypes::Uint32 )
                {
                    SetPropertyValue( pPropertyDataAddress, (uint32) typeValue.GetUint64() );
                }
                else if ( propInfo.m_typeID == CoreTypes::Int32 )
                {
                    SetPropertyValue( pPropertyDataAddress, (int32) typeValue.GetInt64() );
                }
                else if ( propInfo.m_typeID == CoreTypes::Uint64 )
                {
                    SetPropertyValue( pPropertyDataAddress, typeValue.GetUint64() );
                }
                else if ( propInfo.m_typeID == CoreTypes::Int64 )
                {
                    SetPropertyValue( pPropertyDataAddress, typeValue.GetInt64() );
                }
                else // Invalid JSON data encountered
                {
                    KRG_HALT();
                }
            }
            else if ( typeValue.IsDouble() )
            {
                if ( propInfo.m_typeID == CoreTypes::Float )
                {
                    SetPropertyValue( pPropertyDataAddress, typeValue.GetFloat() );
                }
                else if ( propInfo.m_typeID == CoreTypes::Double )
                {
                    SetPropertyValue( pPropertyDataAddress, typeValue.GetDouble() );
                }
                else // Invalid JSON data encountered
                {
                    KRG_HALT();
                }
            }
            else // Invalid JSON data encountered
            {
                KRG_HALT();
            }
        }
    }

    //-------------------------------------------------------------------------

    NativeTypeReader::NativeTypeReader( TypeRegistry const& typeRegistry )
        : m_typeRegistry( typeRegistry )
    {}

    NativeTypeReader::~NativeTypeReader()
    {
        Reset();
    }

    bool NativeTypeReader::ReadFromFile( FileSystemPath const& filePath )
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

            bool const isValidJsonFile = ( m_document.GetParseError() == rapidjson::kParseErrorNone );
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

    void NativeTypeReader::Reset()
    {
        m_document.Clear();
        m_numSerializedTypes = 0;
        m_deserializedTypeIdx = 0;

        if ( m_pFileBuffer != nullptr )
        {
            KRG::Free( m_pFileBuffer );
        }
    }

    rapidjson::Value const& NativeTypeReader::GetValueToBeDeserialized() const
    {
        KRG_ASSERT( m_deserializedTypeIdx < m_numSerializedTypes );

        if ( m_document.IsArray() )
        {
            KRG_ASSERT( m_document.IsArray() );
            return m_document[m_deserializedTypeIdx++];
        }
        else
        {
            return m_document;
        }
    }

    void NativeTypeReader::DeserializeType( rapidjson::Value const& currentJsonValue, TypeID typeID, void* pTypeData ) const
    {
        KRG_ASSERT( !IsCoreType( typeID ) );

        auto const pTypeInfo = m_typeRegistry.GetTypeInfo( typeID );
        KRG_ASSERT( pTypeInfo != nullptr );

        KRG_ASSERT( currentJsonValue.IsObject() && currentJsonValue.HasMember( Serialization::TypeSerialization::Key_TypeID ) );
        TypeID const readTypeID( currentJsonValue[Serialization::TypeSerialization::Key_TypeID].GetString() );

        // If you hit this the type in the JSON file and the type you are trying to deserialize do not match
        KRG_ASSERT( readTypeID == typeID );

        //-------------------------------------------------------------------------

        for ( auto const& propInfoPair : pTypeInfo->m_properties )
        {
            PropertyInfo const& propInfo = propInfoPair.second;

            // Try get serialized value
            const char* pPropertyName = propInfo.m_ID.ToString();
            if ( !currentJsonValue.HasMember( pPropertyName ) )
            {
                continue;
            }

            // Read value
            auto pPropertyDataAddress = propInfo.GetPropertyAddress( pTypeData );
            if ( propInfo.IsArrayProperty() )
            {
                KRG_ASSERT( currentJsonValue[pPropertyName].IsArray() );
                auto jsonArrayValue = currentJsonValue[pPropertyName].GetArray();

                // Skip empty arrays
                size_t const numArrayElements = jsonArrayValue.Size();
                if ( numArrayElements == 0 )
                {
                    continue;
                }

                // Static array
                if ( propInfo.IsStaticArrayProperty() )
                {
                    if ( propInfo.m_size < numArrayElements )
                    {
                        KRG_LOG_WARNING( "Resource", "Static array size mismatch for %s, expected maximum %d elements, encountered %d elements", propInfo.m_size, propInfo.m_size, (int32) numArrayElements );
                        continue;
                    }

                    size_t const elementByteSize = m_typeRegistry.GetTypeByteSize( propInfo.m_typeID );
                    KRG_ASSERT( elementByteSize > 0 );

                    Byte* pArrayElementAddress = reinterpret_cast<Byte*>( pPropertyDataAddress );
                    for ( auto i = 0; i < numArrayElements; i++ )
                    {
                        DeserializeProperty( jsonArrayValue[i], propInfo, pArrayElementAddress );
                        pArrayElementAddress += elementByteSize;
                    }
                }
                else
                {
                    // Do the traversal backwards to only allocate once
                    for ( int32 i = (int32) ( numArrayElements - 1 ); i >= 0; i-- )
                    {
                        auto pArrayElementAddress = pTypeInfo->m_pTypeHelper->GetDynamicArrayElementDataPtr( pTypeData, propInfo.m_ID, i );
                        DeserializeProperty( jsonArrayValue[i], propInfo, pArrayElementAddress );
                    }
                }
            }
            else // Complex Type
            {
                DeserializeProperty( currentJsonValue[pPropertyName], propInfo, pPropertyDataAddress );
            }
        }
    }

    void NativeTypeReader::DeserializeProperty( rapidjson::Value const& currentJsonValue, PropertyInfo const& propertyInfo, void* pTypeData ) const
    {
        if ( IsCoreType( propertyInfo.m_typeID ) )
        {
            String const typeName = propertyInfo.m_typeID.GetAsStringID().ToString();
            KRG_ASSERT( IsCoreType( propertyInfo.m_typeID ) );
            ReadCoreType( m_typeRegistry, propertyInfo, currentJsonValue, pTypeData );
        }
        else // Complex Type
        {
            DeserializeType( currentJsonValue, propertyInfo.m_typeID, pTypeData );
        }
    }
}

#endif