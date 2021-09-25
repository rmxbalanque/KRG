#include "EntityCollection.h"
#include "EntityCollectionDescriptor.h"
#include "Engine/Core/Entity/Entity.h"
#include "Engine/Core/Entity/EntitySystem.h"
#include "System/Core/Profiling/Profiling.h"
#include "System/Core/Threading/TaskSystem.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    Entity* EntityCollection::CreateEntityFromDescriptor( TypeSystem::TypeRegistry const& typeRegistry, EntityDescriptor const& entityDesc )
    {
        KRG_ASSERT( entityDesc.IsValid() );

        auto pEntityTypeInfo = Entity::s_pTypeInfo;
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
            auto pEntityComponent = TypeSystem::TypeCreator::CreateTypeFromDescriptor<EntityComponent>( typeRegistry, componentDesc );
            KRG_ASSERT( pEntityComponent != nullptr );

            TypeSystem::TypeInfo const* pTypeInfo = pEntityComponent->GetTypeInfo();
            KRG_ASSERT( pTypeInfo != nullptr );

            // Set IDs and add to component lists
            pEntityComponent->m_ID = componentDesc.m_ID;
            pEntityComponent->m_name = componentDesc.m_name;
            pEntityComponent->m_entityID = pEntity->m_ID;
            pEntity->m_components.push_back( pEntityComponent );

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

        int32 const numComponents = (int32) pEntity->m_components.size();
        for ( int32 spatialComponentIdx = 0; spatialComponentIdx < entityDesc.m_numSpatialComponents; spatialComponentIdx++ )
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
            int32 const parentComponentIdx = entityDesc.FindComponentIndex( spatialComponentDesc.m_spatialParentID );
            KRG_ASSERT( parentComponentIdx != InvalidIndex );

            auto pParentSpatialComponent = static_cast<SpatialEntityComponent*>( pEntity->m_components[parentComponentIdx] );
            if ( spatialComponentDesc.m_spatialParentID == pParentSpatialComponent->GetID() )
            {
                auto pSpatialComponent = static_cast<SpatialEntityComponent*>( pEntity->m_components[spatialComponentIdx] );
                pSpatialComponent->m_pSpatialParent = pParentSpatialComponent;

                pParentSpatialComponent->m_spatialChildren.emplace_back( pSpatialComponent );
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

        return pEntity;
    }

    //-------------------------------------------------------------------------

    EntityCollection::EntityCollection( TypeSystem::TypeRegistry const& typeRegistry, UUID ID, EntityCollectionDescriptor const& entityCollectionTemplate )
        : m_ID( ID )
    {
        KRG_ASSERT( ID.IsValid() );
        CreateAllEntitiesSequential( typeRegistry, entityCollectionTemplate );
        ResolveEntitySpatialAttachments( entityCollectionTemplate );
    }

    EntityCollection::~EntityCollection()
    {
        DestroyAllEntities();
    }

    void EntityCollection::AddEntity( Entity* pEntity )
    {
        // Ensure that the entity to add, is not already part of a collection and that it's deactivated
        KRG_ASSERT( pEntity != nullptr && !pEntity->IsInCollection() && !pEntity->IsActivated() );
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
        KRG_ASSERT( !pEntity->IsActivated() );

        m_entityLookupMap.erase( iter );
        m_entities.erase_first( pEntity );
    }

    //-------------------------------------------------------------------------

    void EntityCollection::CreateAllEntitiesSequential( TypeSystem::TypeRegistry const& typeRegistry, EntityCollectionDescriptor const& entityCollectionTemplate )
    {
        KRG_PROFILE_FUNCTION_SCENE();

        m_entities.reserve( entityCollectionTemplate.m_entityDescriptors.size() );
        m_entityLookupMap.reserve( entityCollectionTemplate.m_entityDescriptors.size() );

        for ( auto const& entityDesc : entityCollectionTemplate.m_entityDescriptors )
        {
            auto pEntity = CreateEntityFromDescriptor( typeRegistry, entityDesc );
            AddEntity( pEntity );
        }
    }

    void EntityCollection::CreateAllEntitiesParallel( TaskSystem& taskSystem, TypeSystem::TypeRegistry const& typeRegistry, EntityCollectionDescriptor const& entityCollectionTemplate )
    {
        KRG_PROFILE_FUNCTION_SCENE();

        //-------------------------------------------------------------------------

        struct EntityCreationTask : public IAsyncTask
        {
            EntityCreationTask( TypeSystem::TypeRegistry const& typeRegistry, TVector<EntityDescriptor> const& descriptors, EntityCollection& targetCollection )
                : m_typeRegistry( typeRegistry )
                , m_descriptors( descriptors )
                , m_targetCollection( targetCollection )
            {
                m_SetSize = (uint32) descriptors.size();
                m_MinRange = 10;
            }

            virtual void ExecuteRange( TaskSetPartition range, uint32 threadnum ) override final
            {
                KRG_PROFILE_SCOPE_SCENE( "Entity Creation Task" );
                for ( uint64 i = range.start; i < range.end; ++i )
                {
                    m_targetCollection.m_entities[i] = EntityCollection::CreateEntityFromDescriptor( m_typeRegistry, m_descriptors[i] );
                }
            }

        private:

            TypeSystem::TypeRegistry const&         m_typeRegistry;
            TVector<EntityDescriptor> const&        m_descriptors;
            EntityCollection&                       m_targetCollection;
        };

        //-------------------------------------------------------------------------

        // Create all entities in parallel
        EntityCreationTask updateTask( typeRegistry, entityCollectionTemplate.m_entityDescriptors, *this );
        m_entities.resize( entityCollectionTemplate.m_entityDescriptors.size() );
        taskSystem.ScheduleTask( &updateTask );
        taskSystem.WaitForTask( &updateTask );

        // Add all entities to lookup map
        m_entityLookupMap.reserve( entityCollectionTemplate.m_entityDescriptors.size() );
        for ( auto pEntity : m_entities )
        {
            m_entityLookupMap.insert( TPair<UUID, Entity*>( pEntity->m_ID, pEntity ) );
        }
    }

    void EntityCollection::ResolveEntitySpatialAttachments( EntityCollectionDescriptor const& entityCollectionTemplate )
    {
        KRG_PROFILE_FUNCTION_SCENE();

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

            pEntity->SetSpatialParent( pParentEntity, entityDesc.m_attachmentSocketID );
        }
    }

    void EntityCollection::DestroyAllEntities()
    {
        for ( auto& pEntity : m_entities )
        {
            KRG::Delete( pEntity );
        }

        m_entities.clear();
        m_entityLookupMap.clear();
    }
}