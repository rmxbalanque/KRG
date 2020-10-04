#include "EntityWorld.h"
#include "Map/EntityMap.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/Resource/ResourceSystem.h"
#include "System/Core/Profiling/Profiling.h"
#include "System/Core/Settings/DebugSettings.h"
#include "EASTL/bonus/adaptors.h"
#include "System/Core/Time/Timers.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Settings
    {
        #if KRG_DEBUG_INSTRUMENTATION
        static DebugSettingBool g_showEntityWorldLoadErrors( "ShowEntityWorldLoadErrors", "Entity/World", "", false );
        #endif
    }

    //-------------------------------------------------------------------------

    EntityWorld::~EntityWorld()
    {
        KRG_ASSERT( m_activeLoaders.empty() );
        KRG_ASSERT( m_loadedMaps.empty() && m_loadingMaps.empty() );
        KRG_ASSERT( m_globalSystems.empty() );

        for ( S8 i = 0; i < (S8) UpdateStage::NumStages; i++ )
        {
            KRG_ASSERT( m_entityUpdateLists[i].empty() );
            KRG_ASSERT( m_systemUpdateLists[i].empty() );
        }
    }

    void EntityWorld::Initialize( SystemRegistry const& systemsRegistry )
    {
        m_loadingContext = EntityLoadingContext( systemsRegistry.GetSystem<TypeSystem::TypeRegistry>(), systemsRegistry.GetSystem<Resource::ResourceSystem>() );
        KRG_ASSERT( m_loadingContext.IsValid() );

        m_pTaskSystem = systemsRegistry.GetSystem<TaskSystem>();
        KRG_ASSERT( m_pTaskSystem != nullptr );

        // Initialize all global systems
        for ( auto pSystem : m_globalSystems )
        {
            pSystem->InitializeSystem( systemsRegistry );
        }

        m_initialized = true;
    }

    void EntityWorld::Shutdown()
    {
        // Cancel any load/unload requests
        m_mapLoadRequests.clear();
        m_mapUnloadRequests.clear();

        // Unload any currently loading maps
        //-------------------------------------------------------------------------

        for ( auto& pLoadingMap : m_loadingMaps )
        {
            UnloadMap( pLoadingMap->GetResourceID() );
        }

        // Unload any fully loaded maps
        //-------------------------------------------------------------------------

        // Cancel any active load requests
        for ( auto pLoader : m_activeLoaders )
        {
            pLoader->CancelOperation( m_loadingContext );
            KRG::Delete( pLoader );
        }
        m_activeLoaders.clear();

        for ( auto& pLoadedMap : m_loadedMaps )
        {
            UnloadMap( pLoadedMap->GetResourceID() );
        }

        // Run the loading update as this will immediately unload all maps
        //-------------------------------------------------------------------------

        UpdateLoading();

        // Shutdown all global systems
        //-------------------------------------------------------------------------

        for( auto pSystem : m_globalSystems )
        {
            pSystem->ShutdownSystem();
        }

        //-------------------------------------------------------------------------

        m_pTaskSystem = nullptr;
        m_initialized = false;
    }

    //-------------------------------------------------------------------------
    // Frame Update
    //-------------------------------------------------------------------------

    void EntityWorld::Update( UpdateContext const& context )
    {
        struct EntityUpdateTask : public IAsyncTask
        {
            EntityUpdateTask( UpdateContext const& context, TVector<Entity*>& updateList )
                : m_context( context )
                , m_updateList( updateList )
            { 
                m_SetSize = (U32) updateList.size();
            }

            virtual void ExecuteRange( TaskSetPartition range, U32 threadnum ) override final
            {
                for ( uint64_t i = range.start; i < range.end; ++i )
                {
                    KRG_PROFILE_GROUPED_SCOPE_COLOR( "Entity", "Update Entity", MP_LIMEGREEN );
                    m_updateList[i]->UpdateSystems( m_context );
                }
            }

        private:

            UpdateContext const&    m_context;
            TVector<Entity*>&       m_updateList;
        };

        //-------------------------------------------------------------------------

        S8 const updateStageIdx = (S8) context.GetUpdateStage();

        // Update entities
        //-------------------------------------------------------------------------

        EntityUpdateTask updateTask( context, m_entityUpdateLists[updateStageIdx] );
        m_pTaskSystem->ScheduleTask( &updateTask );
        m_pTaskSystem->WaitForTask( &updateTask );

        // Update systems
        //-------------------------------------------------------------------------

        for ( auto pSystem : m_systemUpdateLists[updateStageIdx] )
        {
            KRG_PROFILE_GROUPED_SCOPE_COLOR( "Entity", "Update Systems", MP_LIMEGREEN );
            KRG_ASSERT( pSystem->GetRequiredUpdatePriorities().IsUpdateStageEnabled( (UpdateStage) updateStageIdx ) );
            pSystem->UpdateSystem( context );
        }
    }

    //-------------------------------------------------------------------------
    // Systems
    //-------------------------------------------------------------------------

    void EntityWorld::RegisterGlobalSystem( IGlobalEntitySystem* pSystem )
    {
        KRG_ASSERT( pSystem != nullptr && m_initialized == false );
        m_globalSystems.push_back( pSystem );

        // Add to update lists
        //-------------------------------------------------------------------------

        for ( S8 i = 0; i < (S8) UpdateStage::NumStages; i++ )
        {
            if ( pSystem->GetRequiredUpdatePriorities().IsUpdateStageEnabled( (UpdateStage) i ) )
            {
                m_systemUpdateLists[i].push_back( pSystem );
            }

            // Sort update list
            auto comparator = [i] ( IGlobalEntitySystem* const& pSystemA, IGlobalEntitySystem* const& pSystemB )
            {
                U8 const A = pSystemA->GetRequiredUpdatePriorities().GetPriorityForStage( (UpdateStage) i );
                U8 const B = pSystemB->GetRequiredUpdatePriorities().GetPriorityForStage( (UpdateStage) i );
                return A > B;
            };

            eastl::sort( m_systemUpdateLists[i].begin(), m_systemUpdateLists[i].end(), comparator );
        }
    }

    void EntityWorld::UnregisterGlobalSystem( IGlobalEntitySystem* pSystem )
    {
        KRG_ASSERT( pSystem != nullptr && m_initialized == false );
        auto iter = VectorFind( m_globalSystems, pSystem );
        KRG_ASSERT( iter != m_globalSystems.end() );

        m_globalSystems.erase_unsorted( iter );

        // Remove from update lists
        //-------------------------------------------------------------------------

        for ( S8 i = 0; i < (S8) UpdateStage::NumStages; i++ )
        {
            if ( pSystem->GetRequiredUpdatePriorities().IsUpdateStageEnabled( (UpdateStage) i ) )
            {
                auto updateIter = VectorFind( m_systemUpdateLists[i], pSystem );
                KRG_ASSERT( updateIter != m_systemUpdateLists[i].end() );
                m_systemUpdateLists[i].erase( updateIter );
            }
        }
    }

    //-------------------------------------------------------------------------
    // Loading
    //-------------------------------------------------------------------------

    bool EntityWorld::IsMapLoaded( ResourceID const& mapResourceID ) const
    {
        // Make sure the map isn't already loaded or loading, since duplicate loads are not allowed
        for ( auto& pLoadedMap : m_loadedMaps )
        {
            if ( pLoadedMap->GetResourceID() == mapResourceID )
            {
                return true;
            }
        }

        return false;
    }

    bool EntityWorld::IsMapLoadedOrLoading( ResourceID const& mapResourceID ) const
    {
        if ( VectorContains( m_mapLoadRequests, mapResourceID ) )
        {
            return true;
        }

        for ( auto& pMapInstance : m_loadingMaps )
        {
            if( pMapInstance->GetResourceID() == mapResourceID )
            {
                return true;
            }
        }

        for ( auto& pMapInstance : m_loadedMaps )
        {
            if ( pMapInstance->GetResourceID() == mapResourceID )
            {
                return true;
            }
        }

        return false;
    }

    void EntityWorld::LoadMap( ResourceID const& mapResourceID )
    {
        KRG_ASSERT( mapResourceID.IsValid() );
        KRG_ASSERT( !IsMapLoadedOrLoading( mapResourceID ) );

        // If we have an unload request for this map, just cancel it
        if ( VectorContains( m_mapUnloadRequests, mapResourceID ) )
        {
            m_mapUnloadRequests.erase_last_unsorted( mapResourceID );
        }
        else // Queue load request
        {
            KRG_ASSERT( !VectorContains( m_mapLoadRequests, mapResourceID ) );
            m_mapLoadRequests.emplace_back( mapResourceID );
        }
    }

    void EntityWorld::UnloadMap( ResourceID const& mapResourceID )
    {
        KRG_ASSERT( mapResourceID.IsValid() );

        // If we have a load request for this map, just cancel it
        if ( VectorContains( m_mapLoadRequests, mapResourceID ) )
        {
            m_mapLoadRequests.erase_last_unsorted( mapResourceID );
        }
        else // Queue unload request
        {
            KRG_ASSERT( !VectorContains( m_mapUnloadRequests, mapResourceID ) );
            m_mapUnloadRequests.emplace_back( mapResourceID );
        }
    }

    //-------------------------------------------------------------------------

    void EntityWorld::UpdateLoading()
    {
        KRG_PROFILE_GROUPED_SCOPE_COLOR( "Entity", "World Loading", MP_LIMEGREEN );

        //-------------------------------------------------------------------------
        // Process map load requests
        //-------------------------------------------------------------------------

        for ( auto& mapResourceID : m_mapLoadRequests )
        {
            m_loadingMaps.emplace_back( KRG::New<EntityMapInstance>( mapResourceID ) );
            m_loadingMaps.back()->Load( m_loadingContext );
        }

        m_mapLoadRequests.clear();

        //-------------------------------------------------------------------------
        // Process map unload requests
        //-------------------------------------------------------------------------

        for ( auto& mapResourceID : m_mapUnloadRequests )
        {
            bool requestHandled = false;

            // If the map is still in the loading list, then directly unload it
            S32 const numLoadingMaps = (S32) m_loadingMaps.size();
            for ( auto i = 0; i < numLoadingMaps; i++ )
            {
                if ( m_loadingMaps[i]->GetResourceID() == mapResourceID )
                {
                    // Unload and erase map instance
                    m_loadingMaps[i]->Unload( m_loadingContext );
                    KRG::Delete( m_loadingMaps[i] );
                    VectorEraseUnsorted( m_loadingMaps, i );

                    requestHandled = true;
                    break;
                }
            }

            if ( requestHandled )
            {
                continue;
            }

            //-------------------------------------------------------------------------

            // If the map is still in the loading list, then shutdown all entities and then unload the map
            S32 const numLoadedMaps = (S32) m_loadedMaps.size();
            for ( S32 i = 0; i < numLoadedMaps; i++ )
            {
                if ( m_loadedMaps[i]->GetResourceID() == mapResourceID )
                {
                    Timer t;

                    // Shutdown and unload entities
                    for ( auto pEntity : m_loadedMaps[i]->GetEntities() )
                    {
                        if( pEntity->IsLoading() || pEntity->IsUnloaded() )
                        {
                            continue;
                        }

                        ShutdownEntity( pEntity );

                        //-------------------------------------------------------------------------

                        // Remove from the global lookup map
                        auto entityIter = m_entityLookupMap.find( pEntity->GetID() );
                        KRG_ASSERT( entityIter != m_entityLookupMap.end() );
                        m_entityLookupMap.erase( entityIter );
                    }

                    {
                        auto unloader = EntityLoader::Unload( m_loadingContext, m_loadedMaps[i]->GetEntities() );
                    }

                    //-------------------------------------------------------------------------

                    // Unload and erase map instance
                    m_loadedMaps[i]->Unload( m_loadingContext );
                    KRG::Delete( m_loadingMaps[i] );
                    VectorEraseUnsorted( m_loadedMaps, i );

                    requestHandled = true;
                    break;
                }
            }

            KRG_ASSERT( requestHandled ); // Unload request for unknown map
        }

        m_mapUnloadRequests.clear();

        //-------------------------------------------------------------------------
        // Update map loads
        //-------------------------------------------------------------------------

        for ( auto i = 0; i < m_loadingMaps.size(); i++ )
        {
            auto pMapInstance = m_loadingMaps[i];
            pMapInstance->UpdateLoading( m_loadingContext );

            // If we are finished loading
            if( !pMapInstance->IsLoading() )
            {
                // Remove from the loading maps list
                VectorEraseUnsorted( m_loadingMaps, i );
                i--;

                // Handle load result
                if( pMapInstance->IsLoaded() )
                {
                    for ( auto pEntity : pMapInstance->GetEntities() )
                    {
                        // Add to global lookup map
                        KRG_ASSERT( m_entityLookupMap.find( pEntity->GetID() ) == m_entityLookupMap.end() );
                        m_entityLookupMap.insert( TPair<UUID, Entity*>( pEntity->GetID(), pEntity ) );
                    }

                    // Request load of all entities
                    m_loadedMaps.emplace_back( pMapInstance );
                    m_activeLoaders.emplace_back( KRG::New<EntityLoader>( EntityLoader::Load( m_loadingContext, pMapInstance->GetEntities() ) ) );
                }
                else // We failed loading, unload the resource ID and release the instance memory
                {
                    KRG_ASSERT( pMapInstance->HasLoadingFailed() );
                    KRG_LOG_ERROR( "Entity", "Failed to load map: %s", pMapInstance->GetResourceID().c_str() );
                    pMapInstance->Unload( m_loadingContext );
                    KRG::Delete( pMapInstance );
                }
            }
        }

        //-------------------------------------------------------------------------
        // Update Entity Loads
        //-------------------------------------------------------------------------

        for ( S32 i = (S32) m_activeLoaders.size() - 1; i >= 0; i-- )
        {
            auto pEntityLoader = m_activeLoaders[i];
            pEntityLoader->UpdateLoading( m_loadingContext );

            if ( pEntityLoader->IsLoaded() )
            {
                for ( auto pEntity : pEntityLoader->GetEntities() )
                {
                    InitializeEntity( pEntity );
                }

                // For reload operations we have an explicit list of entities to reactivate
                //-------------------------------------------------------------------------

                if ( pEntityLoader->IsReloadOperation() )
                {
                    for ( auto pEntityToActivate : pEntityLoader->GetEntitiesToReactivate() )
                    {
                        if ( pEntityToActivate->IsLoaded() )
                        {
                            continue;
                        }
                    }
                }

                // Remove the loader as we are done with it
                //-------------------------------------------------------------------------

                KRG::Delete( pEntityLoader );
                VectorEraseUnsorted( m_activeLoaders, i );
            }
        }

        //-------------------------------------------------------------------------
        // Process hot-reload requests
        //-------------------------------------------------------------------------
        // This has to happen last since the EntityWorld loading occurs immediately before the resource system allowing the resource system
        // to immediately process any unload requests.

        auto const& entitiesThatRequireReload = m_loadingContext.m_pResourceSystem->GetUsersThatRequireReload();
        if ( !entitiesThatRequireReload.empty() )
        {
            TVector<Entity*> entitiesToReload;
            for ( auto userID : entitiesThatRequireReload )
            {
                auto iter = m_entityLookupMap.find( userID );
                if ( iter != m_entityLookupMap.end() )
                {
                    entitiesToReload.emplace_back( iter->second );
                }
            }

            if ( !entitiesToReload.empty() )
            {
                ReloadEntities( entitiesToReload );
            }
        }
    }

    //-------------------------------------------------------------------------
    // Entity operations
    //-------------------------------------------------------------------------

    void EntityWorld::InitializeEntity( Entity* pEntity )
    {
        KRG_ASSERT( pEntity->IsLoaded() );

        //-------------------------------------------------------------------------

        pEntity->Initialize( *m_pSystemsRegistry );
       
        // Add to global systems
        for ( auto pGlobalSystem : m_globalSystems )
        {
            for ( auto pComponent : pEntity->m_components )
            {
                if ( pComponent->IsInitialized() )
                {
                    pGlobalSystem->RegisterComponent( pEntity, pComponent );
                }
            }
        }

        // Add to update lists
        for ( S8 i = 0; i < (S8) UpdateStage::NumStages; i++ )
        {
            if ( pEntity->RequiresUpdate( (UpdateStage) i ) )
            {
                m_entityUpdateLists[i].push_back( pEntity );
            }
        }
    }

    void EntityWorld::ShutdownEntity( Entity* pEntity )
    {
        KRG_ASSERT( pEntity->IsInitialized() );

        //-------------------------------------------------------------------------

        // Remove from update lists
        for ( S8 i = 0; i < (S8) UpdateStage::NumStages; i++ )
        {
            if ( pEntity->RequiresUpdate( (UpdateStage) i ) )
            {
                m_entityUpdateLists[i].erase_first( pEntity );
            }
        }

        // Remove from global systems
        for ( auto pGlobalSystem : m_globalSystems )
        {
            for ( auto pComponent : pEntity->m_components )
            {
                if ( pComponent->IsInitialized() )
                {
                    pGlobalSystem->UnregisterComponent( pEntity, pComponent );
                }
            }
        }

        pEntity->Shutdown();
    }

    //-------------------------------------------------------------------------

    void EntityWorld::ReloadEntities( TVector<Entity*> const& entities )
    {
        TInlineVector<Entity*, 5> attachmentHierarchy;

        for ( auto pEntity : entities )
        {
            KRG_ASSERT( pEntity != nullptr );

            //-------------------------------------------------------------------------

            if( pEntity->IsInitialized() )
            {
                ShutdownEntity( pEntity );
            }
        }

        // Create the entity reloader and set the list of entities that need to be activated
        //-------------------------------------------------------------------------

        auto pEntityReloader = m_activeLoaders.emplace_back( KRG::New<EntityLoader>( EntityLoader::Reload( m_loadingContext, entities ) ) );
        pEntityReloader->SetEntitiesToReactivate( attachmentHierarchy );
    }
}