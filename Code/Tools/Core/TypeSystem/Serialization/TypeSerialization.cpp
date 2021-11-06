#include "TypeSerialization.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------
// Descriptors
//-------------------------------------------------------------------------

namespace KRG::TypeSystem::Serialization
{
    // Type descriptor reader needs to support both nested and unnested formats as it needs to read the outputs from both descriptor serialization and type model serialization
    struct TypeDescriptorReader
    {
        static bool ReadArrayDescriptor( TypeRegistry const& typeRegistry, TypeInfo const* pTypeInfo, RapidJsonValue const& arrayValue, TInlineVector<PropertyDescriptor, 6>& outPropertyValues, String const& propertyPathPrefix )
        {
            KRG_ASSERT( pTypeInfo != nullptr && arrayValue.IsArray() );

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
                    if ( !ReadTypeDescriptor( typeRegistry, pTypeInfo, arrayValue[i], outPropertyValues, newPrefix ) )
                    {
                        return false;
                    }
                }
                else // Add regular property value
                {
                    auto const propertyPath = PropertyPath( String().sprintf( "%s%d", propertyPathPrefix.c_str(), i ) );
                    auto pPropertyInfo = typeRegistry.ResolvePropertyPath( pTypeInfo, propertyPath );
                    if ( pPropertyInfo != nullptr )
                    {
                        outPropertyValues.push_back( PropertyDescriptor( typeRegistry, propertyPath, *pPropertyInfo, arrayValue[i].GetString() ) );
                    }
                }
            }

