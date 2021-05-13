#include "TypeSerialization.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/Core/Logging/Log.h"
#include "Tools/Core/TypeSystem/TypeInstanceModel.h"

//-------------------------------------------------------------------------
// Descriptors
//-------------------------------------------------------------------------

namespace KRG::TypeSystem::Serialization
{
    // Type descriptor reader needs to support both nested and unnested formats as it needs to read the outputs from both descriptor serialization and type model serialization
    struct TypeDescriptorReader
    {
        static bool ReadArrayDescriptor( TypeRegistry const& typeRegistry, rapidjson::Value const& arrayValue, TInlineVector<PropertyDescriptor, 6>& outPropertyValues, String const& propertyPathPrefix )
        {
            KRG_ASSERT( arrayValue.IsArray() );

            int32 const numElements = (int32) arrayValue.Size();
            for ( int32 i = 0; i < numElements; i++ )
            {
                if ( arrayValue[i].IsArray() )
                {
                    // We dont support arrays of arrays
                    KRG_LOG_ERROR( "TypeSystem", "We dont support arrays of arrays" );
                    return false;
                }
                else if ( arrayValue[i].IsObject() )
                {
                    String const newPrefix = String().sprintf( "%s%d/", propertyPathPrefix.c_str(), i );
                    if ( !ReadTypeDescriptor( typeRegistry, arrayValue[i], outPropertyValues, newPrefix ) )
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

        static bool ReadTypeDescriptor( TypeRegistry const& typeRegistry, rapidjson::Value const& typeObjectValue, TInlineVector<PropertyDescriptor, 6>& outPropertyValues, String const& propertyPathPrefix = String() )
        {
            auto const typeIDIter = typeObjectValue.FindMember( Constants::s_typeID );
            if ( typeIDIter == typeObjectValue.MemberEnd() )
            {
                KRG_LOG_ERROR( "TypeSystem", "Missing typeID for object" );
                return false;
            }

            // Get type info
            //-------------------------------------------------------------------------

            TypeID const typeID( typeIDIter->value.GetString() );
            auto const pTypeInfo = typeRegistry.GetTypeInfo( typeID );
            if ( pTypeInfo == nullptr )
            {
                KRG_LOG_ERROR( "TypeSystem", "Unknown type encountered: %s", typeID.c_str() );
                return false;
            }

            // Read properties
            //-------------------------------------------------------------------------

            for ( auto itr = typeObjectValue.MemberBegin(); itr != typeObjectValue.MemberEnd(); ++itr )
            {
                StringID const propertyID( itr->name.GetString() );
                if ( !pTypeInfo->GetPropertyInfo( propertyID ) )
                {
                    continue;
                }

                if ( itr->value.IsArray() )
                {
                    String const newPrefix = String().sprintf( "%s%s/", propertyPathPrefix.c_str(), itr->name.GetString() );
                    ReadArrayDescriptor( typeRegistry, itr->value, outPropertyValues, newPrefix );
                }
                else if ( itr->value.IsObject() )
                {
                    String const newPrefix = String().sprintf( "%s%s/", propertyPathPrefix.c_str(), itr->name.GetString() );
                    if ( !ReadTypeDescriptor( typeRegistry, itr->value, outPropertyValues, newPrefix ) )
                    {
                        return false;
                    }
                }
                else // Regular core type property
                {
                    if ( !itr->value.IsString() )
                    {
                        KRG_LOG_ERROR( "TypeSystem", "Core type values must be strings, invalid value detected: %s", itr->name.GetString() );
                        return false;
                    }

                    PropertyPath const propertyPath( String().sprintf( "%s%s", propertyPathPrefix.c_str(), itr->name.GetString() ) );

                    auto pPropertyInfo = typeRegistry.ResolvePropertyPath( pTypeInfo, propertyPath );
                    if ( pPropertyInfo != nullptr )
                    {
                        outPropertyValues.push_back( PropertyDescriptor( typeRegistry, propertyPath, *pPropertyInfo, itr->value.GetString() ) );
                    }
                }
            }

            return true;
        }
    };

    bool ReadTypeDescriptor( TypeRegistry const& typeRegistry, rapidjson::Value const& typeObjectValue, TypeDescriptor& outDesc )
    {
        if ( !typeObjectValue.IsObject() )
        {
            KRG_LOG_ERROR( "TypeSystem", "Supplied json value is not an object" );
            return false;
        }

        auto const typeIDIter = typeObjectValue.FindMember( Constants::s_typeID );
        if ( typeIDIter == typeObjectValue.MemberEnd() )
        {
            KRG_LOG_ERROR( "TypeSystem", "Missing typeID for object" );
            return false;
        }

        outDesc.m_typeID = TypeID( typeIDIter->value.GetString() );
        return TypeDescriptorReader::ReadTypeDescriptor( typeRegistry, typeObjectValue, outDesc.m_properties );
    }

    //-------------------------------------------------------------------------

    // Type descriptor serialization will collapse all properties into a single list per type, using the property paths as property names 
    struct TypeDescriptorWriter
    {
        static void WriteProperty( RapidJsonWriter& writer, PropertyDescriptor const& propertyDesc )
        {
            writer.Key( propertyDesc.m_path.ToString().c_str() );
            writer.Key( propertyDesc.m_stringValue.c_str() );
        }

        static void WriteStructure( RapidJsonWriter& writer, TypeDescriptor const& typeDesc )
        {
            writer.StartObject();

            // Every type has to have a type ID
            writer.Key( Constants::s_typeID );
            writer.String( typeDesc.m_typeID.c_str() );

            // Write all property values
            for ( auto const& propertyValue : typeDesc.m_properties )
            {
                WriteProperty( writer, propertyValue );
            }

            writer.EndObject();
        }
    };

    void WriteTypeDescriptor( TypeRegistry const& typeRegistry, RapidJsonWriter& writer, TypeDescriptor const& typeDesc )
    {
        KRG_ASSERT( typeDesc.IsValid() );
        TypeDescriptorWriter::WriteStructure( writer, typeDesc );
    }

    //-------------------------------------------------------------------------

    struct NativeTypeDescriber
    {
        static void DescribeType( TypeRegistry const& typeRegistry, TypeDescriptor& typeDesc, TypeID typeID, void const* pTypeInstance, PropertyPath& path )
        {
            KRG_ASSERT( !IsCoreType( typeID ) );
            auto const pTypeInfo = typeRegistry.GetTypeInfo( typeID );
            KRG_ASSERT( pTypeInfo != nullptr );

            //-------------------------------------------------------------------------

            for ( auto const& propInfo : pTypeInfo->m_properties )
            {
                if ( propInfo.IsArrayProperty() )
                {
                    size_t const elementByteSize = propInfo.m_arrayElementSize;
                    size_t const numArrayElements = propInfo.IsStaticArrayProperty() ? propInfo.m_arraySize : pTypeInfo->m_pTypeHelper->GetArraySize( pTypeInstance, propInfo.m_ID );
                    Byte const* pArrayElementAddress = pTypeInfo->m_pTypeHelper->GetArrayElementDataPtr( const_cast<void*>( pTypeInstance ), propInfo.m_ID, 0 );

                    // Write array elements
                    for ( auto i = 0; i < numArrayElements; i++ )
                    {
                        path.Append( propInfo.m_ID, i );
                        DescribeProperty( typeRegistry, typeDesc, pTypeInfo, pTypeInstance, propInfo, pArrayElementAddress, path, i );
                        pArrayElementAddress += elementByteSize;
                        path.RemoveLastElement();
                    }
                }
                else
                {
                    path.Append( propInfo.m_ID );
                    auto pPropertyInstance = propInfo.GetPropertyAddress( pTypeInstance );
                    DescribeProperty( typeRegistry, typeDesc, pTypeInfo, pTypeInstance, propInfo, pPropertyInstance, path );
                    path.RemoveLastElement();
                }
            }
        }

        static void DescribeProperty( TypeRegistry const& typeRegistry, TypeDescriptor& typeDesc, TypeInfo const* pParentTypeInfo, void const* pParentInstance, PropertyInfo const& propertyInfo, void const* pPropertyInstance, PropertyPath& path, int32 arrayElementIdx = InvalidIndex )
        {
            if ( IsCoreType( propertyInfo.m_typeID ) || propertyInfo.IsEnumProperty() )
            {
                // Only describe non-default properties
                if ( !pParentTypeInfo->m_pTypeHelper->IsPropertyValueSetToDefault( pParentInstance, propertyInfo.m_ID, arrayElementIdx ) )
                {
                    PropertyDescriptor& propertyDesc = typeDesc.m_properties.emplace_back( PropertyDescriptor() );
                    propertyDesc.m_path = path;
                    Conversion::ConvertNativeTypeToBinary( typeRegistry, propertyInfo, pPropertyInstance, propertyDesc.m_byteValue );
                }
            }
            else
            {
                DescribeType( typeRegistry, typeDesc, propertyInfo.m_typeID, pPropertyInstance, path );
            }
        }
    };

    void CreateTypeDescriptorFromNativeType( TypeRegistry const& typeRegistry, TypeID const& typeID, void const* pTypeInstance, TypeDescriptor& outDesc )
    {
        outDesc.m_typeID = typeID;
        outDesc.m_properties.clear();
        PropertyPath path;
        NativeTypeDescriber::DescribeType( typeRegistry, outDesc, typeID, pTypeInstance, path );
    }
}

//-------------------------------------------------------------------------
// Models
//-------------------------------------------------------------------------

namespace KRG::TypeSystem::Serialization
{
    bool ReadTypeModel( TypeRegistry const& typeRegistry, rapidjson::Value const& typeObjectValue, TypeInstanceModel& outType )
    {
        if ( !typeObjectValue.IsObject() )
        {
            KRG_LOG_ERROR( "TypeSystem", "Supplied json value is not an object" );
            return false;
        }

        if ( !typeObjectValue.HasMember( Constants::s_typeID ) )
        {
            KRG_LOG_ERROR( "TypeSystem", "Missing typeID for object" );
            return false;
        }

        TypeDescriptor typeDesc;
        if ( !ReadTypeDescriptor( typeRegistry, typeObjectValue, typeDesc ) )
        {
            return false;
        }

        outType = TypeInstanceModel( typeRegistry, typeDesc );
        return true;
    }

    //-------------------------------------------------------------------------

    // Type model serialization will not collapse all properties into a single list per type, this will result in a nested output
    struct TypeModelWriter
    {
        static void WriteProperty( RapidJsonWriter& writer, PropertyInstanceModel const& propertyModel )
        {
            if ( propertyModel.IsStructure() )
            {
                WriteStructure( writer, propertyModel );
            }
            else if ( propertyModel.IsArray() )
            {
                WriteArray( writer, propertyModel );
            }
            else
            {
                WriteCoreType( writer, propertyModel );
            }
        }

        static void WriteArray( RapidJsonWriter& writer, PropertyInstanceModel const& propertyModel )
        {
            if ( propertyModel.IsDefaultValue() )
            {
                return;
            }

            //-------------------------------------------------------------------------

            writer.Key( propertyModel.GetName() );

            writer.StartArray();

            for ( auto const& childProperty : propertyModel.GetProperties() )
            {
                if ( childProperty.IsStructure() || childProperty.IsArray() )
                {
                    WriteProperty( writer, childProperty );
                }
                else
                {
                    writer.Key( childProperty.GetStringValue().c_str() );
                }
            }

            writer.EndArray();
        }

        static void WriteCoreType( RapidJsonWriter& writer, PropertyInstanceModel const& propertyModel )
        {
            if ( !propertyModel.IsDefaultValue() )
            {
                writer.Key( propertyModel.GetName() );
                writer.Key( propertyModel.GetStringValue().c_str() );
            }
        }

        static void WriteStructure( RapidJsonWriter& writer, PropertyInstanceModel const& propertyModel )
        {
            KRG_ASSERT( propertyModel.IsStructure() );

            writer.StartObject();

            // Every type has to have a type ID
            writer.Key( Constants::s_typeID );
            writer.String( propertyModel.GetStructureTypeName() );

            if ( !propertyModel.IsDefaultValue() )
            {
                // Write all property values
                for ( auto const& propertyValue : propertyModel.GetProperties() )
                {
                    WriteProperty( writer, propertyValue );
                }
            }

            writer.EndObject();
        }
    };

    void WriteTypeModel( TypeRegistry const& typeRegistry, RapidJsonWriter& writer, TypeInstanceModel const& type )
    {
        KRG_ASSERT( type.IsValid() );
        TypeModelWriter::WriteStructure( writer, *type.GetAsPropertyInstance() );
    }
}

//-------------------------------------------------------------------------
// Native
//-------------------------------------------------------------------------

namespace KRG::TypeSystem::Serialization
{
    struct NativeTypeReader
    {
        template<typename T>
        static void SetPropertyValue( void* pAddress, T value )
        {
            *( (T*) pAddress ) = value;
        }

        static bool ReadCoreType( TypeRegistry const& typeRegistry, PropertyInfo const& propInfo, rapidjson::Value const& typeValue, void* pPropertyDataAddress )
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
                    KRG_LOG_ERROR( "TypeSystem", "Invalid JSON file encountered" );
                    return false;
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
                    KRG_LOG_ERROR( "TypeSystem", "Invalid JSON file encountered" );
                    return false;
                }
            }
            else // Invalid JSON data encountered
            {
                KRG_LOG_ERROR( "TypeSystem", "Invalid JSON file encountered" );
                return false;
            }

            return true;
        }

