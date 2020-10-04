#include "EntityMap.h"
#include "System/Resource/ResourceSystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    bool EntityMap::IsValid() const
    {
        return !m_entityArchive.GetEntityDescriptors().empty();
    }

    //-------------------------------------------------------------------------

    EntityMapInstance::EntityMapInstance( ResourceID mapResourceID )
        : m_pMap( mapResourceID )
    {
        KRG_ASSERT( mapResourceID.IsValid() );
    }

    EntityMapInstance::~EntityMapInstance()
    {
        KRG_ASSERT( m_pCollection == nullptr && IsUnloaded() );
    }

    //-------------------------------------------------------------------------

    void EntityMapInstance::Load( EntityLoadingContext const& loadingContext )
    {
        KRG_ASSERT( Threading::IsMainThread() && IsUnloaded() && m_pCollection == nullptr && loadingContext.IsValid() );
        loadingContext.m_pResourceSystem->LoadResource( m_pMap );
    }

    void EntityMapInstance::Unload( EntityLoadingContext const& loadingContext )
    {
        KRG_ASSERT( Threading::IsMainThread() && ( IsLoaded() || IsLoading() || HasLoadingFailed() ) );

        if ( m_pCollection != nullptr )
        {
            KRG::Delete( m_pCollection );
        }

        loadingContext.m_pResourceSystem->UnloadResource( m_pMap );
    }

    bool EntityMapInstance::IsLoading() const
    {
        if ( m_pMap.IsLoading() )
        {
            return true;
        }

        if ( m_pCollection != nullptr )
        {
            return false;
        }

        // If the collection is not instantiated wait for it
        return !m_pMap.HasLoadingFailed();
    }

    void EntityMapInstance::UpdateLoading( EntityLoadingContext const& loadingContext )
    {
        KRG_ASSERT( Threading::IsMainThread() && loadingContext.IsValid() );

        if ( m_pCollection == nullptr )
        {
            if ( m_pMap.IsLoaded() )
            {
                KRG_ASSERT( m_pMap.IsLoaded() && m_pMap->IsValid() );

                // Create instance
                auto collectionArchive = m_pMap->GetEntityArchive();
                m_pCollection = KRG::New<EntityCollection>( *loadingContext.m_pTypeRegistry, m_pMap->GetID(), collectionArchive );
                KRG_ASSERT( m_pCollection != nullptr );
            }
        }
    }
}