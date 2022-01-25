#include "EntityDescriptors.h"
#include "System/Core/Logging/Log.h"
#include "Entity.h"
#include "System/Core/Profiling/Profiling.h"
#include "System/Core/Threading/TaskSystem.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    int32 EntityDescriptor::FindComponentIndex( StringID const& componentName ) const
    {
        KRG_ASSERT( componentName.IsValid() );

        int32 const numComponents = (int32) m_components.size();
        for ( int32 i = 0; i < numComponents; i++ )
        {
            if ( m_components[i].m_name == componentName )
            {
                return i;
            }
        }

        return InvalidIndex;
    }
}

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    void EntityCollectionDescriptor::Reserve( int32 numEntities )
    {
        m_entityDescriptors.reserve( numEntities );
        m_entityLookupMap.reserve( numEntities );
    }

    void EntityCollectionDescriptor::GenerateSpatialAttachmentInfo()
    {
        m_entitySpatialAttachmentInfo.clear();
        m_entitySpatialAttachmentInfo.reserve( m_entityDescriptors.size() );

        int32 const numEntities = (int32) m_entityDescriptors.size();
        for ( int32 i = 0; i < numEntities; i++ )
        {
            auto const& entityDesc = m_entityDescriptors[i];
            if ( !entityDesc.IsSpatialEntity() || !entityDesc.HasSpatialParent() )
            {
                continue;
            }

            //-------------------------------------------------------------------------

            SpatialAttachmentInfo attachmentInfo;
            attachmentInfo.m_entityIdx = i;
            attachmentInfo.m_parentEntityIdx = FindEntityIndex( entityDesc.m_spatialParentName );

            if ( attachmentInfo.m_parentEntityIdx != InvalidIndex )
            {
                m_entitySpatialAttachmentInfo.push_back( attachmentInfo );
            }
        }
    }

    TVector<EntityCollectionDescriptor::SearchResult> EntityCollectionDescriptor::GetComponentsOfType( TypeSystem::TypeRegistry const& typeRegistry, TypeSystem::TypeID typeID, bool allowDerivedTypes )
    {
        TVector<SearchResult> foundComponents;

        for ( auto& entityDesc : m_entityDescriptors )
        {
            for ( auto& componentDesc : entityDesc.m_components )
            {
                if ( componentDesc.m_typeID == typeID )
                {
                    auto& result = foundComponents.emplace_back( SearchResult() );
                    result.m_pEntity = &entityDesc;
                    result.m_pComponent = &componentDesc;
                }
                else if ( allowDerivedTypes )
                {
                    auto pTypeInfo = typeRegistry.GetTypeInfo( componentDesc.m_typeID );
                    KRG_ASSERT( pTypeInfo != nullptr );

                    if ( pTypeInfo->IsDerivedFrom( typeID ) )
                    {
                        auto& result = foundComponents.emplace_back( SearchResult() );
                        result.m_pEntity = &entityDesc;
                        result.m_pComponent = &componentDesc;
                    }
                }
            }
        }

        //-------------------------------------------------------------------------

        return foundComponents;
    }

    //-------------------------------------------------------------------------

    TVector<Entity*> EntityCollectionDescriptor::InstantiateCollection( TaskSystem* pTaskSystem, TypeSystem::TypeRegistry const& typeRegistry ) const
    {
        KRG_PROFILE_SCOPE_SCENE( "Instantiate Entity Collection" );

        int32 const numEntitiesToCreate = (int32) m_entityDescriptors.size();
        TVector<Entity*> createdEntities;
        createdEntities.resize( numEntitiesToCreate );

        //-------------------------------------------------------------------------

        // For small number of entities, just create them inline!
        if ( pTaskSystem == nullptr || numEntitiesToCreate <= 5 )
        {
            for ( auto i = 0; i < numEntitiesToCreate; i++ )
            {
                createdEntities[i] = Entity::CreateFromDescriptor( typeRegistry, m_entityDescriptors[i] );
            }
        }
        else // Go wide and create all entities in parallel
        {
            struct EntityCreationTask : public ITaskSet
            {
                EntityCreationTask( TypeSystem::TypeRegistry const& typeRegistry, TVector<EntityDescriptor> const& descriptors, TVector<Entity*>& createdEntities )
                    : m_typeRegistry( typeRegistry )
                    , m_descriptors( descriptors )
                    , m_createdEntities( createdEntities )
                {
                    m_SetSize = (uint32) descriptors.size();
                    m_MinRange = 10;
                }

                virtual void ExecuteRange( TaskSetPartition range, uint32 threadnum ) override final
                {
                    KRG_PROFILE_SCOPE_SCENE( "Entity Creation Task" );
                    for ( uint64 i = range.start; i < range.end; ++i )
                    {
                        m_createdEntities[i] = Entity::CreateFromDescriptor( m_typeRegistry, m_descriptors[i] );
                    }
                }

            private:

                TypeSystem::TypeRegistry const&         m_typeRegistry;
                TVector<EntityDescriptor> const&        m_descriptors;
                TVector<Entity*>&                       m_createdEntities;
            };

            //-------------------------------------------------------------------------

            // Create all entities in parallel
            EntityCreationTask updateTask( typeRegistry, m_entityDescriptors, createdEntities );
            pTaskSystem->ScheduleTask( &updateTask );
            pTaskSystem->WaitForTask( &updateTask );
        }

        // Resolve spatial connections
        //-------------------------------------------------------------------------

        {
            KRG_PROFILE_SCOPE_SCENE( "Resolve spatial connections" );

            for ( auto const& entityAttachmentInfo : m_entitySpatialAttachmentInfo )
            {
                KRG_ASSERT( entityAttachmentInfo.m_entityIdx != InvalidIndex && entityAttachmentInfo.m_parentEntityIdx != InvalidIndex );

                auto const& entityDesc = m_entityDescriptors[entityAttachmentInfo.m_entityIdx];
                KRG_ASSERT( entityDesc.HasSpatialParent() );

                // The entity collection compiler will guaranteed that entities are always sorted so that parents are created/initialized before their attached entities
                KRG_ASSERT( entityAttachmentInfo.m_parentEntityIdx < entityAttachmentInfo.m_entityIdx );

                //-------------------------------------------------------------------------

                Entity* pEntity = createdEntities[entityAttachmentInfo.m_entityIdx];
                KRG_ASSERT( pEntity->IsSpatialEntity() );

                Entity* pParentEntity = createdEntities[entityAttachmentInfo.m_parentEntityIdx];
                KRG_ASSERT( pParentEntity->IsSpatialEntity() );

                pEntity->SetSpatialParent( pParentEntity, entityDesc.m_attachmentSocketID );
            }
        }

        //-------------------------------------------------------------------------

        return createdEntities;
    }
}