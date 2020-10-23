#include "DynamicPropertyInstance.h"
#include "System/TypeSystem/PropertyPath.h"

//-------------------------------------------------------------------------

namespace KRG
{
    S64 GetEnumValue( TypeSystem::EnumInfo const* pEnumInfo, void const* pPropertyValue )
    {
        KRG_ASSERT( pEnumInfo != nullptr && pPropertyValue != nullptr );

        S64 value = 0;

        switch ( pEnumInfo->m_underlyingType )
        {
            case TypeSystem::CoreTypes::U8:
            {
                value = (S64) *reinterpret_cast<U8 const*>( pPropertyValue );
            }
            break;

            case TypeSystem::CoreTypes::S8:
            {
                value = (S64) *reinterpret_cast<S8 const*>( pPropertyValue );
            }
            break;

            case TypeSystem::CoreTypes::U16:
            {
                value = (S64) *reinterpret_cast<U16 const*>( pPropertyValue );
            }
            break;

            case TypeSystem::CoreTypes::S16:
            {
                value = (S64) *reinterpret_cast<S16 const*>( pPropertyValue );
            }
            break;

            case TypeSystem::CoreTypes::U32:
            {
                value = (S64) *reinterpret_cast<U32 const*>( pPropertyValue );
            }
            break;

            case TypeSystem::CoreTypes::S32:
            {
                value = (S64) *reinterpret_cast<S32 const*>( pPropertyValue );
            }
            break;

            default:
            KRG_HALT();
            break;
        }

        return value;
    }

    String GetEnumStringValue( TypeSystem::EnumInfo const* pEnumInfo, void const* pPropertyValue )
    {
        S64 const defaultValue = GetEnumValue( pEnumInfo, pPropertyValue );
        return pEnumInfo->GetConstantLabel( defaultValue ).c_str();
    }

    //-------------------------------------------------------------------------

