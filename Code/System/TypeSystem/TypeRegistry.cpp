#include "TypeRegistry.h"
#include "PropertyPath.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem
    {
        TypeRegistry::~TypeRegistry()
        {
            KRG_ASSERT( m_registeredEnums.empty() && m_registeredTypes.empty() && m_registeredResourceTypes.empty() );
        }

        //-------------------------------------------------------------------------

        TypeInfo const* TypeRegistry::RegisterType( TypeInfo const& type )
        {
            KRG_ASSERT( type.m_ID.IsValid() && !IsCoreType( type.m_ID ) );
            KRG_ASSERT( m_registeredTypes.find( type.m_ID ) == m_registeredTypes.end() );
            m_registeredTypes.insert( eastl::pair<TypeID, TypeInfo*>( type.m_ID, KRG::New<TypeInfo>( type ) ) );
            return m_registeredTypes[type.m_ID];
        }

        void TypeRegistry::UnregisterType( TypeID const typeID )
        {
            KRG_ASSERT( typeID.IsValid() && !IsCoreType( typeID ) );
            auto iter = m_registeredTypes.find( typeID );
            KRG_ASSERT( iter != m_registeredTypes.end() );
            KRG::Delete<TypeInfo>( iter->second );
            m_registeredTypes.erase( iter );
        }

        TypeInfo const* TypeRegistry::GetTypeInfo( TypeID typeID ) const
        {
            KRG_ASSERT( typeID.IsValid() && !IsCoreType( typeID ) );
            auto iter = m_registeredTypes.find( typeID );
            if ( iter != m_registeredTypes.end() )
            {
                return iter->second;
            }
            else
            {
                return nullptr;
            }
        }

        ResolvedPropertyInfo TypeRegistry::ResolvePropertyPath( TypeInfo const* pTypeInfo, PropertyPath const& pathID ) const
        {
            ResolvedPropertyInfo resolvedPropertyInfo;

            TypeInfo const* pParentTypeInfo = pTypeInfo;
            PropertyInfo const* pFoundPropertyInfo = nullptr;
            int32 propertyOffset = 0;

            // Resolve property path
            size_t const numPathElements = pathID.GetNumElements();
            size_t const lastElementIdx = numPathElements - 1;
            for ( size_t i = 0; i < numPathElements; i++ )
            {
                pFoundPropertyInfo = pParentTypeInfo->GetPropertyInfo( pathID[i].m_propertyID );
                if ( pFoundPropertyInfo == nullptr )
                {
                    break;
                }

                // Increment the offset to take into account the parent types' offsets
                propertyOffset += pFoundPropertyInfo->m_offset;

                if ( i != lastElementIdx )
                {
                    // If this occurs, we have an invalid path as each element must contain other properties
                    if ( IsCoreType( pFoundPropertyInfo->m_typeID ) && !pFoundPropertyInfo->IsArrayProperty() )
                    {
                        KRG_LOG_WARNING( "TypeSystem", "Cant resolve malformed property path" ); \
                        pFoundPropertyInfo = nullptr;
                        break;
                    }

                    // Get the type desc of the property
                    pParentTypeInfo = GetTypeInfo( pFoundPropertyInfo->m_typeID );
                    if ( pParentTypeInfo == nullptr )
                    {
                        KRG_LOG_ERROR( "TypeSystem", "Cant resolve property path since it contains an unknown type" );
                        pFoundPropertyInfo = nullptr;
                    }
                }
            }

            // Fill out the resulting property info
            if ( pFoundPropertyInfo != nullptr )
            {
                resolvedPropertyInfo.m_pPropertyInfo = pFoundPropertyInfo;
                resolvedPropertyInfo.m_offset = propertyOffset;
            }

            return resolvedPropertyInfo;
        }

        bool TypeRegistry::IsTypeDerivedFrom( TypeID typeID, TypeID parentTypeID ) const
        {
            KRG_ASSERT( typeID.IsValid() && parentTypeID.IsValid() );
            KRG_ASSERT( !IsCoreType( typeID ) && !IsCoreType( parentTypeID ) );

            auto pTypeInfo = GetTypeInfo( typeID );
            KRG_ASSERT( pTypeInfo != nullptr );

            return pTypeInfo->IsDerivedFrom( parentTypeID );
        }

        TVector<TypeInfo const*> TypeRegistry::GetAllTypesWithMatchingMetadata( TBitFlags<ETypeInfoMetaData> metadataFlags ) const
        {
            TVector<TypeInfo const*> matchingTypes;

            for ( auto const& typeInfoPair : m_registeredTypes )
            {
                if ( typeInfoPair.second->m_metadata == metadataFlags )
                {
                    matchingTypes.emplace_back( typeInfoPair.second );
                }
            }

            return matchingTypes;
        }

        //-------------------------------------------------------------------------

        EnumInfo const* TypeRegistry::RegisterEnum( EnumInfo const& type )
        {
            KRG_ASSERT( type.m_ID.IsValid() );
            KRG_ASSERT( m_registeredEnums.find( type.m_ID ) == m_registeredEnums.end() );
            m_registeredEnums[type.m_ID] = KRG::New<EnumInfo>( type );
            return m_registeredEnums[type.m_ID];
        }

        void TypeRegistry::UnregisterEnum( TypeID const typeID )
        {
            KRG_ASSERT( typeID.IsValid() );
            auto iter = m_registeredEnums.find( typeID );
            KRG_ASSERT( iter != m_registeredEnums.end() );
            KRG::Delete<EnumInfo>( iter->second );
            m_registeredEnums.erase( iter );
        }

        EnumInfo const* TypeRegistry::GetEnumInfo( TypeID enumID ) const
        {
            KRG_ASSERT( enumID.IsValid() );
            auto iter = m_registeredEnums.find( enumID );
            if ( iter != m_registeredEnums.end() )
            {
                return iter->second;
            }
            else
            {
                return nullptr;
            }
        }

        //-------------------------------------------------------------------------

        void TypeRegistry::RegisterResourceTypeID( TypeID typeID, ResourceTypeID resourceTypeID )
        {
            KRG_ASSERT( typeID.IsValid() && !IsCoreType( typeID ) );
            KRG_ASSERT( resourceTypeID.IsValid() );
            KRG_ASSERT( m_registeredResourceTypes.find( typeID ) == m_registeredResourceTypes.end() );
            m_registeredResourceTypes.insert( eastl::pair<TypeID, ResourceTypeID>( typeID, resourceTypeID ) );
        }

        void TypeRegistry::UnregisterResourceTypeID( TypeID typeID )
        {
            KRG_ASSERT( typeID.IsValid() && !IsCoreType( typeID ) );
            auto iter = m_registeredResourceTypes.find( typeID );
            KRG_ASSERT( iter != m_registeredResourceTypes.end() );
            m_registeredResourceTypes.erase( iter );
        }

        ResourceTypeID TypeRegistry::GetResourceTypeIDForType( TypeID typeID ) const
        {
            ResourceTypeID resourceTypeID;

            auto iter = m_registeredResourceTypes.find( typeID );
            if ( iter != m_registeredResourceTypes.end() )
            {
                resourceTypeID = iter->second;
            }

            return resourceTypeID;
        }

        //-------------------------------------------------------------------------

        size_t TypeRegistry::GetTypeByteSize( TypeID typeID ) const
        {
            KRG_ASSERT( typeID.IsValid() );

            if ( IsCoreType( typeID ) )
            {
                return GetCoreTypeSize( typeID );
            }
            else
            {
                auto pChildTypeInfo = GetTypeInfo( typeID );
                KRG_ASSERT( pChildTypeInfo != nullptr );
                return pChildTypeInfo->m_size;
            }
        }
    }
}