#include "EntityLoader.h"
#include "System/Entity/Entity.h"
#include "System/Core/Threading/Threading.h"

//-------------------------------------------------------------------------

namespace KRG
{
    EntityLoader::EntityLoader( EntityLoadingContext const& loadingContext, TVector<Entity*> const& entities, EntityLoader::Operation operation )
        : m_entities( entities )
        , m_operation( operation )
    {
        Initialize( loadingContext );
    }

    EntityLoader::EntityLoader( EntityLoadingContext const& loadingContext, Entity* pEntity, EntityLoader::Operation operation )
        : m_operation( operation )
    {
        m_entities.emplace_back( pEntity );
        Initialize( loadingContext );
    }

    EntityLoader::EntityLoader( EntityLoader&& loader )
        : m_entities( loader.m_entities )
        , m_activeEntityLoadRequests( loader.m_activeEntityLoadRequests )
        , m_loadingStatus( loader.m_loadingStatus )
        , m_operation( loader.m_operation )
    {
        loader.m_entities.clear();
        loader.m_activeEntityLoadRequests.clear();
        loader.m_loadingStatus = LoadingStatus::Unloaded;
        loader.m_operation = Operation::Invalid;
    }

    EntityLoader::~EntityLoader()
    {
        KRG_ASSERT( !IsLoading() && m_activeEntityLoadRequests.empty() );
    }

    //-------------------------------------------------------------------------

    void EntityLoader::Initialize( EntityLoadingContext const& loadingContext )
    {
        switch ( m_operation )
        {
            case Operation::Load :
            {
                #if KRG_DEBUG_INSTRUMENTATION
                for ( auto pEntity : m_entities )
                {
                    KRG_ASSERT( pEntity->IsUnloaded() );
                }
                #endif

                Load( loadingContext );
            }
            break;

            case Operation::Unload:
            {
                Unload( loadingContext );
            }
            break;

            case Operation::Reload:
            {
                Unload( loadingContext );
                Load( loadingContext );
            }
            break;

            default:
            {
                KRG_HALT();
            }
            break;
        }
    }

    //-------------------------------------------------------------------------

    void EntityLoader::Load( EntityLoadingContext const& loadingContext )
    {
        KRG_ASSERT( Threading::IsMainThread() && loadingContext.IsValid() && ( IsUnloaded() || IsUnloading() ) );

        m_activeEntityLoadRequests.clear();

        //-------------------------------------------------------------------------

        for ( auto pEntity : m_entities )
        {
            if ( pEntity->IsUnloaded() )
            {
                pEntity->Load( loadingContext );
            }

            m_activeEntityLoadRequests.push_back( pEntity );
        }

        m_loadingStatus = LoadingStatus::Loading;
    }

    void EntityLoader::Unload( EntityLoadingContext const& loadingContext )
    {
        KRG_ASSERT( Threading::IsMainThread() && loadingContext.IsValid() );

        m_activeEntityLoadRequests.clear();

        //-------------------------------------------------------------------------

        for ( auto pEntity : m_entities )
        {
            KRG_ASSERT( !pEntity->IsInitialized()  );
            if ( pEntity->IsLoaded() || pEntity->IsLoading() )
            {
                pEntity->Unload( loadingContext );
            }
            KRG_ASSERT( pEntity->IsUnloaded() );
        }

        m_loadingStatus = LoadingStatus::Unloading;
    }

    //-------------------------------------------------------------------------

    LoadingStatus EntityLoader::UpdateLoading( EntityLoadingContext const& loadingContext )
    {
        bool const isPendingReload = m_operation == Operation::Reload && m_loadingStatus == LoadingStatus::Unloaded;
        KRG_ASSERT( Threading::IsMainThread() && ( IsLoading() || IsUnloading() || isPendingReload ) );

        // Unload
        //-------------------------------------------------------------------------

        if ( m_operation == Operation::Unload && m_loadingStatus == LoadingStatus::Unloading )
        {
            // We need to wait at least one frame to give the resource system a chance to process our unload request
            m_loadingStatus = LoadingStatus::Unloaded;
            return LoadingStatus::Unloading;
        }

        // Loading Updates
        //-------------------------------------------------------------------------

        if ( m_loadingStatus == LoadingStatus::Loading )
        {
            for ( auto j = (S32) m_activeEntityLoadRequests.size() - 1; j >= 0; j-- )
            {
                auto pLoadingEntity = m_activeEntityLoadRequests[j];
                pLoadingEntity->UpdateLoading();

                // Remove any objects that have finished loading
                if ( pLoadingEntity->IsLoaded() )
                {
                    VectorEraseUnsorted( m_activeEntityLoadRequests, j );
                }
            }

            // Check if loading is complete
            if ( m_activeEntityLoadRequests.empty() )
            {
                // We still want to initialize and activate all entities that successfully loaded so we can never fail loading a collection
                KRG_ASSERT( m_loadingStatus == LoadingStatus::Loading );
                m_loadingStatus = LoadingStatus::Loaded;
            }
        }

        return m_loadingStatus;
    }
}