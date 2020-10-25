#include "EntityMap.h"
#include "System/Resource/ResourceSystem.h"
#include "System/Entity/Entity.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    EntityMap::EntityMap()
        : m_isTransientMap( true )
    {}

    EntityMap::~EntityMap()
    {
        KRG_ASSERT( m_pCollection == nullptr && IsUnloaded() );
    }

    //-------------------------------------------------------------------------

    EntityMap& EntityMap::operator=( EntityMap const& map )
    {
        m_pMapDesc = map.m_pMapDesc;
        KRG_ASSERT( map.m_status == Status::Unloaded );
        return *this;
    }

    EntityMap& EntityMap::operator=( EntityMap&& map )
    {
        m_pMapDesc = eastl::move( map.m_pMapDesc );
        m_status = map.m_status;
        m_pCollection = map.m_pCollection;

        map.m_pCollection = nullptr;
        map.m_status = Status::Unloaded;
        return *this;
    }

    //-------------------------------------------------------------------------

    void EntityMap::Load( LoadingContext const& loadingContext )
    {
        KRG_ASSERT( Threading::IsMainThread() && m_pCollection == nullptr && loadingContext.IsValid() );
        KRG_ASSERT( !m_isTransientMap );
        KRG_ASSERT( m_status == Status::Unloaded );

        loadingContext.m_pResourceSystem->LoadResource( m_pMapDesc );
        m_status = Status::MapDescLoading;
    }

    void EntityMap::Unload( LoadingContext const& loadingContext )
    {
        KRG_ASSERT( !m_isTransientMap );
        KRG_ASSERT( m_status != Status::Unloaded );
        m_unloadRequested = true;
    }

    bool EntityMap::UpdateLoading( LoadingContext const& loadingContext )
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
            if ( m_status == Status::Activated )
            {
                Deactivate( loadingContext );
            }

            UnloadEntities( loadingContext );

            if ( m_pCollection != nullptr )
            {
                KRG::Delete( m_pCollection );
            }

            loadingContext.m_pResourceSystem->UnloadResource( m_pMapDesc );

            m_status = Status::Unloaded;
        }

        //-------------------------------------------------------------------------
        // Map descriptor Loading
        //-------------------------------------------------------------------------

        else if ( m_status == Status::MapDescLoading )
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
                KRG_ASSERT( m_pMapDesc->IsValid() );
                m_status = Status::MapDescLoaded;

                // Create entities
                auto collectionArchive = m_pMapDesc->GetCollectionTemplate();
                m_pCollection = KRG::New<EntityCollection>( *loadingContext.m_pTypeRegistry, m_pMapDesc->GetID(), collectionArchive );
                m_loadingEntities.reserve( m_pCollection->GetEntities().size() );
                KRG_ASSERT( m_pCollection != nullptr );

                LoadEntities( loadingContext );
            }
        }

        //-------------------------------------------------------------------------
        // Entity loading
        //-------------------------------------------------------------------------

        for ( S32 i = (S32) m_loadingEntities.size() - 1; i >= 0; i-- )
        {
            auto pEntity = m_loadingEntities[i];
            if ( pEntity->UpdateComponentLoading( loadingContext ) )
            {
                // Remove loaded entity from loading list
                VectorEraseUnsorted( m_loadingEntities, i );

                //-------------------------------------------------------------------------

                // If the map is activated, immediately activate any entities that finish loading
                if ( IsActivated() )
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

    Entity* EntityMap::FindEntity( UUID entityID ) const
    {
        Entity* pFoundEntity = nullptr;

        if ( m_pCollection != nullptr )
        {
            pFoundEntity = m_pCollection->FindEntity( entityID );
        }

        return pFoundEntity;
    }

    void EntityMap::LoadEntities( LoadingContext const& loadingContext )
    {
        KRG_ASSERT( m_pCollection != nullptr && m_status == Status::MapDescLoaded );

        for ( auto pEntity : m_pCollection->GetEntities() )
        {
            pEntity->LoadComponents( loadingContext );
            m_loadingEntities.emplace_back( pEntity );
        }

        m_status = Status::EntitiesLoading;
    }

    void EntityMap::UnloadEntities( LoadingContext const& loadingContext )
    {
        KRG_ASSERT( m_pCollection != nullptr );

        for ( auto pEntity : m_pCollection->GetEntities() )
        {
            pEntity->UnloadComponents( loadingContext );
        }

        m_status = Status::MapDescLoaded;
    }

    //-------------------------------------------------------------------------

    void EntityMap::Activate( LoadingContext const& loadingContext )
    {
        if ( m_isTransientMap )
        {
            KRG_ASSERT( m_status == Status::Unloaded );
            m_pCollection = KRG::New<EntityCollection>();
            m_status = Status::Activated;
        }
        else // Regular map flow
        {
            KRG_ASSERT( m_status == Status::Loaded );

            for ( auto pEntity : m_pCollection->GetEntities() )
            {
                pEntity->Activate( loadingContext );
            }
            
            m_status = Status::Activated;
        }
    }

    void EntityMap::Deactivate( LoadingContext const& loadingContext )
    {
        if ( m_isTransientMap )
        {
            KRG::Delete<EntityCollection>( m_pCollection );
            m_status = Status::Unloaded;
        }
        else // Regular map flow
        {
            KRG_ASSERT( m_status == Status::Activated );

            for ( auto pEntity : m_pCollection->GetEntities() )
            {
                if ( pEntity->IsActivated() )
                {
                    pEntity->Deactivate( loadingContext );
                }
            }

            m_status = Status::Loaded;
        }
    }
}