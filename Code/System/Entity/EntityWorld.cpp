#include "EntityWorld.h"
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
        KRG_ASSERT( m_maps.empty());
        KRG_ASSERT( m_globalSystems.empty() );

        for ( S8 i = 0; i < (S8) UpdateStage::NumStages; i++ )
        {
            KRG_ASSERT( m_entityUpdateLists[i].empty() );
            KRG_ASSERT( m_systemUpdateLists[i].empty() );
        }
    }

    void EntityWorld::Initialize( SystemRegistry const& systemsRegistry )
    {
        m_loadingContext = EntityModel::LoadingContext( systemsRegistry.GetSystem<TypeSystem::TypeRegistry>(), systemsRegistry.GetSystem<Resource::ResourceSystem>() );
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
        // Unload maps
        //-------------------------------------------------------------------------

        for ( auto& map : m_maps )
        {
            UnloadMap( map.GetMapResourceID() );
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

    bool EntityWorld::IsBusyLoading() const
    {
        for ( auto const& map : m_maps )
        {
            if( map.IsLoading() ) 
            {
                return true;
            }
        }

        return false;
    }

    bool EntityWorld::IsMapLoaded( ResourceID const& mapResourceID ) const
    {
        // Make sure the map isn't already loaded or loading, since duplicate loads are not allowed
        for ( auto const& map : m_maps )
        {
            if ( map.GetMapResourceID() == mapResourceID )
            {
                return true;
            }
        }

        return false;
    }

    void EntityWorld::LoadMap( ResourceID const& mapResourceID )
    {
        KRG_ASSERT( mapResourceID.IsValid() && mapResourceID.GetResourceTypeID() == EntityModel::EntityMapDescriptor::GetStaticResourceTypeID() );

        KRG_ASSERT( !IsMapLoaded( mapResourceID ) );
        auto& map = m_maps.emplace_back( EntityModel::EntityMap( mapResourceID ) );
        map.Load( m_loadingContext );
    }

    void EntityWorld::UnloadMap( ResourceID const& mapResourceID )
    {
        KRG_ASSERT( mapResourceID.IsValid() && mapResourceID.GetResourceTypeID() == EntityModel::EntityMapDescriptor::GetStaticResourceTypeID() );

        auto const foundMapIter = VectorFind( m_maps, mapResourceID, [] ( EntityModel::EntityMap const& map, ResourceID const& mapResourceID ) { return map.GetMapResourceID() == mapResourceID; } );
        KRG_ASSERT( foundMapIter != m_maps.end() );
        foundMapIter->Unload( m_loadingContext );
    }

    //-------------------------------------------------------------------------

    void EntityWorld::UpdateLoading()
    {
        KRG_PROFILE_GROUPED_SCOPE_COLOR( "Entity", "World Loading", MP_LIMEGREEN );

        //-------------------------------------------------------------------------
        // Process map load requests
        //-------------------------------------------------------------------------

        for ( S32 i = (S32) m_maps.size() - 1; i >= 0; i-- )
        {
            m_maps[i].UpdateLoading( m_loadingContext );

            if ( m_maps[i].IsUnloaded() )
            {
                m_maps.pop_back();
            }
        }

        ////-------------------------------------------------------------------------
        //// Update Entity Loads
        ////-------------------------------------------------------------------------

        //for ( S32 i = (S32) m_activeLoaders.size() - 1; i >= 0; i-- )
        //{
        //    auto pEntityLoader = m_activeLoaders[i];
        //    pEntityLoader->UpdateLoading( m_loadingContext );

        //    if ( pEntityLoader->IsLoaded() )
        //    {
        //        for ( auto pEntity : pEntityLoader->GetEntities() )
        //        {
        //            InitializeEntity( pEntity );
        //        }

        //        // For reload operations we have an explicit list of entities to reactivate
        //        //-------------------------------------------------------------------------

        //        if ( pEntityLoader->IsReloadOperation() )
        //        {
        //            for ( auto pEntityToActivate : pEntityLoader->GetEntitiesToReactivate() )
        //            {
        //                if ( pEntityToActivate->IsLoaded() )
        //                {
        //                    continue;
        //                }
        //            }
        //        }

        //        // Remove the loader as we are done with it
        //        //-------------------------------------------------------------------------

        //        KRG::Delete( pEntityLoader );
        //        VectorEraseUnsorted( m_activeLoaders, i );
        //    }
        //}

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
        //TInlineVector<Entity*, 5> attachmentHierarchy;

        //for ( auto pEntity : entities )
        //{
        //    KRG_ASSERT( pEntity != nullptr );

        //    //-------------------------------------------------------------------------

        //    if( pEntity->IsInitialized() )
        //    {
        //        ShutdownEntity( pEntity );
        //    }
        //}

        //// Create the entity reloader and set the list of entities that need to be activated
        ////-------------------------------------------------------------------------

        //auto pEntityReloader = m_activeLoaders.emplace_back( KRG::New<EntityModel::EntityLoader>( EntityModel::EntityLoader::Reload( m_loadingContext, entities ) ) );
        //pEntityReloader->SetEntitiesToReactivate( attachmentHierarchy );
    }
}