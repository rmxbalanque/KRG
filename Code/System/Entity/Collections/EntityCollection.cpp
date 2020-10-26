#include "EntityCollection.h"
#include "EntityCollectionDescriptor.h"
#include "System/Entity/Entity.h"
#include "System/Entity/EntitySystem.h"
#include "System/TypeSystem/TypeValueConverter.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    template<typename T>
    static void SetPropertyValue( TypeSystem::TypeRegistry const& typeRegistry, PropertyDescriptor const& propertyValue, T* pRegisteredType )
    {
        KRG_ASSERT( pRegisteredType != nullptr );

        TypeSystem::ResolvedPropertyInfo const resolvedPropertyInfo = typeRegistry.ResolvePropertyPath( pRegisteredType->GetTypeInfo(), propertyValue.m_path );
        if ( !resolvedPropertyInfo.IsValid() )
        {
            KRG_LOG_WARNING( "Property Deserialization", "Unknown property encountered" );
            return;
        }

        TypeSystem::PropertyInfo const* pPropertyInfo = resolvedPropertyInfo.m_pPropertyInfo;
        Byte* pPropertyData = reinterpret_cast<Byte*>( pRegisteredType ) + resolvedPropertyInfo.m_offset;
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

    //-------------------------------------------------------------------------

    EntityCollection::EntityCollection( TypeSystem::TypeRegistry const& typeRegistry, UUID ID, EntityCollectionDescriptor const& entityCollectionTemplate )
        : m_ID( ID )
    {
        KRG_ASSERT( ID.IsValid() );

        //-------------------------------------------------------------------------
        // Create entities and components
        //-------------------------------------------------------------------------

        auto pEntityTypeInfo = Entity::StaticTypeInfo;
        KRG_ASSERT( pEntityTypeInfo != nullptr );

        for ( auto const& entityDesc : entityCollectionTemplate.m_entityDescriptors )
        {
            CreateEntityFromDescriptor( typeRegistry, entityDesc );
        }

        //-------------------------------------------------------------------------
        // Resolve spatial entity parents
        //-------------------------------------------------------------------------

        for ( auto const& entityAttachmentInfo : entityCollectionTemplate.m_entitySpatialAttachmentInfo )
        {
            KRG_ASSERT( entityAttachmentInfo.m_entityIdx != InvalidIndex && entityAttachmentInfo.m_parentEntityIdx != InvalidIndex );
        
            auto const& entityDesc = entityCollectionTemplate.m_entityDescriptors[entityAttachmentInfo.m_entityIdx];
            KRG_ASSERT( entityDesc.HasSpatialParent() );

            //-------------------------------------------------------------------------

            // The entity collection compiler will guaranteed that entities are always sorted so that parents are created/initialized before their attached entities
            KRG_ASSERT( entityAttachmentInfo.m_parentEntityIdx < entityAttachmentInfo.m_entityIdx );

            Entity* pEntity = m_entities[entityAttachmentInfo.m_entityIdx];
            KRG_ASSERT( pEntity->IsSpatialEntity() );

            Entity* pParentEntity = m_entities[entityAttachmentInfo.m_parentEntityIdx];
            KRG_ASSERT( pParentEntity->IsSpatialEntity() );

            Entity::CreateSpatialAttachment( pEntity, pParentEntity, entityDesc.m_attachmentSocketID );
        }
    }

    EntityCollection::~EntityCollection()
    {
        for ( auto& pEntity : m_entities )
        {
            KRG::Delete( pEntity );
        }

        m_entities.clear();
        m_entityLookupMap.clear();
    }

    Entity* EntityCollection::CreateEntityFromDescriptor( TypeSystem::TypeRegistry const& typeRegistry, EntityDescriptor const& entityDesc )
    {
        KRG_ASSERT( entityDesc.IsValid() );

        auto pEntityTypeInfo = Entity::StaticTypeInfo;
        KRG_ASSERT( pEntityTypeInfo != nullptr );

        // Create new entity
        //-------------------------------------------------------------------------

        auto pEntity = reinterpret_cast<Entity*>( pEntityTypeInfo->m_pTypeHelper->CreateType() );
        pEntity->m_ID = entityDesc.m_ID;
        pEntity->m_name = entityDesc.m_name;

        // Create entity components
        //-------------------------------------------------------------------------
        // Component descriptors are sorted during compilation, spatial components are first, followed by regular components

        for ( auto const& componentDesc : entityDesc.m_components )
        {
            TypeSystem::TypeInfo const* pTypeInfo = typeRegistry.GetTypeInfo( componentDesc.m_typeID );
            KRG_ASSERT( pTypeInfo != nullptr );

            auto pEntityComponent = reinterpret_cast<EntityComponent*>( pTypeInfo->m_pTypeHelper->CreateType() );
            KRG_ASSERT( pEntityComponent != nullptr );

            // Set IDs and add to component lists
            pEntityComponent->m_ID = componentDesc.m_ID;
            pEntityComponent->m_name = componentDesc.m_name;
            pEntityComponent->m_entityID = pEntity->m_ID;
            pEntity->m_components.push_back( pEntityComponent );

            // Apply property overrides
            for ( auto const& propertyValue : componentDesc.m_propertyValues )
            {
                SetPropertyValue( typeRegistry, propertyValue, pEntityComponent );
            }

            //-------------------------------------------------------------------------

            if ( componentDesc.IsSpatialComponent() )
            {
                // Set parent socket ID
                auto pSpatialEntityComponent = static_cast<SpatialEntityComponent*>( pEntityComponent );
                pSpatialEntityComponent->m_parentAttachmentSocketID = componentDesc.m_attachmentSocketID;

                // Set as root component
                if ( componentDesc.IsRootComponent() )
                {
                    KRG_ASSERT( pEntity->m_pRootSpatialComponent == nullptr );
                    pEntity->m_pRootSpatialComponent = pSpatialEntityComponent;
                }
            }
        }

        // Create component spatial hierarchy
        //-------------------------------------------------------------------------

        S32 const numComponents = (S32) pEntity->m_components.size();
        for ( S32 spatialComponentIdx = 0; spatialComponentIdx < entityDesc.m_numSpatialComponents; spatialComponentIdx++ )
        {
            auto const& spatialComponentDesc = entityDesc.m_components[spatialComponentIdx];
            KRG_ASSERT( spatialComponentDesc.IsSpatialComponent() );

            // Skip the root component
            if ( spatialComponentDesc.IsRootComponent() )
            {
                KRG_ASSERT( pEntity->GetRootSpatialComponent()->GetID() == spatialComponentDesc.m_ID );
                continue;
            }

            // Todo: profile this lookup and if it becomes too costly, pre-compute the parent indices and serialize them
            S32 const parentComponentIdx = entityDesc.FindComponentIndex( spatialComponentDesc.m_spatialParentID );
            KRG_ASSERT( parentComponentIdx != InvalidIndex );

            auto pParentSpatialComponent = static_cast<SpatialEntityComponent*>( pEntity->m_components[parentComponentIdx] );
            if ( spatialComponentDesc.m_spatialParentID == pParentSpatialComponent->GetID() )
            {
                auto pSpatialComponent = static_cast<SpatialEntityComponent*>( pEntity->m_components[spatialComponentIdx] );
                pSpatialComponent->m_pSpatialParent = pParentSpatialComponent;

                pParentSpatialComponent->m_spatialChildren.emplace_back( pSpatialComponent );
                break;
            }
        }

        // Create entity systems
        //-------------------------------------------------------------------------

        for ( auto const& systemDesc : entityDesc.m_systems )
        {
            TypeSystem::TypeInfo const* pTypeInfo = typeRegistry.GetTypeInfo( systemDesc.m_typeID );
            auto pEntitySystem = reinterpret_cast<IEntitySystem*>( pTypeInfo->m_pTypeHelper->CreateType() );
            KRG_ASSERT( pEntitySystem != nullptr );

            pEntity->m_systems.push_back( pEntitySystem );
        }

        // Add to collection
        //-------------------------------------------------------------------------

        AddEntity( pEntity );
        return pEntity;
    }

    void EntityCollection::AddEntity( Entity* pEntity )
    {
        // Ensure that the entity to add, is not already part of a collection and that it's deactivated
        KRG_ASSERT( pEntity != nullptr && !pEntity->IsInCollection() && pEntity->IsDeactivated() );
        KRG_ASSERT( !ContainsEntity( pEntity->m_ID ) );

        pEntity->m_collectionID = m_ID;
        m_entities.push_back( pEntity );
        m_entityLookupMap.insert( TPair<UUID, Entity*>( pEntity->m_ID, pEntity ) );
    }

    void EntityCollection::RemoveEntity( UUID entityID )
    {
        auto iter = m_entityLookupMap.find( entityID );
        KRG_ASSERT( iter != m_entityLookupMap.end() );

        auto pEntity = iter->second;
        KRG_ASSERT( pEntity->IsDeactivated() );

        m_entityLookupMap.erase( iter );
        m_entities.erase_first( pEntity );
    }
}