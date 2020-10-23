#include "EntityMap.h"
#include "System/Resource/ResourceSystem.h"
#include "System/Entity/Entity.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    EntityMap::EntityMap( ResourceID mapResourceID )
        : m_pMapDesc( mapResourceID )
    {}

    EntityMap::EntityMap( EntityMap const& map )
        : m_pMapDesc( map.m_pMapDesc )
    {
        KRG_ASSERT( map.m_status == Status::Unloaded );
    }

    EntityMap::EntityMap( EntityMap&& map )
        : m_pMapDesc( eastl::move( map.m_pMapDesc ) )
        , m_status( map.m_status )
        , m_pCollection( map.m_pCollection )
    {
        map.m_pCollection = nullptr;
        map.m_status = Status::Unloaded;
    }

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

    //-------------------------------------------------------------------------

    void EntityMap::Load( LoadingContext const& loadingContext )
    {
        KRG_ASSERT( Threading::IsMainThread() && IsUnloaded() && m_pCollection == nullptr && loadingContext.IsValid() );
        loadingContext.m_pResourceSystem->LoadResource( m_pMapDesc );
        m_status = Status::Loading;
    }

    void EntityMap::Unload( LoadingContext const& loadingContext )
    {
        KRG_ASSERT( m_status != Status::Unloaded );
        m_status = Status::UnloadRequested;
    }

    bool EntityMap::IsLoading() const
    {
        if ( m_pMapDesc.IsLoading() )
        {
            return true;
        }

        if ( m_pCollection != nullptr )
        {
            return false;
        }

        // If the collection is not instantiated wait for it
        return !m_pMapDesc.HasLoadingFailed();
    }

    bool EntityMap::UpdateLoading( LoadingContext const& loadingContext )
    {
        KRG_ASSERT( Threading::IsMainThread() && loadingContext.IsValid() );

        //-------------------------------------------------------------------------

        if ( m_status == Status::Loading )
        {
            if ( m_pMapDesc.IsLoading() )
            {
                return false;
            }
            else if ( m_pMapDesc.HasLoadingFailed() )
            {
                m_status = Status::Loaded;
            }
            else if ( m_pMapDesc.IsLoaded() )
            {
                KRG_ASSERT( m_pMapDesc->IsValid() );
                m_status = Status::Loaded;

                // Create entities
                auto collectionArchive = m_pMapDesc->GetCollectionTemplate();
                m_pCollection = KRG::New<EntityCollection>( *loadingContext.m_pTypeRegistry, m_pMapDesc->GetID(), collectionArchive );
                KRG_ASSERT( m_pCollection != nullptr );

                LoadEntities( loadingContext );
            }
        }
        else if ( m_status == Status::UnloadRequested )
        {
            UnloadEntities( loadingContext );

            if ( m_pCollection != nullptr )
            {
                KRG::Delete( m_pCollection );
            }

            loadingContext.m_pResourceSystem->UnloadResource( m_pMapDesc );
        }

        //-------------------------------------------------------------------------

        // Loading operations complete
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
        KRG_ASSERT( m_pCollection != nullptr );

        for ( auto pEntity : m_pCollection->GetEntities() )
        {
            pEntity->Load( loadingContext );
        }
    }

    void EntityMap::UnloadEntities( LoadingContext const& loadingContext )
    {
        KRG_ASSERT( m_pCollection != nullptr );

        for ( auto pEntity : m_pCollection->GetEntities() )
        {
            pEntity->Unload( loadingContext );
        }
    }
}