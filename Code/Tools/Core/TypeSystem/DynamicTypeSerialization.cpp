#include "DynamicTypeSerialization.h"
#include "DynamicTypeInstance.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/TypeSerialization.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Serialization
    {
        static void FlattenProperties( DynamicPropertyInstance const& propertyInstance, TypeSystem::PropertyPath const& currentPath, TVector<SerializedDynamicPropertyValue>& outProperties )
        {
            if ( propertyInstance.IsDefaultValue() )
            {
                return;
            }

            //-------------------------------------------------------------------------

            TypeSystem::PropertyPath propertyPath = currentPath;
            if ( propertyInstance.IsArrayElementProperty() )
            {
                propertyPath.ReplaceLastElement( propertyPath.GetLastElement().m_propertyID, propertyInstance.GetArrayElementIndex() );
            }
            else
            {
                propertyPath += propertyInstance.GetID();
            }

            //-------------------------------------------------------------------------

            if ( propertyInstance.IsArrayProperty() )
            {
                // Serialize dynamic array size
                if ( propertyInstance.IsDynamicArrayProperty() )
                {
                    if ( propertyInstance.GetNumArrayElements() != propertyInstance.GetNumDefaultArrayElements() )
                    {
                        outProperties.push_back( SerializedDynamicPropertyValue( propertyPath, String().sprintf( "%d", propertyInstance.GetNumArrayElements() ) ) );
                    }
                }

                //-------------------------------------------------------------------------

                // Flatten array elements
                for ( auto const& childProperty : propertyInstance.GetProperties() )
                {
                    FlattenProperties( childProperty, propertyPath, outProperties );
                }
            }
            else if ( propertyInstance.IsStructureProperty() )
            {
                for ( auto const& childProperty : propertyInstance.GetProperties() )
                {
                    FlattenProperties( childProperty, propertyPath, outProperties );
                }
            }
            else // Core Types/Enums
            {
                outProperties.push_back( SerializedDynamicPropertyValue( propertyPath, propertyInstance.GetStringValue(), propertyInstance.GetTypeID() ) );
            }
        }

        bool SerializeDynamicType( TypeSystem::TypeRegistry const& typeRegistry, DynamicTypeInstance const& typeInstance, SerializedDynamicType& outType )
        {
            KRG_ASSERT( typeInstance.IsValid() );

            outType.m_typeID = typeInstance.GetTypeID();

            // Get all properties that have a non-default value set
            TypeSystem::PropertyPath path;
            for ( auto const& propertyInstance : typeInstance.GetProperties() )
            {
                FlattenProperties( propertyInstance, path, outType.m_properties );
            }

            return true;
        }

        bool Serialization::DeserializeDynamicType( TypeSystem::TypeRegistry const& typeRegistry, SerializedDynamicType const& serializedType, DynamicTypeInstance& outInstance )
        {
            KRG_ASSERT( serializedType.m_typeID.IsValid() );

            auto pTypeInfo = typeRegistry.GetTypeInfo( serializedType.m_typeID );
            if ( pTypeInfo == nullptr )
            {
                return false;
            }

            outInstance = DynamicTypeInstance( typeRegistry, pTypeInfo );

            for ( auto const& serializedProperty : serializedType.m_properties )
            {
                auto pProperty = outInstance.GetProperty( serializedProperty.m_path, true );
                if ( pProperty )
                {
                    pProperty->SetStringValue( serializedProperty.m_value );
                }
            }

            return true;
        }

        //-------------------------------------------------------------------------

        void WriteSerializedDynamicTypeToJSON( TypeSystem::TypeRegistry const& typeRegistry, rapidjson::Writer<RapidJsonStringBuffer>& writer, DynamicTypeInstance const& type )
        {
            KRG_ASSERT( type.IsValid() );

            SerializedDynamicType serializedType;
            if ( !SerializeDynamicType( typeRegistry, type, serializedType ) )
            {
                KRG_HALT();
            }

            //-------------------------------------------------------------------------

            writer.StartObject();

            // Every type has to have a type ID
            writer.Key( TypeSerializationSettings::TypeKey );
            writer.String( type.GetTypeID().GetAsStringID().ToString() );

            // Write all property values
            for ( auto const& propertyValue : serializedType.m_properties )
            {
                // Write Key
                writer.Key( propertyValue.m_path.ToString().c_str() );

                // Write Value
                writer.Key( propertyValue.m_value.c_str() );
            }

            writer.EndObject();
        }

        void ReadSerializedDynamicTypeFromJSON( TypeSystem::TypeRegistry const& typeRegistry, rapidjson::Value const& jsonObject, DynamicTypeInstance& outType )
        {
            SerializedDynamicType serializedDynamicType;

            //-------------------------------------------------------------------------

            // Read type ID
            KRG_ASSERT( jsonObject.IsObject() && jsonObject.HasMember( TypeSerializationSettings::TypeKey ) );
            serializedDynamicType.m_typeID = TypeSystem::TypeID( jsonObject[TypeSerializationSettings::TypeKey].GetString() );
            KRG_ASSERT( serializedDynamicType.m_typeID.IsValid() );

            // Read properties
            for ( auto itr = jsonObject.MemberBegin(); itr != jsonObject.MemberEnd(); ++itr )
            {
                if ( strcmp( itr->name.GetString(), TypeSerializationSettings::TypeKey ) == 0 )
                {
                    continue;
                }

                serializedDynamicType.m_properties.push_back( { TypeSystem::PropertyPath( itr->name.GetString() ), itr->value.GetString() } );
            }

            //-------------------------------------------------------------------------

            DeserializeDynamicType( typeRegistry, serializedDynamicType, outType );
        }
    }
}