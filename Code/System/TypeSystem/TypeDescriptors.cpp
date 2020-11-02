#include "TypeDescriptors.h"
#include "TypeRegistry.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    StringID PropertyDescriptor::GetEnumValueID() const
    {
        KRG_ASSERT( m_byteValue.size() == 4 );

        U32 rawStringID = 0;
        rawStringID |= m_byteValue[0];
        rawStringID |= m_byteValue[1] << 8;
        rawStringID |= m_byteValue[2] << 16;
        rawStringID |= m_byteValue[3] << 24;

        return StringID( rawStringID );
    }

    void PropertyDescriptor::SetEnumValueID( StringID enumValueID )
    {
        U32 const rawStringID = enumValueID.GetID();
        m_byteValue.emplace_back( rawStringID & 0xFF );
        m_byteValue.emplace_back( ( rawStringID >> 8 ) & 0xFF );
        m_byteValue.emplace_back( ( rawStringID >> 16 ) & 0xFF );
        m_byteValue.emplace_back( ( rawStringID >> 24 ) & 0xFF );
    }

    //-------------------------------------------------------------------------

    void TypeDescriptor::RemovePropertyValue( TypeSystem::PropertyPath const& path )
    {
        for ( S32 i = (S32) m_propertyValues.size() - 1; i >= 0; i-- )
        {
            if ( m_propertyValues[i].m_path == path )
            {
                VectorEraseUnsorted( m_propertyValues, i );
            }
        }
    }

    //-------------------------------------------------------------------------

    namespace
    {
        static void SetPropertyValue( TypeRegistry const& typeRegistry, TypeInfo const& typeInfo, void* pTypeInstance, PropertyDescriptor const& propertyValue )
        {
            KRG_ASSERT( pTypeInstance != nullptr && propertyValue.IsValid() );

            TypeSystem::ResolvedPropertyInfo const resolvedPropertyInfo = typeRegistry.ResolvePropertyPath( &typeInfo, propertyValue.m_path );
            if ( !resolvedPropertyInfo.IsValid() )
            {
                KRG_LOG_ERROR( "TypeSystem", "Tried to set the value for an invalid property (%s) for type (%s)", propertyValue.m_path.ToString().c_str(), typeInfo.m_ID.GetAsStringID().c_str() );
                return;
            }

            TypeSystem::PropertyInfo const* pPropertyInfo = resolvedPropertyInfo.m_pPropertyInfo;
            Byte* pPropertyData = reinterpret_cast<Byte*>( pTypeInstance ) + resolvedPropertyInfo.m_offset;
            Byte* pImmediateParent = pPropertyData - resolvedPropertyInfo.m_offset;

            // Resolve array property overrides
            //-------------------------------------------------------------------------

            if ( pPropertyInfo->IsArrayProperty() )
            {
                KRG_ASSERT( propertyValue.m_path.GetLastElement().IsArrayElement() );
                U32 const arrayIdx = propertyValue.m_path.GetLastElement().m_arrayElementIdx;

                if ( pPropertyInfo->IsStaticArrayProperty() )
                {
                    size_t const elementByteSize = typeRegistry.GetTypeByteSize( pPropertyInfo->m_typeID );
                    size_t const arrayElementOffset = elementByteSize * arrayIdx;
                    pPropertyData += arrayElementOffset;
                }
                else
                {
                    auto pParentTypeInfo = typeRegistry.GetTypeInfo( pPropertyInfo->m_parentTypeID );
                    KRG_ASSERT( pParentTypeInfo != nullptr );
                    pPropertyData = pParentTypeInfo->m_pTypeHelper->GetDynamicArrayElementDataPtr( pImmediateParent, pPropertyInfo->m_ID, arrayIdx );
                }
            }

            // Resolve enum string values
            //-------------------------------------------------------------------------

            if ( pPropertyInfo->IsEnumProperty() )
            {
                auto pEnumInfo = typeRegistry.GetEnumInfo( pPropertyInfo->m_typeID );
                KRG_ASSERT( pEnumInfo != nullptr );

                StringID const enumID = propertyValue.GetEnumValueID();
                S64 const enumValue = pEnumInfo->GetConstantValue( enumID );

                // TODO: make this more elegant
                if ( pPropertyInfo->m_size == 1 )
                {
                    S8 value = (S8) enumValue;
                    memcpy( pPropertyData, &value, pPropertyInfo->m_size );
                }
                else if ( pPropertyInfo->m_size == 2 )
                {
                    S16 value = (S16) enumValue;
                    memcpy( pPropertyData, &value, pPropertyInfo->m_size );
                }
                if ( pPropertyInfo->m_size == 4 )
                {
                    S32 value = (S32) enumValue;
                    memcpy( pPropertyData, &value, pPropertyInfo->m_size );
                }
                else // 64bit enum
                {
                    memcpy( pPropertyData, &enumValue, pPropertyInfo->m_size );
                }
            }

            // Type Conversion
            //-------------------------------------------------------------------------
            else
            {
                TypeSystem::TypeValueConverter::ConvertByteArrayToValue( pPropertyInfo->m_typeID, propertyValue.m_byteValue, pPropertyData );
            }
        }
    }

    void* TypeCreator::CreateFromDescriptor( TypeRegistry const& typeRegistry, TypeInfo const& typeInfo, TypeDescriptor const& typeDesc )
    {
        KRG_ASSERT( typeDesc.IsValid() && typeInfo.m_ID == typeDesc.m_typeID );

        void* pTypeInstance = typeInfo.m_pTypeHelper->CreateType();

        for ( auto const& propertyValue : typeDesc.m_propertyValues )
        {
            SetPropertyValue( typeRegistry, typeInfo, pTypeInstance, propertyValue );
        }

        return pTypeInstance;
    }
}