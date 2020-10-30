#if _WIN32
#include "NativeTypeReader.h"
#include "TypeSerializationCommon.h"
#include "System/TypeSystem/TypeValueConverter.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/ITypeHelper.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

using namespace rapidjson;

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

        static void ReadCoreType( TypeID const& typeID, Value const& typeValue, void* pPropertyDataAddress )
        {
            KRG_ASSERT( pPropertyDataAddress != nullptr );

            if ( typeValue.IsString() )
            {
                String const valueString = String( typeValue.GetString() );
                TypeValueConverter::ConvertStringToValue( typeID, valueString, pPropertyDataAddress );
            }
            else if ( typeValue.IsBool() )
            {
                KRG_ASSERT( typeID == CoreTypes::Bool );
                SetPropertyValue( pPropertyDataAddress, typeValue.GetBool() );
            }
            else if ( typeValue.IsInt64() || typeValue.IsUint64() )
            {
                if ( typeID == CoreTypes::F32 )
                {
                    SetPropertyValue( pPropertyDataAddress, (U8) typeValue.IsUint64() );
                }
                else if ( typeID == CoreTypes::F64 )
                {
                    SetPropertyValue( pPropertyDataAddress, (S8) typeValue.GetInt64() );
                }
                else if ( typeID == CoreTypes::F32 )
                {
                    SetPropertyValue( pPropertyDataAddress, (U16) typeValue.IsUint64() );
                }
                else if ( typeID == CoreTypes::F64 )
                {
                    SetPropertyValue( pPropertyDataAddress, (S16) typeValue.GetInt64() );
                }
                else if ( typeID == CoreTypes::F32 )
                {
                    SetPropertyValue( pPropertyDataAddress, (U32) typeValue.IsUint64() );
                }
                else if ( typeID == CoreTypes::F64 )
                {
                    SetPropertyValue( pPropertyDataAddress, (S32) typeValue.GetInt64() );
                }
                else if ( typeID == CoreTypes::F32 )
                {
                    SetPropertyValue( pPropertyDataAddress, (U64) typeValue.IsUint64() );
                }
                else if ( typeID == CoreTypes::F64 )
                {
                    SetPropertyValue( pPropertyDataAddress, (S64) typeValue.GetInt64() );
                }
                else
                {
                    KRG_HALT();
                }
            }
            else if ( typeValue.IsDouble() )
            {
                if ( typeID == CoreTypes::F32 )
                {
                    SetPropertyValue( pPropertyDataAddress, (F32) typeValue.GetDouble() );
                }
                else if ( typeID == CoreTypes::F64 )
                {
                    SetPropertyValue( pPropertyDataAddress, (F64) typeValue.GetDouble() );
                }
                else
                {
                    KRG_HALT();
                }
            }
            else
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
        String const typeName = typeID.GetAsStringID().ToString();

        if ( IsCoreType( typeID ) )
        {
            ReadCoreType( typeID, currentJsonValue, pTypeData );
        }
        else // Complex type
        {
            auto const pTypeInfo = m_typeRegistry.GetTypeInfo( typeID );
            KRG_ASSERT( pTypeInfo != nullptr );

            KRG_ASSERT( currentJsonValue.IsObject() && currentJsonValue.HasMember( Serialization::TypeSerialization::Key_TypeID ) );
            TypeID const readTypeID( currentJsonValue[Serialization::TypeSerialization::Key_TypeID].GetString() );
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
                            KRG_LOG_WARNING( "Resource", "Static array size mismatch for %s, expected maximum %d elements, encountered %d elements", propInfo.m_size, propInfo.m_size, (S32) numArrayElements );
                            continue;
                        }

                        size_t const elementByteSize = m_typeRegistry.GetTypeByteSize( propInfo.m_typeID );
                        KRG_ASSERT( elementByteSize > 0 );

                        Byte* pArrayElementAddress = reinterpret_cast<Byte*>( pPropertyDataAddress );
                        for ( auto i = 0; i < numArrayElements; i++ )
                        {
                            DeserializeType( jsonArrayValue[i], propInfo.m_typeID, pArrayElementAddress );
                            pArrayElementAddress += elementByteSize;
                        }
                    }
                    else
                    {
                        // Do the traversal backwards to only allocate once
                        for ( S32 i = (S32) ( numArrayElements - 1 ); i >= 0; i-- )
                        {
                            auto pArrayElementAddress = pTypeInfo->m_pTypeHelper->GetDynamicArrayElementDataPtr( pTypeData, propInfo.m_ID, i );
                            DeserializeType( jsonArrayValue[i], propInfo.m_typeID, pArrayElementAddress );
                        }
                    }
                }
                else if ( IsCoreType( propInfo.m_typeID ) )
                {
                    ReadCoreType( propInfo.m_typeID, currentJsonValue[pPropertyName], pPropertyDataAddress );
                }
                else // Complex Type
                {
                    DeserializeType( currentJsonValue[pPropertyName], propInfo.m_typeID, pPropertyDataAddress );
                }
            }
        }
    }
}

#endif