        //-------------------------------------------------------------------------

        static bool ReadType( TypeRegistry const& typeRegistry, rapidjson::Value const& currentJsonValue, TypeID typeID, void* pTypeData )
        {
            KRG_ASSERT( !IsCoreType( typeID ) );

            auto const pTypeInfo = typeRegistry.GetTypeInfo( typeID );
            if ( pTypeInfo == nullptr )
            {
                KRG_LOG_ERROR( "TypeSystem", "Unknown type encountered: %s", typeID.c_str() );
                return false;
            }

            KRG_ASSERT( currentJsonValue.IsObject() );
            auto const typeIDValueIter = currentJsonValue.FindMember( Constants::s_typeID );
            KRG_ASSERT( typeIDValueIter != currentJsonValue.MemberEnd() );
            TypeID const actualTypeID( typeIDValueIter->value.GetString() );

            // If you hit this the type in the JSON file and the type you are trying to deserialize do not match
            if ( typeID != actualTypeID )
            {
                KRG_LOG_ERROR( "TypeSystem", "Type mismatch, expected %s, encountered %s", typeID.c_str(), actualTypeID.c_str() );
                return false;
            }

            //-------------------------------------------------------------------------

            for ( auto const& propInfo : pTypeInfo->m_properties )
            {
                // Read Arrays
                auto pPropertyDataAddress = propInfo.GetPropertyAddress( pTypeData );
                if ( propInfo.IsArrayProperty() )
                {
                    // Try get serialized value
                    const char* pPropertyName = propInfo.m_ID.ToString();
                    auto const propertyNameIter = currentJsonValue.FindMember( pPropertyName );
                    if ( propertyNameIter == currentJsonValue.MemberEnd() )
                    {
                        continue;
                    }

                    KRG_ASSERT( propertyNameIter->value.IsArray() );
                    auto jsonArrayValue = propertyNameIter->value.GetArray();

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
                            KRG_LOG_ERROR( "TypeSystem", "Static array size mismatch for %s, expected maximum %d elements, encountered %d elements", propInfo.m_size, propInfo.m_size, (int32) numArrayElements );
                            return false;
                        }

                        Byte* pArrayElementAddress = reinterpret_cast<Byte*>( pPropertyDataAddress );
                        for ( auto i = 0; i < numArrayElements; i++ )
                        {
                            if ( !ReadProperty( typeRegistry, jsonArrayValue[i], propInfo, pArrayElementAddress ) )
                            {
                                return false;
                            }
                            pArrayElementAddress += propInfo.m_arrayElementSize;
                        }
                    }
                    else // Dynamic array
                    {
                        // Do the traversal backwards to only allocate once
                        for ( int32 i = (int32) ( numArrayElements - 1 ); i >= 0; i-- )
                        {
                            auto pArrayElementAddress = pTypeInfo->m_pTypeHelper->GetArrayElementDataPtr( pTypeData, propInfo.m_ID, i );
                            if ( !ReadProperty( typeRegistry, jsonArrayValue[i], propInfo, pArrayElementAddress ) )
                            {
                                return false;
                            }
                        }
                    }
                }
                else // Non-array type
                {
                    // Try get serialized value
                    const char* pPropertyName = propInfo.m_ID.ToString();
                    auto const propertyValueIter = currentJsonValue.FindMember( pPropertyName );
                    if ( propertyValueIter == currentJsonValue.MemberEnd() )
                    {
                        continue;
                    }

                    if ( !ReadProperty( typeRegistry, propertyValueIter->value, propInfo, pPropertyDataAddress ) )
                    {
                        return false;
                    }
                }
            }

