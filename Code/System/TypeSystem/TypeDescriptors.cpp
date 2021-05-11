#include "TypeDescriptors.h"
#include "TypeRegistry.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    void TypeDescriptor::RemovePropertyValue( TypeSystem::PropertyPath const& path )
    {
        for ( int32 i = (int32) m_properties.size() - 1; i >= 0; i-- )
        {
            if ( m_properties[i].m_path == path )
            {
                m_properties.erase_unsorted( m_properties.begin() + i );
            }
        }
    }
}

//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    struct ResolvedPathElement
    {
        StringID                m_propertyID;
        int32                   m_arrayElementIdx;
        PropertyInfo const*     m_pPropertyInfo;
        Byte*                   m_pAddress;
    };

    struct ResolvedPath
    {
        inline bool IsValid() const { return !m_pathElements.empty(); }
        inline void Reset() { m_pathElements.clear(); }

        TInlineVector<ResolvedPathElement, 6> m_pathElements;
    };

    // Resolves a given property path with a given type instance to calculate the final address of the property this path refers to
    ResolvedPath ResolvePropertyPath( TypeRegistry const& typeRegistry, TypeInfo const* pTypeInfo, Byte* const pTypeInstanceAddress, PropertyPath const& path )
    {
        ResolvedPath resolvedPath;

        Byte* pResolvedTypeInstance = pTypeInstanceAddress;
        TypeInfo const* pResolvedTypeInfo = pTypeInfo;
        PropertyInfo const* pFoundPropertyInfo = nullptr;
        
        // Resolve property path
        size_t const numPathElements = path.GetNumElements();
        size_t const lastElementIdx = numPathElements - 1;
        for ( size_t i = 0; i < numPathElements; i++ )
        {
            KRG_ASSERT( pResolvedTypeInfo != nullptr );

            // Get the property info for the path element
            pFoundPropertyInfo = pResolvedTypeInfo->GetPropertyInfo( path[i].m_propertyID );
            if ( pFoundPropertyInfo == nullptr )
            {
                resolvedPath.Reset();
                break;
            }

            ResolvedPathElement& resolvedPathElement = resolvedPath.m_pathElements.emplace_back();
            resolvedPathElement.m_propertyID = path[i].m_propertyID;
            resolvedPathElement.m_arrayElementIdx = path[i].m_arrayElementIdx;
            resolvedPathElement.m_pPropertyInfo = pFoundPropertyInfo;

            // Calculate the address of the resolved property
            if ( pFoundPropertyInfo->IsArrayProperty() )
            {
                resolvedPathElement.m_pAddress = pResolvedTypeInfo->m_pTypeHelper->GetArrayElementDataPtr( pResolvedTypeInstance, path[i].m_propertyID, path[i].m_arrayElementIdx );
            }
            else // Structure/Type
            {
                resolvedPathElement.m_pAddress = pResolvedTypeInstance + pFoundPropertyInfo->m_offset;
            }

            // Update the resolved type instance and type info
            pResolvedTypeInstance = resolvedPathElement.m_pAddress;
            if ( !IsCoreType( resolvedPathElement.m_pPropertyInfo->m_typeID ) )
            {
                pResolvedTypeInfo = typeRegistry.GetTypeInfo( resolvedPathElement.m_pPropertyInfo->m_typeID );
            }
            else
            {
                pResolvedTypeInfo = nullptr;
            }
        }

        return resolvedPath;
    }

    //-------------------------------------------------------------------------

    void* TypeCreator::SetPropertyValues( TypeRegistry const& typeRegistry, TypeInfo const& typeInfo, TypeDescriptor const& typeDesc, void* pTypeInstance )
    {
        KRG_ASSERT( typeDesc.IsValid() && typeInfo.m_ID == typeDesc.m_typeID );

        for ( auto const& propertyValue : typeDesc.m_properties )
        {
            KRG_ASSERT( propertyValue.IsValid() );

            // Resolve a property path for a given instance
            auto resolvedPath = ResolvePropertyPath( typeRegistry, &typeInfo, (Byte*) pTypeInstance, propertyValue.m_path );
            if ( !resolvedPath.IsValid() )
            {
                KRG_LOG_ERROR( "TypeSystem", "Tried to set the value for an invalid property (%s) for type (%s)", propertyValue.m_path.ToString().c_str(), typeInfo.m_ID.GetAsStringID().c_str() );
                continue;
            }

            // Set actual property value
            auto const& resolvedProperty = resolvedPath.m_pathElements.back();
            Conversion::ConvertBinaryToNativeType( typeRegistry, *resolvedProperty.m_pPropertyInfo, propertyValue.m_byteValue, resolvedProperty.m_pAddress );
        }

        return pTypeInstance;
    }
}

//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    void TypeDescriptorCollection::Reset()
    {
        m_descriptors.clear();
        m_totalRequiredSize = -1;
        m_requiredAlignment = -1;
        m_typeInfos.clear();
        m_typeSizes.clear();
        m_typePaddings.clear();
    }

    void TypeDescriptorCollection::CalculateCollectionRequirements( TypeRegistry const& typeRegistry )
    {
        m_totalRequiredSize = 0;
        m_requiredAlignment = 0;

        m_typeInfos.clear();
        m_typeSizes.clear();
        m_typePaddings.clear();

        int32 const numDescs = (int32) m_descriptors.size();
        if ( numDescs == 0 )
        {
            return;
        }

        //-------------------------------------------------------------------------

        m_typeInfos.reserve( numDescs );
        m_typeSizes.reserve( numDescs );
        m_typePaddings.reserve( numDescs );

        uintptr_t predictedMemoryOffset = 0;

        for ( auto const& typeDesc : m_descriptors )
        {
            auto pTypeInfo = typeRegistry.GetTypeInfo( typeDesc.m_typeID );
            KRG_ASSERT( pTypeInfo != nullptr );
            KRG_ASSERT( pTypeInfo->m_size > 0 && pTypeInfo->m_alignment > 0 );

            // Update overall alignment requirements and the required padding
            m_requiredAlignment = Math::Max( m_requiredAlignment, pTypeInfo->m_alignment );
            size_t const requiredPadding = Memory::CalculatePaddingForAlignment( predictedMemoryOffset, pTypeInfo->m_alignment );
            predictedMemoryOffset += (uintptr_t) pTypeInfo->m_size + requiredPadding;

            m_typeInfos.emplace_back( pTypeInfo );
            m_typeSizes.emplace_back( pTypeInfo->m_size );
            m_typePaddings.emplace_back( (uint32) requiredPadding );
        }

        m_totalRequiredSize = (uint32) predictedMemoryOffset;
    }
}