    DynamicPropertyInstance::DynamicPropertyInstance( TypeSystem::TypeRegistry const& typeRegistry, TypeSystem::PropertyInfo const& propertyInfo )
        : m_propertyInfo( propertyInfo )
        , m_pTypeRegistry( &typeRegistry )
    {
        KRG_ASSERT( propertyInfo.IsValid() );

        //-------------------------------------------------------------------------

        m_friendlyName = m_propertyInfo.m_ID.c_str();
        m_friendlyName = StringUtils::RemoveAllOccurrencesInPlace( StringUtils::RemoveAllOccurrencesInPlace( m_friendlyName, "m_p" ), "m_" );
        m_friendlyName[0] = (char) toupper( m_friendlyName[0] );

        //-------------------------------------------------------------------------

        if ( IsArrayProperty() )
        {
            m_value = m_defaultValue = String().sprintf( "%d", m_propertyInfo.m_arraySize );
            CreateDefaultArrayElements();
        }
        else
        {
            if ( IsCoreTypeProperty() )
            {
                TypeSystem::TypeValueConverter::ConvertValueToString( m_propertyInfo.m_typeID, m_propertyInfo.m_pDefaultValue, m_defaultValue );
                m_value = m_defaultValue;
            }
            else if ( IsEnumProperty() )
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
                    m_childProperties.emplace_back( DynamicPropertyInstance( typeRegistry, childPropertyInfo.second ) );
                }
            }
        }
    }

    DynamicPropertyInstance::DynamicPropertyInstance( TypeSystem::TypeRegistry const& typeRegistry, TypeSystem::PropertyInfo const& propertyInfo, String const& stringValue )
        : DynamicPropertyInstance( typeRegistry, propertyInfo )
    {
        KRG_ASSERT( ( IsCoreTypeProperty() || IsEnumProperty() ) && !stringValue.empty() );
        m_value = stringValue;
        m_defaultValue = stringValue;
    }

    DynamicPropertyInstance::DynamicPropertyInstance( TypeSystem::TypeRegistry const& typeRegistry, TypeSystem::TypeInfo const* pTypeInfo )
        : m_pTypeRegistry( &typeRegistry )
    {
        KRG_ASSERT( pTypeInfo != nullptr );
        m_pTypeInfo = pTypeInfo;

        // Create property instances
        for ( auto const& childPropertyInfo : m_pTypeInfo->m_properties )
        {
            m_childProperties.emplace_back( DynamicPropertyInstance( typeRegistry, childPropertyInfo.second ) );
        }
    }

    //-------------------------------------------------------------------------

    DynamicPropertyInstance* DynamicPropertyInstance::GetProperty( TypeSystem::PropertyPath const& path, bool allowDynamicArrayElementCreation )
    {
        KRG_ASSERT( IsValid() );
        KRG_ASSERT( !m_propertyInfo.IsArrayProperty() ); // Array properties do not have children with IDs

        DynamicPropertyInstance* pFoundPropertyInstance = nullptr;

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

        if ( pathElement.IsArrayElement() && !pFoundPropertyInstance->IsArrayProperty() )
        {
            return nullptr;
        }

        //-------------------------------------------------------------------------

        if ( pathElement.IsArrayElement() )
        {
            // Handle the static array case
            if ( pFoundPropertyInstance->IsStaticArrayProperty() )
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
                        S32 const numElementsToAdd = pathElement.m_arrayElementIdx - pFoundPropertyInstance->GetNumArrayElements() + 1;
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
            TypeSystem::PropertyPath const subPath = path.GetPathWithoutFirstElement();
            return pFoundPropertyInstance->GetProperty( subPath );
        }

        return pFoundPropertyInstance;
    }

    //-------------------------------------------------------------------------

    TVector<char const*> DynamicPropertyInstance::GetEnumValues() const
    {
        KRG_ASSERT( IsEnumProperty() );

        auto const pEnumInfo = m_pTypeRegistry->GetEnumInfo( GetTypeID() );
        KRG_ASSERT( pEnumInfo != nullptr );

        TVector<char const*> outValues;
        for ( auto const& constant : pEnumInfo->m_constants )
        {
            outValues.emplace_back( constant.first.ToString() );
        }

        return outValues;
    }

    S64 DynamicPropertyInstance::GetIntForEnumStringValue( char const* pString )
    {
        KRG_ASSERT( IsEnumProperty() );

        auto const pEnumInfo = m_pTypeRegistry->GetEnumInfo( GetTypeID() );
        KRG_ASSERT( pEnumInfo != nullptr );
        return pEnumInfo->GetConstantValue( StringID( pString ) );
    }

    char const* DynamicPropertyInstance::GetStringForEnumValue( S64 value )
    {
        KRG_ASSERT( IsEnumProperty() );

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

    bool DynamicPropertyInstance::IsValidEnumStringValue( char const* pString )
    {
        KRG_ASSERT( IsEnumProperty() );

        auto const pEnumInfo = m_pTypeRegistry->GetEnumInfo( GetTypeID() );
        KRG_ASSERT( pEnumInfo != nullptr );

        return pEnumInfo->IsValidValue( StringID( pString ) );
    }

    //-------------------------------------------------------------------------

    void DynamicPropertyInstance::CreateDefaultArrayElements()
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

    void DynamicPropertyInstance::AddArrayElementInternal()
    {
        KRG_ASSERT( IsValid() );
        KRG_ASSERT( IsArrayProperty() );

        DynamicPropertyInstance* pAddedArrayElement = nullptr;
        S32 const newArrayElementIdx = (S32) m_childProperties.size();

        TypeSystem::PropertyInfo arrayElementInfo = m_propertyInfo;
        arrayElementInfo.m_flags.ClearFlag( TypeSystem::PropertyInfo::Flags::IsArray );
        arrayElementInfo.m_flags.ClearFlag( TypeSystem::PropertyInfo::Flags::IsDynamicArray );
        arrayElementInfo.m_ID = StringID( String().sprintf( "m_%d", newArrayElementIdx ) );

        //-------------------------------------------------------------------------

        // If this is one of the defaults elements, set the correct default values
        if ( newArrayElementIdx < m_propertyInfo.m_arraySize )
        {
            if ( TypeSystem::IsCoreType( arrayElementInfo.m_typeID ) )
            {
                String arrayElementDefaultStringValue;
                TypeSystem::TypeValueConverter::ConvertValueToString( m_propertyInfo.m_typeID, m_propertyInfo.GetArrayDefaultElementPtr( newArrayElementIdx ), arrayElementDefaultStringValue );
                pAddedArrayElement = &m_childProperties.emplace_back( DynamicPropertyInstance( *m_pTypeRegistry, arrayElementInfo, arrayElementDefaultStringValue ) );
            }
            else if ( m_propertyInfo.IsEnumProperty() )
            {
                auto const pEnumInfo = m_pTypeRegistry->GetEnumInfo( arrayElementInfo.m_typeID );
                KRG_ASSERT( pEnumInfo != nullptr );

                String arrayElementDefaultStringValue = GetEnumStringValue( pEnumInfo, m_propertyInfo.GetArrayDefaultElementPtr( newArrayElementIdx ) );
                pAddedArrayElement = &m_childProperties.emplace_back( DynamicPropertyInstance( *m_pTypeRegistry, arrayElementInfo, arrayElementDefaultStringValue ) );
            }
            else
            {
                pAddedArrayElement = &m_childProperties.emplace_back( DynamicPropertyInstance( *m_pTypeRegistry, arrayElementInfo ) );
            }
        }
        else
        {
            pAddedArrayElement = &m_childProperties.emplace_back( DynamicPropertyInstance( *m_pTypeRegistry, arrayElementInfo ) );
        }

        //-------------------------------------------------------------------------

        KRG_ASSERT( pAddedArrayElement != nullptr );
        pAddedArrayElement->m_arrayElementIdx = newArrayElementIdx;
    }

    void DynamicPropertyInstance::SetNumArrayElements( S32 numElements )
    {
        KRG_ASSERT( IsValid() );
        KRG_ASSERT( IsDynamicArrayProperty() );
        KRG_ASSERT( numElements >= 0 );

        if ( numElements == 0 )
        {
            m_childProperties.clear();
        }
       
        else
        {
            S32 const currentNumElements = GetNumArrayElements();
            if ( numElements == currentNumElements )
            {
                return;
            }
            else if ( numElements > currentNumElements )
            {
                S32 const numElementsToAdd = numElements - currentNumElements;
                for ( auto i = 0; i < numElementsToAdd; i++ )
                {
                    AddArrayElement();
                }
            }
            else
            {
                S32 const numElementsToRemove = currentNumElements - numElements;
                for ( auto i = 0; i < numElementsToRemove; i++ )
                {
                    m_childProperties.pop_back();
                }
            }
        }
    }

    void DynamicPropertyInstance::RemoveArrayElement( S32 elementIdx )
    {
        KRG_ASSERT( IsValid() );
        KRG_ASSERT( IsDynamicArrayProperty() );
        KRG_ASSERT( elementIdx >= 0 && elementIdx < m_childProperties.size() );
        m_childProperties.erase( m_childProperties.begin() + elementIdx );

        // Fix up array element IDs and indices
        for ( S32 i = elementIdx; i < (S32) m_childProperties.size(); i++ )
        {
            m_childProperties[i].m_propertyInfo.m_ID = StringID( String().sprintf( "m_%d", i ) );
            m_childProperties[i].m_arrayElementIdx = i;
        }
    }

    //-------------------------------------------------------------------------

    bool DynamicPropertyInstance::IsDefaultValue() const
    {
        if ( m_value != m_defaultValue )
        {
            return false;
        }

        //-------------------------------------------------------------------------

        if ( IsDynamicArrayProperty() )
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
                    TypeSystem::TypeValueConverter::ConvertValueToString( m_propertyInfo.m_typeID, m_propertyInfo.GetArrayDefaultElementPtr( i ), defaultElementValue );
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

    void DynamicPropertyInstance::ResetToDefaultValue()
    {
        m_value = m_defaultValue;

        //-------------------------------------------------------------------------

        // If we are resetting a dynamic array property, destroy all child properties and recreate them from the defaults
        if ( IsDynamicArrayProperty() )
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
}