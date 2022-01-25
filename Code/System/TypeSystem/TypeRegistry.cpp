#include "TypeRegistry.h"
#include "PropertyPath.h"
#include "System/Core/Logging/Log.h"
#include "TypeRegistrationMacros.h"
#include "TypeHelpers.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    TypeRegistry::TypeRegistry()
    {
        TypeHelpers::TTypeHelper<IRegisteredType>::RegisterType( *this );
    }

    TypeRegistry::~TypeRegistry()
    {
        TypeHelpers::TTypeHelper<IRegisteredType>::UnregisterType( *this );
        KRG_ASSERT( m_registeredEnums.empty() && m_registeredTypes.empty() && m_registeredResourceTypes.empty() );
    }

    //-------------------------------------------------------------------------

    TypeInfo const* TypeRegistry::RegisterType( TypeInfo const& type )
    {
        KRG_ASSERT( type.m_ID.IsValid() && !CoreTypeRegistry::IsCoreType( type.m_ID ) );
        KRG_ASSERT( m_registeredTypes.find( type.m_ID ) == m_registeredTypes.end() );
        m_registeredTypes.insert( eastl::pair<TypeID, TypeInfo*>( type.m_ID, KRG::New<TypeInfo>( type ) ) );
        return m_registeredTypes[type.m_ID];
    }

    void TypeRegistry::UnregisterType( TypeID const typeID )
    {
        KRG_ASSERT( typeID.IsValid() && !CoreTypeRegistry::IsCoreType( typeID ) );
        auto iter = m_registeredTypes.find( typeID );
        KRG_ASSERT( iter != m_registeredTypes.end() );
        KRG::Delete<TypeInfo>( iter->second );
        m_registeredTypes.erase( iter );
    }

    TypeInfo const* TypeRegistry::GetTypeInfo( TypeID typeID ) const
    {
        KRG_ASSERT( typeID.IsValid() && !CoreTypeRegistry::IsCoreType( typeID ) );
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

    PropertyInfo const* TypeRegistry::ResolvePropertyPath( TypeInfo const* pTypeInfo, PropertyPath const& pathID ) const
    {
        TypeInfo const* pParentTypeInfo = pTypeInfo;
        PropertyInfo const* pFoundPropertyInfo = nullptr;

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

            if ( i != lastElementIdx )
            {
                // If this occurs, we have an invalid path as each element must contain other properties
                if ( CoreTypeRegistry::IsCoreType( pFoundPropertyInfo->m_typeID ) && !pFoundPropertyInfo->IsArrayProperty() )
                {
                    KRG_LOG_WARNING( "TypeSystem", "Cant resolve malformed property path" );
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

        return pFoundPropertyInfo;
    }

    bool TypeRegistry::IsTypeDerivedFrom( TypeID typeID, TypeID parentTypeID ) const
    {
        KRG_ASSERT( typeID.IsValid() && parentTypeID.IsValid() );
        KRG_ASSERT( !CoreTypeRegistry::IsCoreType( typeID ) && !CoreTypeRegistry::IsCoreType( parentTypeID ) );

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


    TVector<TypeInfo const*> TypeRegistry::GetAllTypes( bool includeAbstractTypes, bool sortAlphabetically ) const
    {
        TVector<TypeInfo const*> types;

        for ( auto const& typeInfoPair : m_registeredTypes )
        {
            if ( !includeAbstractTypes && typeInfoPair.second->IsAbstractType() )
            {
                continue;
            }

            types.emplace_back( typeInfoPair.second );
        }

        if ( sortAlphabetically )
        {
            auto sortPredicate = [] ( TypeSystem::TypeInfo const* const& pTypeInfoA, TypeSystem::TypeInfo const* const& pTypeInfoB )
            {
                #if KRG_DEVELOPMENT_TOOLS
                return pTypeInfoA->m_friendlyName < pTypeInfoB->m_friendlyName;
                #else
                return strcmp( pTypeInfoA->m_ID.c_str(), pTypeInfoB->m_ID.c_str() );
                #endif
            };

            eastl::sort( types.begin(), types.end(), sortPredicate );
        }

        return types;
    }

    TVector<TypeInfo const*> TypeRegistry::GetAllDerivedTypes( TypeID parentTypeID, bool includeParentTypeInResults, bool includeAbstractTypes, bool sortAlphabetically ) const
    {
        TVector<TypeInfo const*> matchingTypes;

        for ( auto const& typeInfoPair : m_registeredTypes )
        {
            if ( !includeParentTypeInResults && typeInfoPair.first == parentTypeID )
            {
                continue;
            }

            if ( !includeAbstractTypes && typeInfoPair.second->IsAbstractType() )
            {
                continue;
            }

            if ( typeInfoPair.second->IsDerivedFrom( parentTypeID ) )
            {
                matchingTypes.emplace_back( typeInfoPair.second );
            }
        }

        if ( sortAlphabetically )
        {
            auto sortPredicate = [] ( TypeSystem::TypeInfo const* const& pTypeInfoA, TypeSystem::TypeInfo const* const& pTypeInfoB )
            {
                #if KRG_DEVELOPMENT_TOOLS
                return pTypeInfoA->m_friendlyName < pTypeInfoB->m_friendlyName;
                #else
                return strcmp( pTypeInfoA->m_ID.c_str(), pTypeInfoB->m_ID.c_str() );
                #endif
            };

            eastl::sort( matchingTypes.begin(), matchingTypes.end(), sortPredicate );
        }

        return matchingTypes;
    }

    TInlineVector<KRG::TypeSystem::TypeID, 5> TypeRegistry::GetAllCastableTypes( IRegisteredType const* pType ) const
    {
        struct Helper
        {
            static void RecursivelyFindAllParents( TypeInfo const* pTypeInfo, TInlineVector<KRG::TypeSystem::TypeID, 5>& outParentTypeIDs )
            {
                outParentTypeIDs.emplace_back( pTypeInfo->m_ID );

                for ( auto pParentTypeInfo : pTypeInfo->m_parentTypes )
                {
                    RecursivelyFindAllParents( pParentTypeInfo, outParentTypeIDs );
                }
            }
        };

        //-------------------------------------------------------------------------

        KRG_ASSERT( pType != nullptr );
        TInlineVector<KRG::TypeSystem::TypeID, 5> parentTypeIDs;
        Helper::RecursivelyFindAllParents( pType->GetTypeInfo(), parentTypeIDs );
        return parentTypeIDs;
    }

    bool TypeRegistry::AreTypesInTheSameHierarchy( TypeID typeA, TypeID typeB ) const 
    {
        auto pTypeInfoA = GetTypeInfo( typeA );
        auto pTypeInfoB = GetTypeInfo( typeB );
        return AreTypesInTheSameHierarchy( pTypeInfoA, pTypeInfoB );
    }

    bool TypeRegistry::AreTypesInTheSameHierarchy( TypeInfo const* pTypeInfoA, TypeInfo const* pTypeInfoB ) const
    {
        if ( pTypeInfoA->IsDerivedFrom( pTypeInfoB->m_ID ) )
        {
            return true;
        }

        if ( pTypeInfoB->IsDerivedFrom( pTypeInfoA->m_ID ) )
        {
            return true;
        }

        return false;
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

    void TypeRegistry::RegisterResourceTypeID( ResourceInfo const& resourceInfo )
    {
        KRG_ASSERT( resourceInfo.IsValid() && !CoreTypeRegistry::IsCoreType( resourceInfo.m_typeID ) );
        KRG_ASSERT( m_registeredResourceTypes.find( resourceInfo.m_typeID ) == m_registeredResourceTypes.end() );
        m_registeredResourceTypes.insert( eastl::pair<TypeID, ResourceInfo>( resourceInfo.m_typeID, resourceInfo ) );
    }

    void TypeRegistry::UnregisterResourceTypeID( TypeID typeID )
    {
        KRG_ASSERT( typeID.IsValid() && !CoreTypeRegistry::IsCoreType( typeID ) );
        auto iter = m_registeredResourceTypes.find( typeID );
        KRG_ASSERT( iter != m_registeredResourceTypes.end() );
        m_registeredResourceTypes.erase( iter );
    }

    bool TypeRegistry::IsRegisteredResourceType( ResourceTypeID resourceTypeID ) const
    {
        for ( auto const& pair : m_registeredResourceTypes )
        {
            if ( pair.second.m_resourceTypeID == resourceTypeID )
            {
                return true;
            }
        }

        return false;
    }

    ResourceInfo const* TypeRegistry::GetResourceInfoForType( TypeID typeID ) const
    {
        auto iter = m_registeredResourceTypes.find( typeID );
        if ( iter != m_registeredResourceTypes.end() )
        {
            return &iter->second;
        }

        //-------------------------------------------------------------------------

        KRG_HALT();
        return nullptr;
    }

    ResourceInfo const* TypeRegistry::GetResourceInfoForResourceType( ResourceTypeID resourceTypeID ) const
    {
        for ( auto const& resourceInfo : m_registeredResourceTypes )
        {
            if ( resourceInfo.second.m_resourceTypeID == resourceTypeID )
            {
                return &resourceInfo.second;
            }
        }

        //-------------------------------------------------------------------------

        KRG_HALT();
        return nullptr;
    }

    //-------------------------------------------------------------------------

    size_t TypeRegistry::GetTypeByteSize( TypeID typeID ) const
    {
        KRG_ASSERT( typeID.IsValid() );

        if ( IsCoreType( typeID ) )
        {
            return CoreTypeRegistry::GetTypeSize( typeID );
        }
        else
        {
            auto pChildTypeInfo = GetTypeInfo( typeID );
            KRG_ASSERT( pChildTypeInfo != nullptr );
            return pChildTypeInfo->m_size;
        }
    }
}