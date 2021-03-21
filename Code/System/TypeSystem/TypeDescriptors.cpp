#include "TypeDescriptors.h"
#include "TypeRegistry.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    void TypeDescriptor::RemovePropertyValue( TypeSystem::PropertyPath const& path )
    {
        for ( int32 i = (int32) m_propertyValues.size() - 1; i >= 0; i-- )
        {
            if ( m_propertyValues[i].m_path == path )
            {
                m_propertyValues.erase_unsorted( m_propertyValues.begin() + i );
            }
        }
    }

    //-------------------------------------------------------------------------

    void* TypeCreator::CreateFromDescriptor( TypeRegistry const& typeRegistry, TypeInfo const& typeInfo, TypeDescriptor const& typeDesc )
    {
        KRG_ASSERT( typeDesc.IsValid() && typeInfo.m_ID == typeDesc.m_typeID );

        void* pTypeInstance = typeInfo.m_pTypeHelper->CreateType();
        KRG_ASSERT( pTypeInstance != nullptr );

        // Set property value
        for ( auto const& propertyValue : typeDesc.m_propertyValues )
        {
            KRG_ASSERT( propertyValue.IsValid() );

            TypeSystem::ResolvedPropertyInfo const resolvedPropertyInfo = typeRegistry.ResolvePropertyPath( &typeInfo, propertyValue.m_path );
            if ( !resolvedPropertyInfo.IsValid() )
            {
                KRG_LOG_ERROR( "TypeSystem", "Tried to set the value for an invalid property (%s) for type (%s)", propertyValue.m_path.ToString().c_str(), typeInfo.m_ID.GetAsStringID().c_str() );
                continue;
            }

            TypeSystem::PropertyInfo const* pPropertyInfo = resolvedPropertyInfo.m_pPropertyInfo;
            Byte* pPropertyData = reinterpret_cast<Byte*>( pTypeInstance ) + resolvedPropertyInfo.m_offset;
            Byte* pImmediateParent = pPropertyData - resolvedPropertyInfo.m_offset;

            // Resolve array property overrides
            if ( pPropertyInfo->IsArrayProperty() )
            {
                KRG_ASSERT( propertyValue.m_path.GetLastElement().IsArrayElement() );
                uint32 const arrayIdx = propertyValue.m_path.GetLastElement().m_arrayElementIdx;

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

            // Set actual property value
            Conversion::ConvertBinaryToNativeType( typeRegistry, *pPropertyInfo, propertyValue.m_byteValue, pPropertyData );
        }

        return pTypeInstance;
    }
}