            return true;
        }

        static bool ReadTypeDescriptor( TypeRegistry const& typeRegistry, TypeInfo const* pTypeInfo, RapidJsonValue const& typeObjectValue, TInlineVector<PropertyDescriptor, 6>& outPropertyValues, String const& propertyPathPrefix = String() )
        {
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
                    ReadArrayDescriptor( typeRegistry, pTypeInfo, itr->value, outPropertyValues, newPrefix );
                }
                else if ( itr->value.IsObject() )
                {
                    String const newPrefix = String().sprintf( "%s%s/", propertyPathPrefix.c_str(), itr->name.GetString() );
                    if ( !ReadTypeDescriptor( typeRegistry, pTypeInfo, itr->value, outPropertyValues, newPrefix ) )
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

    bool ReadTypeDescriptor( TypeRegistry const& typeRegistry, RapidJsonValue const& typeObjectValue, TypeDescriptor& outDesc )
    {
        if ( !typeObjectValue.IsObject() )
        {
            KRG_LOG_ERROR( "TypeSystem", "Supplied json value is not an object" );
            return false;
        }

        // Get type info
        //-------------------------------------------------------------------------

        auto const typeIDIter = typeObjectValue.FindMember( Constants::s_typeID );
        if ( typeIDIter == typeObjectValue.MemberEnd() )
        {
            KRG_LOG_ERROR( "TypeSystem", "Missing typeID for object" );
            return false;
        }

        TypeID const typeID( typeIDIter->value.GetString() );
        auto const pTypeInfo = typeRegistry.GetTypeInfo( typeID );
        if ( pTypeInfo == nullptr )
        {
            KRG_LOG_ERROR( "TypeSystem", "Unknown type encountered: %s", typeID.c_str() );
            return false;
        }

        // Read descriptor
        //-------------------------------------------------------------------------

        outDesc.m_typeID = TypeID( typeIDIter->value.GetString() );
        return TypeDescriptorReader::ReadTypeDescriptor( typeRegistry, pTypeInfo, typeObjectValue, outDesc.m_properties );
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
        static void DescribeType( TypeRegistry const& typeRegistry, TypeDescriptor& typeDesc, TypeID typeID, IRegisteredType const* pTypeInstance, PropertyPath& path, bool setStringValues )
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
                    Byte const* pArrayElementAddress = pTypeInfo->m_pTypeHelper->GetArrayElementDataPtr( const_cast<IRegisteredType*>( pTypeInstance ), propInfo.m_ID, 0 );

                    // Write array elements
                    for ( auto i = 0; i < numArrayElements; i++ )
                    {
                        path.Append( propInfo.m_ID, i );
                        DescribeProperty( typeRegistry, typeDesc, pTypeInfo, pTypeInstance, propInfo, setStringValues, pArrayElementAddress, path, i );
                        pArrayElementAddress += elementByteSize;
                        path.RemoveLastElement();
                    }
                }
                else
                {
                    path.Append( propInfo.m_ID );
                    auto pPropertyInstance = propInfo.GetPropertyAddress( pTypeInstance );
                    DescribeProperty( typeRegistry, typeDesc, pTypeInfo, pTypeInstance,propInfo, setStringValues, pPropertyInstance, path );
                    path.RemoveLastElement();
                }
            }
        }

        static void DescribeProperty( TypeRegistry const& typeRegistry, TypeDescriptor& typeDesc, TypeInfo const* pParentTypeInfo, IRegisteredType const* pParentInstance, PropertyInfo const& propertyInfo, bool setStringValues, void const* pPropertyInstance, PropertyPath& path, int32 arrayElementIdx = InvalidIndex )
        {
            if ( IsCoreType( propertyInfo.m_typeID ) || propertyInfo.IsEnumProperty() )
            {
                // Only describe non-default properties
                if ( !pParentTypeInfo->m_pTypeHelper->IsPropertyValueSetToDefault( pParentInstance, propertyInfo.m_ID, arrayElementIdx ) )
                {
                    PropertyDescriptor& propertyDesc = typeDesc.m_properties.emplace_back( PropertyDescriptor() );
                    propertyDesc.m_path = path;
                    Conversion::ConvertNativeTypeToBinary( typeRegistry, propertyInfo, pPropertyInstance, propertyDesc.m_byteValue );

                    if ( setStringValues )
                    {
                        Conversion::ConvertNativeTypeToString( typeRegistry, propertyInfo, pPropertyInstance, propertyDesc.m_stringValue );
                    }
                }
            }
            else
            {
                DescribeType( typeRegistry, typeDesc, propertyInfo.m_typeID, (IRegisteredType*) pPropertyInstance, path, setStringValues );
            }
        }
    };

    void CreateTypeDescriptorFromNativeType( TypeRegistry const& typeRegistry, IRegisteredType const* pTypeInstance, TypeDescriptor& outDesc, bool setStringValues )
    {
        outDesc.m_typeID = pTypeInstance->GetTypeID();
        outDesc.m_properties.clear();
        PropertyPath path;
        NativeTypeDescriber::DescribeType( typeRegistry, outDesc, outDesc.m_typeID, pTypeInstance, path, setStringValues );
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

        static bool ReadCoreType( TypeRegistry const& typeRegistry, PropertyInfo const& propInfo, RapidJsonValue const& typeValue, void* pPropertyDataAddress )
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

        static bool ReadType( TypeRegistry const& typeRegistry, RapidJsonValue const& currentJsonValue, TypeID typeID, IRegisteredType* pTypeData )
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

        static bool ReadProperty( TypeRegistry const& typeRegistry, RapidJsonValue const& currentJsonValue, PropertyInfo const& propertyInfo, void* pPropertyInstance )
        {
            if ( IsCoreType( propertyInfo.m_typeID ) || propertyInfo.IsEnumProperty() )
            {
                String const typeName = propertyInfo.m_typeID.c_str();
                return ReadCoreType( typeRegistry, propertyInfo, currentJsonValue, pPropertyInstance );
            }
            else // Complex Type
            {
                return ReadType( typeRegistry, currentJsonValue, propertyInfo.m_typeID, (IRegisteredType*) pPropertyInstance );
            }
        }
    };

    bool ReadNativeType( TypeRegistry const& typeRegistry, RapidJsonValue const& typeObjectValue, IRegisteredType* pTypeInstance )
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

        return NativeTypeReader::ReadType( typeRegistry, typeObjectValue, pTypeInstance->GetTypeID(), pTypeInstance );
    }

    //-------------------------------------------------------------------------

    struct NativeTypeWriter
    {
        static void WriteType( TypeRegistry const& typeRegistry, RapidJsonWriter& writer, String& scratchBuffer, TypeID typeID, IRegisteredType const* pTypeInstance )
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
                auto pPropertyDataAddress = propInfo.GetPropertyAddress( pTypeInstance );
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
                        pElementAddress = propInfo.GetPropertyAddress<Byte>( pTypeInstance );
                    }
                    else // Dynamic array
                    {
                        TVector<Byte> const& dynamicArray = *propInfo.GetPropertyAddress< TVector<Byte> >( pTypeInstance );
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

        static void WriteProperty( TypeRegistry const& typeRegistry, RapidJsonWriter& writer, String& scratchBuffer, PropertyInfo const& propertyInfo, void const* pPropertyInstance )
        {
            if ( IsCoreType( propertyInfo.m_typeID ) || propertyInfo.IsEnumProperty() )
            {
                Conversion::ConvertNativeTypeToString( typeRegistry, propertyInfo, pPropertyInstance, scratchBuffer );
                writer.String( scratchBuffer.c_str() );
            }
            else
            {
                WriteType( typeRegistry, writer, scratchBuffer, propertyInfo.m_typeID, (IRegisteredType*) pPropertyInstance );
            }
        }
    };

    void WriteNativeType( TypeRegistry const& typeRegistry, RapidJsonWriter& writer, IRegisteredType const* pTypeInstance )
    {
        String scratchBuffer;
        scratchBuffer.reserve( 255 );
        NativeTypeWriter::WriteType( typeRegistry, writer, scratchBuffer, pTypeInstance->GetTypeID(), pTypeInstance );
    }

    IRegisteredType* CreateAndReadNativeType( TypeRegistry const& typeRegistry, RapidJsonValue const& typeObjectValue )
    {
        auto const typeIDIter = typeObjectValue.FindMember( Constants::s_typeID );
        if ( typeIDIter == typeObjectValue.MemberEnd() )
        {
            KRG_LOG_ERROR( "TypeSystem", "Missing typeID for object" );
            return nullptr;
        }

        TypeID const typeID( typeIDIter->value.GetString() );
        auto const pTypeInfo = typeRegistry.GetTypeInfo( typeID );
        if ( pTypeInfo == nullptr )
        {
            KRG_LOG_ERROR( "TypeSystem", "Unknown type encountered: %s", typeID.c_str() );
            return nullptr;
        }

        IRegisteredType* pTypeInstance = pTypeInfo->m_pTypeHelper->CreateType();

        if ( !ReadNativeType( typeRegistry, typeObjectValue, pTypeInstance ) )
        {
            KRG::Delete( pTypeInstance );
            return nullptr;
        }

        return pTypeInstance;
    }
}