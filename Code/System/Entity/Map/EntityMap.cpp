#include "EntityMap.h"
#include "System/Resource/ResourceSystem.h"
#include "System/Entity/Entity.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    EntityMap::EntityMap()
        : m_isTransientMap( true )
        , m_entityUpdateEventBindingID( Entity::OnEntityStateUpdated().Bind( [this] ( Entity* pEntity ) { OnEntityStateUpdated( pEntity ); } ) )
    {}

    EntityMap::EntityMap( ResourceID mapResourceID )
        : m_pMapDesc( mapResourceID )
        , m_entityUpdateEventBindingID( Entity::OnEntityStateUpdated().Bind( [this] ( Entity* pEntity ) { OnEntityStateUpdated( pEntity ); } ) )
    {}

    EntityMap::EntityMap( EntityMap const& map )
        : m_entityUpdateEventBindingID( Entity::OnEntityStateUpdated().Bind( [this] ( Entity* pEntity ) { OnEntityStateUpdated( pEntity ); } ) )
    {
        operator=( map );
    }

    EntityMap::EntityMap( EntityMap&& map )
        : m_entityUpdateEventBindingID( Entity::OnEntityStateUpdated().Bind( [this] ( Entity* pEntity ) { OnEntityStateUpdated( pEntity ); } ) )
    {
        operator=( eastl::move( map ) );
    }

    EntityMap::~EntityMap()
    {
        KRG_ASSERT( m_pCollection == nullptr && IsUnloaded() );
        Entity::OnEntityStateUpdated().Unbind( m_entityUpdateEventBindingID );
    }

    //-------------------------------------------------------------------------

    EntityMap& EntityMap::operator=( EntityMap const& map )
    {
        // Only allow copy constructor for unloaded maps
        KRG_ASSERT( m_status == Status::Unloaded && map.m_status == Status::Unloaded );

        m_pMapDesc = map.m_pMapDesc;
        const_cast<bool&>( m_isTransientMap ) = map.m_isTransientMap;
        return *this;
    }

    EntityMap& EntityMap::operator=( EntityMap&& map )
    {
        m_pMapDesc = eastl::move( map.m_pMapDesc );
        m_pCollection = map.m_pCollection;
        m_loadingEntities = eastl::move( map.m_loadingEntities );
        m_entitiesToReload = eastl::move( map.m_entitiesToReload );
        m_status = map.m_status;
        m_unloadRequested = map.m_unloadRequested;
        const_cast<bool&>( m_isTransientMap ) = map.m_isTransientMap;

        map.m_pCollection = nullptr;
        map.m_status = Status::Unloaded;
        return *this;
    }

    //-------------------------------------------------------------------------

    void EntityMap::AddEntity( Entity* pEntity )
    {
        // Ensure that the entity to add, is not already part of a collection and that it's deactivated
        KRG_ASSERT( pEntity != nullptr && !pEntity->IsInCollection() && !pEntity->HasRequestedComponentLoad() );
        KRG_ASSERT( !VectorContains( m_entitiesToAdd, pEntity ) );
        m_entitiesToAdd.emplace_back( pEntity );
    }

    Entity* EntityMap::RemoveEntity( UUID entityID )
    {
        Entity* pEntityToRemove = nullptr;

        // Cancel add request if the collection doesnt exist yet
        if ( m_pCollection != nullptr )
        {
            S32 const entityIdx = VectorFindIndex( m_entitiesToAdd, entityID, [] ( Entity* pEntity, UUID entityID ) { return pEntity->GetID() == entityID; } );
            KRG_ASSERT( entityIdx != InvalidIndex );
            pEntityToRemove = m_entitiesToAdd[entityIdx];
            VectorEraseUnsorted( m_entitiesToAdd, entityIdx );
        }
        else // Queue removal
        {
            // Note! We explicitly dont support the case where you add and remove an entity on the same frame. Try to avoid doing stupid things!
            KRG_ASSERT( !VectorContains( m_entitiesToAdd, entityID, [] ( Entity* pEntity, UUID entityID ) { return pEntity->GetID() == entityID; } ) );

            pEntityToRemove = FindEntity( entityID );
            KRG_ASSERT( pEntityToRemove != nullptr ); 
            m_entitiesToRemove.emplace_back( pEntityToRemove );
        }

        return pEntityToRemove;
    }

    //-------------------------------------------------------------------------

    void EntityMap::Load( LoadingContext const& loadingContext )
    {
        KRG_ASSERT( Threading::IsMainThread() && m_pCollection == nullptr && loadingContext.IsValid() );
        KRG_ASSERT( m_status == Status::Unloaded );

        if ( m_isTransientMap )
        {
            m_pCollection = KRG::New<EntityCollection>();
            m_status = Status::Loaded;
        }
        else // Request loading of map resource
        {
            loadingContext.m_pResourceSystem->LoadResource( m_pMapDesc );
            m_status = Status::MapLoading;
        }
    }

    void EntityMap::Unload( LoadingContext const& loadingContext )
    {
        KRG_ASSERT( m_status != Status::Unloaded );
        m_unloadRequested = true;
    }

    void EntityMap::Activate( LoadingContext const& loadingContext )
    {
        KRG_ASSERT( m_status == Status::Loaded );

        for ( auto pEntity : m_pCollection->GetEntities() )
        {
            pEntity->Activate( loadingContext );
        }

        m_status = Status::Activated;
    }

    void EntityMap::Deactivate( LoadingContext const& loadingContext )
    {
        KRG_ASSERT( m_status == Status::Activated );

        //-------------------------------------------------------------------------

        for ( auto pEntity : m_pCollection->GetEntities() )
        {
            if ( pEntity->IsActivated() )
            {
                pEntity->Deactivate( loadingContext );
            }
        }

        m_status = Status::Loaded;
    }

    //-------------------------------------------------------------------------

    bool EntityMap::UpdateState( LoadingContext const& loadingContext )
    {
        KRG_ASSERT( Threading::IsMainThread() && loadingContext.IsValid() );

        //-------------------------------------------------------------------------
        // Process hot-reload requests
        //-------------------------------------------------------------------------

        // We need to have at least a single frame delay between the unload and load requests
        // so that the resource system can actually unload the resource, instead of just canceling the unload request
        if ( !m_entitiesToReload.empty() )
        {
            for ( auto pEntity : m_entitiesToReload )
            {
                KRG_ASSERT( pEntity != nullptr && pEntity->IsDeactivated() );
                pEntity->LoadComponents( loadingContext );
                m_loadingEntities.emplace_back( pEntity );
            }

            m_entitiesToReload.clear();
        }

        // Find all entities that need to be reloaded. immediately unload them and then queue them for reload
        auto const& entitiesThatRequireReload = loadingContext.m_pResourceSystem->GetUsersThatRequireReload();
        if ( !entitiesThatRequireReload.empty() )
        {
            for ( auto const& entityID : entitiesThatRequireReload )
            {
                auto pFoundEntity = FindEntity( entityID );
                if ( pFoundEntity != nullptr )
                {
                    // Immediately unload the entity
                    if ( pFoundEntity->IsActivated() )
                    {
                        pFoundEntity->Deactivate( loadingContext );
                    }

                    pFoundEntity->UnloadComponents( loadingContext );

                    // Add to list of entities to reload next frame
                    m_entitiesToReload.emplace_back( pFoundEntity );
                }
            }
        }
        
        //-------------------------------------------------------------------------
        // Map Unload
        //-------------------------------------------------------------------------

        if ( m_unloadRequested )
        {
            if ( m_status != Status::LoadFailed )
            {
                if ( m_pCollection != nullptr )
                {
                    // Deactivate and unload entities
                    for ( auto pEntity : m_pCollection->GetEntities() )
                    {
                        if ( pEntity->IsActivated() )
                        {
                            pEntity->Deactivate( loadingContext );
                        }

                        pEntity->UnloadComponents( loadingContext );
                    }

                    // Delete collection
                    KRG::Delete( m_pCollection );
                }

                // Since entity ownership is transferred via the add call, we need to delete all pending add entity requests
                for ( auto pEntity : m_entitiesToAdd )
                {
                    KRG_ASSERT( !pEntity->HasRequestedComponentLoad() );
                    KRG::Delete( pEntity );
                }
                m_entitiesToAdd.clear();

                // Clear all internal entity lists
                m_loadingEntities.clear();
                m_entitiesToReload.clear();
                m_entitiesToRemove.clear();
            }

            // Unload the map resource
            if ( !m_isTransientMap )
            {
                loadingContext.m_pResourceSystem->UnloadResource( m_pMapDesc );
            }

            m_status = Status::Unloaded;
        }

        //-------------------------------------------------------------------------
        // Map descriptor Loading
        //-------------------------------------------------------------------------

        else if ( m_status == Status::MapLoading )
        {
            KRG_ASSERT( !m_isTransientMap );

            if ( m_pMapDesc.IsLoading() )
            {
                return false;
            }
            else if ( m_pMapDesc.HasLoadingFailed() )
            {
                m_status = Status::LoadFailed;
            }
            else if ( m_pMapDesc.IsLoaded() )
            {
                if ( m_pMapDesc->IsValid() )
                {
                    // Create entities
                    auto collectionArchive = m_pMapDesc->GetCollection();
                    m_pCollection = KRG::New<EntityCollection>( *loadingContext.m_pTypeRegistry, m_pMapDesc->GetID(), collectionArchive );
                    m_loadingEntities.reserve( m_pCollection->GetEntities().size() );
                    KRG_ASSERT( m_pCollection != nullptr );

                    // Load Entities
                    for ( auto pEntity : m_pCollection->GetEntities() )
                    {
                        pEntity->LoadComponents( loadingContext );
                        m_loadingEntities.emplace_back( pEntity );
                    }

                    m_status = Status::EntitiesLoading;
                }
                else // Invalid map data is treated as a failed load
                {
                    m_status = Status::LoadFailed;
                }
            }
        }

        //-------------------------------------------------------------------------
        // Process entity addition
        //-------------------------------------------------------------------------

        // Wait until we have a collection to add the entities too since the map might still be loading
        if ( m_pCollection != nullptr )
        {
            // Add entities to the collection and request load
            for ( auto pEntityToAdd : m_entitiesToAdd )
            {
                m_pCollection->AddEntity( pEntityToAdd );
                pEntityToAdd->LoadComponents( loadingContext );
                m_loadingEntities.emplace_back( pEntityToAdd );
            }

            m_entitiesToAdd.clear();
        }

        //-------------------------------------------------------------------------
        // Process entity removal
        //-------------------------------------------------------------------------

        // Unload and deactivate entities and remove them from the collection
        for ( auto pEntityToRemove : m_entitiesToRemove )
        {
            // Deactivate if activated
            if ( pEntityToRemove->IsActivated() )
            {
                pEntityToRemove->Deactivate( loadingContext );
            }
            else // Remove from loading list as we might still be loaded this entity
            {
                m_loadingEntities.erase_first_unsorted( pEntityToRemove );
                m_entitiesToReload.erase_first_unsorted( pEntityToRemove );
            }

            // Unload components and remove from collection
            pEntityToRemove->UnloadComponents( loadingContext );
            m_pCollection->RemoveEntity( pEntityToRemove->m_ID );
        }

        m_entitiesToRemove.clear();

        //-------------------------------------------------------------------------
        // Entity loading
        //-------------------------------------------------------------------------

        for ( S32 i = (S32) m_loadingEntities.size() - 1; i >= 0; i-- )
        {
            auto pEntity = m_loadingEntities[i];
            if ( pEntity->UpdateLoadingAndEntityState( loadingContext ) )
            {
                // Remove loaded entity from loading list
                VectorEraseUnsorted( m_loadingEntities, i );

                //-------------------------------------------------------------------------

                // If the map is activated, immediately activate any entities that finish loading
                if ( IsActivated() && pEntity->IsDeactivated() )
                {
                    pEntity->Activate( loadingContext );
                }
            }
            else // Entity is still loading
            {
                return false;
            }
        }

        // Ensure that we set the status to loaded, if we were in the entity loading stage and all loads completed
        if ( m_status == Status::EntitiesLoading && m_loadingEntities.empty() )
        {
            KRG_ASSERT( !m_isTransientMap );
            m_status = Status::Loaded;
        }

        // All Loading operations complete
        //-------------------------------------------------------------------------

        return true;
    }

    //-------------------------------------------------------------------------

    Entity* EntityMap::FindEntity( UUID entityID ) const
    {
        Entity* pFoundEntity = nullptr;

        if ( m_pCollection != nullptr )
        {
            pFoundEntity = m_pCollection->FindEntity( entityID );
        }

        return pFoundEntity;
    }

    void EntityMap::OnEntityStateUpdated( Entity* pEntity )
    {
        if ( pEntity->GetCollectionID() != m_pCollection->GetID() )
        {
            return;
        }

        KRG_ASSERT( FindEntity( pEntity->GetID() ) );
        m_loadingEntities.emplace_back( pEntity );
    }
}