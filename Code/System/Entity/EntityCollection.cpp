#include "EntityCollection.h"
#include "Serialization/EntityArchive.h"
#include "System/Entity/EntitySystem.h"
#include "System/TypeSystem/TypeValueConverter.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace
    {
        template<typename T>
        static void SetPropertyValue( TypeSystem::TypeRegistry const& typeRegistry, Serialization::SerializedPropertyValue const& propertyValue, T* pRegisteredType )
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

            // Resolve ResourceIDs to ResourcePtrs
            //-------------------------------------------------------------------------

            if ( pPropertyInfo->m_typeID == TypeSystem::CoreTypes::TResourcePtr || pPropertyInfo->m_typeID == TypeSystem::CoreTypes::ResourcePtr )
            {
                // Get the serialize resource ID and validate the resource type vs. the expected type
                ResourceID const deserializedResourceID( (char const*) propertyValue.m_valueData.data() );

                TypeSystem::TypeInfo const* pImmediateParentTypeInfo = typeRegistry.GetTypeInfo( pPropertyInfo->m_parentTypeID );
                KRG_ASSERT( pImmediateParentTypeInfo != nullptr );
                ResourceTypeID const expectedResourceTypeID = pImmediateParentTypeInfo->m_pTypeHelper->GetExpectedResourceTypeForProperty( pImmediateParent, pPropertyInfo->m_ID );

                // Set the resource ptr value
                if ( expectedResourceTypeID == ResourceTypeID::Unknown || deserializedResourceID.GetResourceTypeID() == expectedResourceTypeID )
                {
                    Resource::ResourcePtr* pResourcePtr = reinterpret_cast<Resource::ResourcePtr*>( pPropertyData );
                    *pResourcePtr = Resource::ResourcePtr( deserializedResourceID );
                }
                else
                {
                    KRG_LOG_WARNING( "Property Deserialization: ", "Mismatched resource type ID for property %s, expected %s and received %s", pPropertyInfo->m_ID.ToString(), expectedResourceTypeID.ToString().c_str(), deserializedResourceID.GetResourceTypeID().ToString().c_str() );
                }
            }

            // Resolve enum string values
            //-------------------------------------------------------------------------

            else if ( pPropertyInfo->IsEnumProperty() )
            {
                auto pEnumInfo = typeRegistry.GetEnumInfo( pPropertyInfo->m_typeID );
                KRG_ASSERT( pEnumInfo != nullptr );

                StringID const enumID( *reinterpret_cast<U32 const*>( propertyValue.m_valueData.data() ) );
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
                TypeSystem::ConvertByteArrayToValue( pPropertyInfo->m_typeID, propertyValue.m_valueData, pPropertyData );
            }
        }
    }

    //-------------------------------------------------------------------------

    EntityCollection::EntityCollection( UUID ID )
        : m_ID( ID )
    {}

    EntityCollection::EntityCollection( TypeSystem::TypeRegistry const& typeRegistry, UUID ID, Serialization::EntityArchive const& entityArchive )
        : EntityCollection( ID )
    {
        KRG_ASSERT( ID.IsValid() );

        //-------------------------------------------------------------------------
        // Create entities and components
        //-------------------------------------------------------------------------

        auto pEntityTypeInfo = Entity::TypeInfoPtr;
        KRG_ASSERT( pEntityTypeInfo != nullptr );

        TVector<TPair<Entity*, Serialization::SerializedEntity const&>> entitiesThatNeedSpatialAttachment;

        S32 itemIdx = 0;
        for ( auto const& serializedEntity : entityArchive.GetEntityDescriptors() )
        {

            // Create new entity
            //-------------------------------------------------------------------------

            auto pEntity = reinterpret_cast<Entity*>( pEntityTypeInfo->m_pTypeHelper->CreateType() );

            // Set IDs and add to template
            pEntity->m_ID = serializedEntity.m_ID;
            pEntity->m_name = serializedEntity.m_name;

            if ( serializedEntity.HasSpatialParent() )
            {
                entitiesThatNeedSpatialAttachment.push_back( TPair<Entity*, Serialization::SerializedEntity const&>( pEntity, serializedEntity ) );
            }

            m_entities.push_back( pEntity );
            m_entityLookupMap.insert( TPair<UUID, Entity*>( serializedEntity.m_ID, pEntity ) );

            itemIdx++;

            // Create entity components
            //-------------------------------------------------------------------------

            auto CreateBaseComponent = [this, &typeRegistry, &pEntity] ( Serialization::SerializedEntityComponent const& componentDesc )
            {
                TypeSystem::TypeInfo const* pTypeInfo = typeRegistry.GetTypeInfo( componentDesc.m_typeID );
                KRG_ASSERT( pTypeInfo != nullptr );

                auto pEntityComponent = reinterpret_cast<EntityComponent*>( pTypeInfo->m_pTypeHelper->CreateType() );
                KRG_ASSERT( pEntityComponent != nullptr );

                // Set IDs and add to component lists
                pEntityComponent->m_ID = componentDesc.m_ID;
                pEntityComponent->m_name = componentDesc.m_name;
                pEntity->m_components.push_back( pEntityComponent );

                // Apply property overrides
                for ( auto const& propertyValue : componentDesc.m_propertyValues )
                {
                    SetPropertyValue( typeRegistry, propertyValue, pEntityComponent );
                }

                return pEntityComponent;
            };

            //-------------------------------------------------------------------------

            // Entity Components
            for ( auto const& serializedComponent : serializedEntity.m_components )
            {
                CreateBaseComponent( serializedComponent );
                itemIdx++;
            }

            // Spatial Components
            U32 const spatialComponentStartIdx = (U32) pEntity->m_components.size();
            for ( auto const& serializedSpatialComponent : serializedEntity.m_spatialComponents )
            {
                // Set parent socket ID
                auto pSpatialEntityComponent = static_cast<SpatialEntityComponent*>( CreateBaseComponent( serializedSpatialComponent ) );
                pSpatialEntityComponent->m_parentAttachmentSocketID = serializedSpatialComponent.m_attachmentSocketID;
                pSpatialEntityComponent->m_isRootComponent = serializedSpatialComponent.IsRootComponent();

                // Set Entity Root Component
                if( !serializedSpatialComponent.m_spatialParentID.IsValid() )
                {
                    KRG_ASSERT( pEntity->m_pRootSpatialComponent == nullptr );
                    pEntity->m_pRootSpatialComponent = pSpatialEntityComponent;
                }

                itemIdx++;
            }

            // Create spatial hierarchy
            //-------------------------------------------------------------------------

            U32 const numComponents = (U32) pEntity->m_components.size();
            for( auto spatialComponentIdx = spatialComponentStartIdx; spatialComponentIdx < numComponents; spatialComponentIdx++ )
            {
                U32 const serializedSpatialComponentIdx = spatialComponentIdx - spatialComponentStartIdx;
                auto const& serializedSpatialComponent = serializedEntity.m_spatialComponents[serializedSpatialComponentIdx];

                // If we have a valid parent ID, resolve it
                if( serializedSpatialComponent.m_spatialParentID.IsValid() )
                {
                    auto pSpatialComponent = static_cast<SpatialEntityComponent*>( pEntity->m_components[spatialComponentIdx].GetRawPtr() );

                    // Check all other spatial components for this ID
                    for ( auto otherSpatialComponentIdx = spatialComponentStartIdx; otherSpatialComponentIdx < numComponents; otherSpatialComponentIdx++ )
                    {
                        if( otherSpatialComponentIdx == spatialComponentIdx )
                        {
                            continue;
                        }

                        // If this our parent, set the spatial parent pointer, and add ourselves to it's children
                        auto pOtherSpatialComponent = static_cast<SpatialEntityComponent*>( pEntity->m_components[otherSpatialComponentIdx].GetRawPtr() );
                        if( serializedSpatialComponent.m_spatialParentID == pOtherSpatialComponent->GetID() )
                        {
                            pOtherSpatialComponent->m_spatialChildren.emplace_back( pSpatialComponent );
                            pSpatialComponent->m_pSpatialParent = pOtherSpatialComponent;
                            break;
                        }
                    }
                }
            }

            // Create entity systems
            //-------------------------------------------------------------------------

            for ( auto const& systemDesc : serializedEntity.m_systems )
            {
                TypeSystem::TypeInfo const* pTypeInfo = typeRegistry.GetTypeInfo( systemDesc.m_typeID );
                auto pEntitySystem = reinterpret_cast<IEntitySystem*>( pTypeInfo->m_pTypeHelper->CreateType() );
                KRG_ASSERT( pEntitySystem != nullptr );

                pEntity->m_systems.push_back( pEntitySystem );
                itemIdx++;
            }
        }

        //-------------------------------------------------------------------------
        // Resolve spatial entity parents
        //-------------------------------------------------------------------------

        for ( auto const& pair : entitiesThatNeedSpatialAttachment )
        {
            Entity* pEntity = pair.first;
            Serialization::SerializedEntity const& entityDesc = pair.second;

            Entity* pParentEntity = FindEntity( entityDesc.m_spatialParentID );
            KRG_ASSERT( pParentEntity != nullptr && pParentEntity->IsSpatialEntity() );

            // The entity collection compiler will guaranteed that entities are always sorted so that parents are created/initialized before their attached entities
            KRG_ASSERT( pParentEntity < pEntity );

            Entity::CreateSpatialAttachment( pEntity, pParentEntity, entityDesc.m_attachmentSocketID );
        }
    }

    EntityCollection::~EntityCollection()
    {
        // Execute all destructors
        //-------------------------------------------------------------------------

        for ( auto& pEntity : m_entities )
        {
            KRG_ASSERT( !pEntity->IsInitialized() );

            Entity::BreakAllEntitySpatialAttachments( pEntity );

            // Destroy Systems
            for ( auto& pSystem : pEntity->m_systems )
            {
                KRG::Delete( pSystem );
            }

            // Destroy components
            for ( auto& pComponent : pEntity->m_components )
            {
                auto pRawComponent = pComponent.GetRawPtr();
                KRG::Delete( pRawComponent );
            }

            KRG::Delete( pEntity );
        }
        m_entities.clear();
    }
}