            return true;
        }

        static bool ReadProperty( TypeRegistry const& typeRegistry, rapidjson::Value const& currentJsonValue, PropertyInfo const& propertyInfo, void* pTypeData )
        {
            if ( IsCoreType( propertyInfo.m_typeID ) || propertyInfo.IsEnumProperty() )
            {
                String const typeName = propertyInfo.m_typeID.c_str();
                return ReadCoreType( typeRegistry, propertyInfo, currentJsonValue, pTypeData );
            }
            else // Complex Type
            {
                return ReadType( typeRegistry, currentJsonValue, propertyInfo.m_typeID, pTypeData );
            }
        }
    };

    bool ReadNativeType( TypeRegistry const& typeRegistry, rapidjson::Value const& typeObjectValue, TypeID const& typeID, void* pTypeInstance )
    {
        if ( !typeObjectValue.IsObject() )
        {
            KRG_LOG_ERROR( "TypeSystem", "Supplied json value is not an object" );
            return false;
        }

        if ( !typeObjectValue.HasMember( Constants::s_typeID ) )
        {
            KRG_LOG_ERROR( "TypeSystem", "Missing typeID for object" );
            return false;
        }

        return NativeTypeReader::ReadType( typeRegistry, typeObjectValue, typeID, pTypeInstance );
    }

    //-------------------------------------------------------------------------

    struct NativeTypeWriter
    {
        static void WriteType( TypeRegistry const& typeRegistry, RapidJsonWriter& writer, String& scratchBuffer, TypeID typeID, void const* pTypeData )
        {
            KRG_ASSERT( !IsCoreType( typeID ) );
            auto const pTypeInfo = typeRegistry.GetTypeInfo( typeID );
            KRG_ASSERT( pTypeInfo != nullptr );

            writer.StartObject();

            // Every type has to have a type ID
            writer.Key( Constants::s_typeID );
            writer.String( typeID.c_str() );

            //-------------------------------------------------------------------------

            for ( auto const& propInfo : pTypeInfo->m_properties )
            {
                // Write Key
                const char* pPropertyName = propInfo.m_ID.ToString();
                KRG_ASSERT( pPropertyName != nullptr );
                writer.Key( pPropertyName );

                // Write Value
                auto pPropertyDataAddress = propInfo.GetPropertyAddress( pTypeData );
                if ( propInfo.IsArrayProperty() )
                {
                    size_t const elementByteSize = typeRegistry.GetTypeByteSize( propInfo.m_typeID );
                    KRG_ASSERT( elementByteSize > 0 );

                    writer.StartArray();

                    size_t numArrayElements = 0;
                    Byte const* pElementAddress = nullptr;

                    // Static array
                    if ( propInfo.IsStaticArrayProperty() )
                    {
                        numArrayElements = propInfo.m_size / elementByteSize;
                        pElementAddress = propInfo.GetPropertyAddress<Byte>( pTypeData );
                    }
                    else // Dynamic array
                    {
                        TVector<Byte> const& dynamicArray = *propInfo.GetPropertyAddress< TVector<Byte> >( pTypeData );
                        size_t const arrayByteSize = dynamicArray.size();
                        numArrayElements = arrayByteSize / elementByteSize;
                        pElementAddress = dynamicArray.data();
                    }

                    // Write array elements
                    for ( auto i = 0; i < numArrayElements; i++ )
                    {
                        WriteProperty( typeRegistry, writer, scratchBuffer, propInfo, pElementAddress );
                        pElementAddress += elementByteSize;
                    }

                    writer.EndArray();
                }
                else
                {
                    WriteProperty( typeRegistry, writer, scratchBuffer, propInfo, pPropertyDataAddress );
                }
            }

            writer.EndObject();
        }

        static void WriteProperty( TypeRegistry const& typeRegistry, RapidJsonWriter& writer, String& scratchBuffer, PropertyInfo const& propertyInfo, void const* pTypeData )
        {
            if ( IsCoreType( propertyInfo.m_typeID ) || propertyInfo.IsEnumProperty() )
            {
                Conversion::ConvertNativeTypeToString( typeRegistry, propertyInfo, pTypeData, scratchBuffer );
                writer.String( scratchBuffer.c_str() );
            }
            else
            {
                WriteType( typeRegistry, writer, scratchBuffer, propertyInfo.m_typeID, pTypeData );
            }
        }
    };

    void WriteNativeType( TypeRegistry const& typeRegistry, RapidJsonWriter& writer, TypeID const& typeID, void const* pTypeInstance )
    {
        String scratchBuffer;
        scratchBuffer.reserve( 255 );
        NativeTypeWriter::WriteType( typeRegistry, writer, scratchBuffer, typeID, pTypeInstance );
    }
}