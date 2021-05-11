#include "TypeInstanceModel.h"
#include "System/TypeSystem/CoreTypeConversions.h"
#include "System/TypeSystem/TypeRegistry.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    namespace
    {
        static int64 GetEnumValue( EnumInfo const* pEnumInfo, void const* pPropertyValue )
        {
            KRG_ASSERT( pEnumInfo != nullptr && pPropertyValue != nullptr );

            int64 value = 0;

            switch ( pEnumInfo->m_underlyingType )
            {
                case CoreTypes::Uint8:
                {
                    value = ( int64 ) * reinterpret_cast<uint8 const*>( pPropertyValue );
                }
                break;

                case CoreTypes::Int8:
                {
                    value = ( int64 ) * reinterpret_cast<int8 const*>( pPropertyValue );
                }
                break;

                case CoreTypes::Uint16:
                {
                    value = ( int64 ) * reinterpret_cast<uint16 const*>( pPropertyValue );
                }
                break;

                case CoreTypes::Int16:
                {
                    value = ( int64 ) * reinterpret_cast<int16 const*>( pPropertyValue );
                }
                break;

                case CoreTypes::Uint32:
                {
                    value = ( int64 ) * reinterpret_cast<uint32 const*>( pPropertyValue );
                }
                break;

                case CoreTypes::Int32:
                {
                    value = ( int64 ) * reinterpret_cast<int32 const*>( pPropertyValue );
                }
                break;

                default:
                KRG_HALT();
                break;
            }

            return value;
        }

        static String GetEnumStringValue( EnumInfo const* pEnumInfo, void const* pPropertyValue )
        {
            int64 const defaultValue = GetEnumValue( pEnumInfo, pPropertyValue );
            return pEnumInfo->GetConstantLabel( defaultValue ).c_str();
        }
    }

    //-------------------------------------------------------------------------
    // PROPERTY INSTANCE
    //-------------------------------------------------------------------------

    PropertyInstanceModel::PropertyInstanceModel( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo )
        : m_propertyInfo( propertyInfo )
        , m_pTypeRegistry( &typeRegistry )
    {
        KRG_ASSERT( propertyInfo.IsValid() );

        // Friendly names
        //-------------------------------------------------------------------------

        m_friendlyName = m_propertyInfo.m_ID.c_str();
        if ( m_propertyInfo.m_typeID == CoreTypes::TResourcePtr )
        {
            m_friendlyName = StringUtils::RemoveAllOccurrencesInPlace( StringUtils::RemoveAllOccurrencesInPlace( m_friendlyName, "m_p" ), "m_" );
        }
        else
        {
            m_friendlyName = StringUtils::RemoveAllOccurrencesInPlace( m_friendlyName, "m_" );
        }
        m_friendlyName[0] = (char) toupper( m_friendlyName[0] );

        //-------------------------------------------------------------------------

        m_friendlyTypeName = propertyInfo.m_typeID.GetAsStringID().c_str();
        StringUtils::RemoveAllOccurrencesInPlace( m_friendlyTypeName, "KRG::" );

        // Properties
        //-------------------------------------------------------------------------

        if ( IsArray() )
        {
            m_value = m_defaultValue = String().sprintf( "%d", m_propertyInfo.m_arraySize );
            CreateDefaultArrayElements();
        }
        else
        {
            if ( IsCoreType() )
            {
                Conversion::ConvertNativeTypeToString( typeRegistry, m_propertyInfo, m_propertyInfo.m_pDefaultValue, m_defaultValue );
                m_value = m_defaultValue;
            }
            else if ( IsEnum() )
            {
                auto const pEnumInfo = m_pTypeRegistry->GetEnumInfo( GetTypeID() );
                KRG_ASSERT( pEnumInfo != nullptr );
                m_value = m_defaultValue = GetEnumStringValue( pEnumInfo, m_propertyInfo.m_pDefaultValue );
            }
            else // Complex type
            {
                m_pTypeInfo = typeRegistry.GetTypeInfo( m_propertyInfo.m_typeID );
                KRG_ASSERT( m_pTypeInfo != nullptr );

                // Create property instances
                for ( auto const& childPropertyInfo : m_pTypeInfo->m_properties )
                {
                    m_childProperties.emplace_back( PropertyInstanceModel( typeRegistry, childPropertyInfo ) );
                }
            }
        }
    }

    PropertyInstanceModel::PropertyInstanceModel( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo, String const& stringValue )
        : PropertyInstanceModel( typeRegistry, propertyInfo )
    {
        KRG_ASSERT( ( IsCoreType() || IsEnum() ) && !stringValue.empty() );
        m_value = stringValue;
        m_defaultValue = stringValue;
    }

    PropertyInstanceModel::PropertyInstanceModel( TypeRegistry const& typeRegistry, TypeInfo const* pTypeInfo )
        : m_pTypeRegistry( &typeRegistry )
    {
        KRG_ASSERT( pTypeInfo != nullptr );
        m_pTypeInfo = pTypeInfo;

        m_friendlyTypeName = m_pTypeInfo->m_ID.GetAsStringID().c_str();
        StringUtils::RemoveAllOccurrencesInPlace( m_friendlyTypeName, "KRG::" );

        // Create property instances
        for ( auto const& childPropertyInfo : m_pTypeInfo->m_properties )
        {
            m_childProperties.emplace_back( PropertyInstanceModel( typeRegistry, childPropertyInfo ) );
        }
    }

    //-------------------------------------------------------------------------

    ResourceTypeID PropertyInstanceModel::GetResourceTypeIDForResourceProperty() const
    {
        KRG_ASSERT( IsResourcePtr() || m_propertyInfo.m_typeID == CoreTypes::ResourceID );

        ResourceTypeID resourceTypeID;
        if ( m_propertyInfo.m_typeID == CoreTypes::TResourcePtr )
        {
            resourceTypeID = m_pTypeRegistry->GetResourceTypeIDForType( m_propertyInfo.m_templateArgumentTypeID );
        }

        return resourceTypeID;
    }

    //-------------------------------------------------------------------------

    PropertyInstanceModel* PropertyInstanceModel::GetProperty( PropertyPath const& path, bool allowDynamicArrayElementCreation )
    {
        KRG_ASSERT( IsValid() );
        KRG_ASSERT( !m_propertyInfo.IsArrayProperty() ); // Array properties do not have children with IDs

        PropertyInstanceModel* pFoundPropertyInstance = nullptr;

        //-------------------------------------------------------------------------

        auto const& pathElement = path.FirstElement();
        for ( auto& childProperty : m_childProperties )
        {
            if ( childProperty.GetID() == pathElement.m_propertyID )
            {
                pFoundPropertyInstance = &childProperty;
                break;
            }
        }

        if ( pFoundPropertyInstance == nullptr )
        {
            return nullptr;
        }

        //-------------------------------------------------------------------------

        if ( pathElement.IsArrayElement() && !pFoundPropertyInstance->IsArray() )
        {
            return nullptr;
        }

        //-------------------------------------------------------------------------

        if ( pathElement.IsArrayElement() )
        {
            // Handle the static array case
            if ( pFoundPropertyInstance->IsStaticArray() )
            {
                if ( pFoundPropertyInstance->IsValidArrayElementIndex( pathElement.m_arrayElementIdx ) )
                {
                    pFoundPropertyInstance = &pFoundPropertyInstance->GetArrayElement( pathElement.m_arrayElementIdx );
                }
                else
                {
                    return nullptr;
                }
            }
            else // Dynamic arrays
            {
                if ( pFoundPropertyInstance->IsValidArrayElementIndex( pathElement.m_arrayElementIdx ) )
                {
                    pFoundPropertyInstance = &pFoundPropertyInstance->GetArrayElement( pathElement.m_arrayElementIdx );
                }
                else
                {
                    if ( allowDynamicArrayElementCreation )
                    {
                        int32 const numElementsToAdd = pathElement.m_arrayElementIdx - pFoundPropertyInstance->GetNumArrayElements() + 1;
                        for ( auto i = 0; i < numElementsToAdd; i++ )
                        {
                            pFoundPropertyInstance->AddArrayElement();
                        }

                        pFoundPropertyInstance = &pFoundPropertyInstance->GetArrayElement( pathElement.m_arrayElementIdx );
                    }
                    else
                    {
                        return nullptr;
                    }
                }
            }
        }

        //-------------------------------------------------------------------------

        if ( path.GetNumElements() > 1 )
        {
            PropertyPath const subPath = path.GetPathWithoutFirstElement();
            return pFoundPropertyInstance->GetProperty( subPath );
        }

        return pFoundPropertyInstance;
    }

    PropertyInstanceModel* PropertyInstanceModel::GetProperty( StringID propertyID )
    {
        KRG_ASSERT( !m_propertyInfo.IsArrayProperty() );

        for ( auto& childProperty : m_childProperties )
        {
            if ( childProperty.GetID() == propertyID )
            {
                return &childProperty;
            }
        }

        return nullptr;
    }

    //-------------------------------------------------------------------------

    EnumInfo const* PropertyInstanceModel::GetEnumInfo() const
    {
        KRG_ASSERT( IsEnum() );

        auto const pEnumInfo = m_pTypeRegistry->GetEnumInfo( GetTypeID() );
        KRG_ASSERT( pEnumInfo != nullptr );

        return pEnumInfo;
    }

    int32 PropertyInstanceModel::GetNumEnumValues() const
    {
        KRG_ASSERT( IsEnum() );

        auto const pEnumInfo = m_pTypeRegistry->GetEnumInfo( GetTypeID() );
        KRG_ASSERT( pEnumInfo != nullptr );
        return (int32) pEnumInfo->m_constants.size();

    }

    TVector<char const*> PropertyInstanceModel::GetEnumValues() const
    {
        KRG_ASSERT( IsEnum() );

        auto const pEnumInfo = m_pTypeRegistry->GetEnumInfo( GetTypeID() );
        KRG_ASSERT( pEnumInfo != nullptr );

        TVector<char const*> outValues;
        for ( auto const& constant : pEnumInfo->m_constants )
        {
            outValues.emplace_back( constant.first.ToString() );
        }

        return outValues;
    }

    int64 PropertyInstanceModel::GetIntForEnumStringValue( char const* pString ) const
    {
        KRG_ASSERT( IsEnum() );

        auto const pEnumInfo = m_pTypeRegistry->GetEnumInfo( GetTypeID() );
        KRG_ASSERT( pEnumInfo != nullptr );
        return pEnumInfo->GetConstantValue( StringID( pString ) );
    }

    char const* PropertyInstanceModel::GetStringValueForEnumValue( int64 value ) const
    {
        KRG_ASSERT( IsEnum() );

        auto const pEnumInfo = m_pTypeRegistry->GetEnumInfo( GetTypeID() );
        KRG_ASSERT( pEnumInfo != nullptr );

        for ( auto const& constant : pEnumInfo->m_constants )
        {
            if ( constant.second == value )
            {
                return constant.first.c_str();
            }
        }

        KRG_UNREACHABLE_CODE();
        return nullptr;
    }

    //-------------------------------------------------------------------------

    void PropertyInstanceModel::CreateDefaultArrayElements()
    {
        KRG_ASSERT( m_pTypeRegistry != nullptr );
        KRG_ASSERT( m_propertyInfo.IsValid() && m_propertyInfo.IsArrayProperty() );

        //-------------------------------------------------------------------------

        if ( m_propertyInfo.IsStaticArrayProperty() )
        {
            KRG_ASSERT( m_propertyInfo.m_arraySize > 0 );
        }

        //-------------------------------------------------------------------------

        for ( auto i = 0; i < m_propertyInfo.m_arraySize; i++ )
        {
            AddArrayElementInternal();
        }
    }

    void PropertyInstanceModel::AddArrayElementInternal()
    {
        KRG_ASSERT( IsValid() );
        KRG_ASSERT( IsArray() );

        PropertyInstanceModel* pAddedArrayElement = nullptr;
        int32 const newArrayElementIdx = (int32) m_childProperties.size();

        PropertyInfo arrayElementInfo = m_propertyInfo;
        arrayElementInfo.m_flags.ClearFlag( PropertyInfo::Flags::IsArray );
        arrayElementInfo.m_flags.ClearFlag( PropertyInfo::Flags::IsDynamicArray );
        arrayElementInfo.m_ID = StringID( String().sprintf( "m_%d", newArrayElementIdx ) );
        
        //-------------------------------------------------------------------------

        // If this is one of the defaults elements, set the correct default values
        if ( newArrayElementIdx < m_propertyInfo.m_arraySize )
        {
            if ( TypeSystem::IsCoreType( arrayElementInfo.m_typeID ) )
            {
                String arrayElementDefaultStringValue;
                Conversion::ConvertNativeTypeToString( *m_pTypeRegistry, m_propertyInfo, m_propertyInfo.GetArrayDefaultElementPtr( newArrayElementIdx ), arrayElementDefaultStringValue );
                pAddedArrayElement = &m_childProperties.emplace_back( PropertyInstanceModel( *m_pTypeRegistry, arrayElementInfo, arrayElementDefaultStringValue ) );
            }
            else if ( m_propertyInfo.IsEnumProperty() )
            {
                auto const pEnumInfo = m_pTypeRegistry->GetEnumInfo( arrayElementInfo.m_typeID );
                KRG_ASSERT( pEnumInfo != nullptr );

                String arrayElementDefaultStringValue = GetEnumStringValue( pEnumInfo, m_propertyInfo.GetArrayDefaultElementPtr( newArrayElementIdx ) );
                pAddedArrayElement = &m_childProperties.emplace_back( PropertyInstanceModel( *m_pTypeRegistry, arrayElementInfo, arrayElementDefaultStringValue ) );
            }
            else
            {
                pAddedArrayElement = &m_childProperties.emplace_back( PropertyInstanceModel( *m_pTypeRegistry, arrayElementInfo ) );
            }
        }
        else
        {
            pAddedArrayElement = &m_childProperties.emplace_back( PropertyInstanceModel( *m_pTypeRegistry, arrayElementInfo ) );
        }

        //-------------------------------------------------------------------------

        KRG_ASSERT( pAddedArrayElement != nullptr );
        pAddedArrayElement->m_arrayElementIdx = newArrayElementIdx;
    }

    void PropertyInstanceModel::SetNumArrayElements( int32 numElements )
    {
        KRG_ASSERT( IsValid() );
        KRG_ASSERT( IsDynamicArray() );
        KRG_ASSERT( numElements >= 0 );

        if ( numElements == 0 )
        {
            m_childProperties.clear();
        }

        else
        {
            int32 const currentNumElements = GetNumArrayElements();
            if ( numElements == currentNumElements )
            {
                return;
            }
            else if ( numElements > currentNumElements )
            {
                int32 const numElementsToAdd = numElements - currentNumElements;
                for ( auto i = 0; i < numElementsToAdd; i++ )
                {
                    AddArrayElement();
                }
            }
            else
            {
                int32 const numElementsToRemove = currentNumElements - numElements;
                for ( auto i = 0; i < numElementsToRemove; i++ )
                {
                    m_childProperties.pop_back();
                }
            }
        }
    }

    void PropertyInstanceModel::RemoveArrayElement( int32 elementIdx )
    {
        KRG_ASSERT( IsValid() );
        KRG_ASSERT( IsDynamicArray() );
        KRG_ASSERT( elementIdx >= 0 && elementIdx < m_childProperties.size() );

        m_childProperties.erase( m_childProperties.begin() + elementIdx );

        // Fix up array element IDs and indices
        for ( int32 i = elementIdx; i < (int32) m_childProperties.size(); i++ )
        {
            m_childProperties[i].m_propertyInfo.m_ID = StringID( String().sprintf( "m_%d", i ) );
            m_childProperties[i].m_friendlyName = String().sprintf( "%d", i );
            m_childProperties[i].m_arrayElementIdx = i;
        }
    }

    //-------------------------------------------------------------------------

    void PropertyInstanceModel::SetStringValue( String const& stringValue )
    {
        KRG_ASSERT( !IsStaticArray() && !IsStructure() );

        if ( IsDynamicArray() )
        {
            m_value = stringValue;
            int32 const numElements = StringUtils::StrToS32( stringValue );
            SetNumArrayElements( numElements );
        }

        if ( !Conversion::IsValidStringValueForType( *m_pTypeRegistry, m_propertyInfo, stringValue ) )
        {
            return;
        }

        m_value = stringValue;
    }

    bool PropertyInstanceModel::IsDefaultValue() const
    {
        if ( m_value != m_defaultValue )
        {
            return false;
        }

        //-------------------------------------------------------------------------

        if ( IsDynamicArray() )
        {
            if ( GetNumArrayElements() != m_propertyInfo.m_arraySize )
            {
                return false;
            }

            // Manually check element vs defaults, since you could have removed and created a new entry which will have a different default that the one specified in c++
            for ( auto i = 0; i < m_propertyInfo.m_arraySize; i++ )
            {
                if ( TypeSystem::IsCoreType( m_propertyInfo.m_typeID ) )
                {
                    String defaultElementValue;
                    Conversion::ConvertNativeTypeToString( *m_pTypeRegistry, m_propertyInfo, m_propertyInfo.GetArrayDefaultElementPtr( i ), defaultElementValue );
                    if ( m_childProperties[i].GetStringValue() != defaultElementValue )
                    {
                        return false;
                    }
                }
                else if ( m_propertyInfo.IsEnumProperty() )
                {
                    auto const pEnumInfo = m_pTypeRegistry->GetEnumInfo( m_propertyInfo.m_typeID );
                    KRG_ASSERT( pEnumInfo != nullptr );

                    String defaultElementValue = GetEnumStringValue( pEnumInfo, m_propertyInfo.GetArrayDefaultElementPtr( i ) );
                    if ( m_childProperties[i].GetStringValue() != defaultElementValue )
                    {
                        return false;
                    }
                }
            }
        }

        //-------------------------------------------------------------------------

        for ( auto const& childProperty : m_childProperties )
        {
            if ( !childProperty.IsDefaultValue() )
            {
                return false;
            }
        }

        //-------------------------------------------------------------------------

        return true;
    }

    void PropertyInstanceModel::ResetToDefaultValue()
    {
        m_value = m_defaultValue;

        //-------------------------------------------------------------------------

        // If we are resetting a dynamic array property, destroy all child properties and recreate them from the defaults
        if ( IsDynamicArray() )
        {
            m_childProperties.clear();
            CreateDefaultArrayElements();
        }

        //-------------------------------------------------------------------------

        for ( auto& childProperty : m_childProperties )
        {
            childProperty.ResetToDefaultValue();
        }
    }

    //-------------------------------------------------------------------------
    // TYPE INSTANCE
    //-------------------------------------------------------------------------

    namespace
    {
        static void FlattenProperties( PropertyInstanceModel const& propertyInstance, PropertyPath const& currentPath, TInlineVector<PropertyDescriptor, 6>& outProperties )
        {
            if ( propertyInstance.IsDefaultValue() )
            {
                return;
            }

            //-------------------------------------------------------------------------

            PropertyPath propertyPath = currentPath;
            if ( propertyInstance.IsArrayElement() )
            {
                propertyPath.ReplaceLastElement( propertyPath.GetLastElement().m_propertyID, propertyInstance.GetArrayElementIndex() );
            }
            else
            {
                propertyPath += propertyInstance.GetID();
            }

            //-------------------------------------------------------------------------

            if ( propertyInstance.IsArray() )
            {
                // Flatten array elements
                for ( auto const& childProperty : propertyInstance.GetProperties() )
                {
                    FlattenProperties( childProperty, propertyPath, outProperties );
                }
            }
            else if ( propertyInstance.IsStructure() )
            {
                for ( auto const& childProperty : propertyInstance.GetProperties() )
                {
                    FlattenProperties( childProperty, propertyPath, outProperties );
                }
            }
            else // Core Types/Enums
            {
                outProperties.push_back( PropertyDescriptor( propertyPath, propertyInstance.GetStringValue(), propertyInstance.GetTypeID(), propertyInstance.GetTemplatedArgumentTypeID() ) );
            }
        }
    }

    //-------------------------------------------------------------------------

    TypeInstanceModel::TypeInstanceModel( TypeRegistry const& typeRegistry, TypeInfo const* pTypeInfo )
        : PropertyInstanceModel( typeRegistry, pTypeInfo )
    {}

    TypeInstanceModel::TypeInstanceModel( TypeRegistry const& typeRegistry, TypeDescriptor const& typeDescriptor )
        : TypeInstanceModel( typeRegistry, typeRegistry.GetTypeInfo( typeDescriptor.m_typeID ) )
    {
        // Set property values
        for ( auto const& serializedProperty : typeDescriptor.m_properties )
        {
            auto pProperty = GetProperty( serializedProperty.m_path, true );
            if ( pProperty )
            {
                pProperty->SetStringValue( serializedProperty.m_stringValue );
            }
        }
    }

    //-------------------------------------------------------------------------

    TypeDescriptor TypeInstanceModel::GetDescriptor() const
    {
        TypeDescriptor descriptor;
        descriptor.m_typeID = m_pTypeInfo->m_ID;

        // Get all properties that have a non-default value set
        PropertyPath path;
        for ( auto const& pPropertyInstance : m_childProperties )
        {
            FlattenProperties( pPropertyInstance, path, descriptor.m_properties );
        }

        return descriptor;
    }
}