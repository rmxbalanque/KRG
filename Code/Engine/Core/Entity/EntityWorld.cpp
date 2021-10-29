#include "EntityWorld.h"
#include "EntityUpdateContext.h"
#include "Debug/EntityWorldDebugView.h"
#include "System/Resource/ResourceSystem.h"
#include "System/Core/Profiling/Profiling.h"
#include "System/Core/Settings/DebugSettings.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Settings
    {
        #if KRG_DEVELOPMENT_TOOLS
        static DebugSettingBool g_showEntityWorldLoadErrors( "ShowEntityWorldLoadErrors", "Entity/World", "", false );
        #endif
    }

    //-------------------------------------------------------------------------

    EntityWorld::~EntityWorld()
    {
        KRG_ASSERT( m_maps.empty());
        KRG_ASSERT( m_worldSystems.empty() );
        KRG_ASSERT( m_entityUpdateList.empty() );

        for ( int8 i = 0; i < (int8) UpdateStage::NumStages; i++ )
        {
            KRG_ASSERT( m_systemUpdateLists[i].empty() );
        }

        #if KRG_DEVELOPMENT_TOOLS
        KRG_ASSERT( m_debugViews.empty() );
        #endif
    }

    void EntityWorld::Initialize( SystemRegistry const& systemsRegistry, TVector<TypeSystem::TypeInfo const*> worldSystemTypeInfos )
    {
        m_pTaskSystem = systemsRegistry.GetSystem<TaskSystem>();
        KRG_ASSERT( m_pTaskSystem != nullptr );

        m_loadingContext = EntityModel::LoadingContext( m_pTaskSystem, systemsRegistry.GetSystem<TypeSystem::TypeRegistry>(), systemsRegistry.GetSystem<Resource::ResourceSystem>() );
        KRG_ASSERT( m_loadingContext.IsValid() );

        // Create World Systems
        //-------------------------------------------------------------------------

        for ( auto pTypeInfo : worldSystemTypeInfos )
        {
            // Create and initialize world system
            auto pWorldSystem = Cast<IWorldEntitySystem>( pTypeInfo->m_pTypeHelper->CreateType() );
            pWorldSystem->InitializeSystem( systemsRegistry );
            m_worldSystems.push_back( pWorldSystem );

            // Add to update lists
            for ( int8 i = 0; i < (int8) UpdateStage::NumStages; i++ )
            {
                if ( pWorldSystem->GetRequiredUpdatePriorities().IsStageEnabled( (UpdateStage) i ) )
                {
                    m_systemUpdateLists[i].push_back( pWorldSystem );
                }

                // Sort update list
                auto comparator = [i] ( IWorldEntitySystem* const& pSystemA, IWorldEntitySystem* const& pSystemB )
                {
                    uint8 const A = pSystemA->GetRequiredUpdatePriorities().GetPriorityForStage( (UpdateStage) i );
                    uint8 const B = pSystemB->GetRequiredUpdatePriorities().GetPriorityForStage( (UpdateStage) i );
                    return A > B;
                };

                eastl::sort( m_systemUpdateLists[i].begin(), m_systemUpdateLists[i].end(), comparator );
            }
        }

        // Create and activate the persistent map
        //-------------------------------------------------------------------------

        m_maps.emplace_back( EntityModel::EntityMap() );
        m_maps[0].Load( m_loadingContext );
        m_maps[0].Activate( m_loadingContext, m_activationContext );

        //-------------------------------------------------------------------------

        m_initialized = true;
    }

    void EntityWorld::Shutdown()
    {
        // Unload maps
        //-------------------------------------------------------------------------
        
        for ( auto& map : m_maps )
        {
            map.Unload( m_loadingContext );
        }

        // Run the loading update as this will immediately unload all maps
        //-------------------------------------------------------------------------

        while ( !m_maps.empty() )
        {
            UpdateLoading();
        }

        // Shutdown all world systems
        //-------------------------------------------------------------------------

        for( auto pWorldSystem : m_worldSystems )
        {
            // Remove from update lists
            for ( int8 i = 0; i < (int8) UpdateStage::NumStages; i++ )
            {
                if ( pWorldSystem->GetRequiredUpdatePriorities().IsStageEnabled( (UpdateStage) i ) )
                {
                    auto updateIter = VectorFind( m_systemUpdateLists[i], pWorldSystem );
                    KRG_ASSERT( updateIter != m_systemUpdateLists[i].end() );
                    m_systemUpdateLists[i].erase( updateIter );
                }
            }

            // Shutdown and destroy world system
            pWorldSystem->ShutdownSystem();
            KRG::Delete( pWorldSystem );
        }

        m_worldSystems.clear();

        //-------------------------------------------------------------------------

        m_pTaskSystem = nullptr;
        m_initialized = false;
    }

    #if KRG_DEVELOPMENT_TOOLS
    void EntityWorld::InitializeDebugViews( SystemRegistry const& systemsRegistry, TVector<TypeSystem::TypeInfo const*> debugViewTypeInfos )
    {
        for ( auto pTypeInfo : debugViewTypeInfos )
        {
            auto pDebugView = Cast<EntityWorldDebugView>( pTypeInfo->m_pTypeHelper->CreateType() );
            pDebugView->Initialize( systemsRegistry, this );
            m_debugViews.push_back( pDebugView );
        }
    }

    void EntityWorld::ShutdownDebugViews()
    {
        for ( auto pDebugView : m_debugViews )
        {
            // Shutdown and destroy world system
            pDebugView->Shutdown();
            KRG::Delete( pDebugView );
        }

        m_debugViews.clear();
    }
    #endif

    //-------------------------------------------------------------------------
    // Misc
    //-------------------------------------------------------------------------

    IWorldEntitySystem* EntityWorld::GetWorldSystem( uint32 worldSystemID ) const
    {
        for ( IWorldEntitySystem* pWorldSystem : m_worldSystems )
        {
            if ( pWorldSystem->GetSystemID() == worldSystemID )
            {
                return pWorldSystem;
            }
        }

        KRG_UNREACHABLE_CODE();
        return nullptr;
    }

    //-------------------------------------------------------------------------
    // Frame Update
    //-------------------------------------------------------------------------

    void EntityWorld::Update( UpdateContext const& context )
    {
        KRG_ASSERT( Threading::IsMainThread() );

        struct EntityUpdateTask : public IAsyncTask
        {
            EntityUpdateTask( EntityUpdateContext const& context, TVector<Entity*>& updateList )
                : m_context( context )
                , m_updateList( updateList )
            {
                m_SetSize = (uint32) updateList.size();
            }

            // Only used for spatial dependency chain updates
            inline void RecursiveEntityUpdate( Entity* pEntity )
            {
                pEntity->UpdateSystems( m_context );

                for ( auto pAttachedEntity : pEntity->m_attachedEntities )
                {
                    RecursiveEntityUpdate( pAttachedEntity );
                }
            }

            virtual void ExecuteRange( TaskSetPartition range, uint32 threadnum ) override final
            {
                for ( uint64 i = range.start; i < range.end; ++i )
                {
                    auto pEntity = m_updateList[i];

                    // Ignore any entities with spatial parents these will be removed from the update list
                    if ( pEntity->HasSpatialParent() )
                    {
                        continue;
                    }

                    //-------------------------------------------------------------------------

                    if ( pEntity->HasAttachedEntities() )
                    {
                        KRG_PROFILE_SCOPE_SCENE( "Update Entity Chain" );
                        RecursiveEntityUpdate( pEntity );
                    }
                    else // Direct entity update
                    {
                        KRG_PROFILE_SCOPE_SCENE( "Update Entity" );
                        pEntity->UpdateSystems( m_context );
                    }
                }
            }

        private:

            EntityUpdateContext const&              m_context;
            TVector<Entity*>&                       m_updateList;
        };

        //-------------------------------------------------------------------------

        EntityUpdateContext entityUpdateContext( context, this );
        UpdateStage const updateStage =  entityUpdateContext.GetUpdateStage();

        // Clear debug drawing
        //-------------------------------------------------------------------------

        if ( updateStage == UpdateStage::FrameStart )
        {
            #if KRG_DEVELOPMENT_TOOLS
            m_debugDrawingSystem.Reset();
            #endif
        }

        // Update entities
        //-------------------------------------------------------------------------

        EntityUpdateTask entityUpdateTask( entityUpdateContext, m_entityUpdateList );
        m_pTaskSystem->ScheduleTask( &entityUpdateTask );
        m_pTaskSystem->WaitForTask( &entityUpdateTask );

        // Update systems
        //-------------------------------------------------------------------------

        for ( auto pSystem : m_systemUpdateLists[(int8) updateStage] )
        {
            KRG_PROFILE_SCOPE_SCENE( "Update World Systems" );
            KRG_ASSERT( pSystem->GetRequiredUpdatePriorities().IsStageEnabled( updateStage ) );
            pSystem->UpdateSystem( entityUpdateContext );
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
        KRG_PROFILE_SCOPE_SCENE( "World Loading" );

        // Update all maps internal loading state
        //-------------------------------------------------------------------------
        // This will fill the world activation/registration lists used below
        // This will also handle all hot-reload unload/load requests

        for ( int32 i = (int32) m_maps.size() - 1; i >= 0; i-- )
        {
            if ( m_maps[i].UpdateState( m_loadingContext, m_activationContext ) )
            {
                if ( m_maps[i].IsLoaded() )
                {
                    m_maps[i].Activate( m_loadingContext, m_activationContext );
                }
                else if ( m_maps[i].IsUnloaded() )
                {
                    m_maps.erase_unsorted( m_maps.begin() + i );
                }
            }
        }

        //-------------------------------------------------------------------------

        ProcessEntityRegistrationRequests();
        ProcessComponentRegistrationRequests();
    }

    void EntityWorld::ProcessEntityRegistrationRequests()
    {
        {
            KRG_PROFILE_SCOPE_SCENE( "Entity Unregistration" );

            Entity* pEntityToUnregister = nullptr;
            while ( m_activationContext.m_unregisterForEntityUpdate.try_dequeue( pEntityToUnregister ) )
            {
                KRG_ASSERT( pEntityToUnregister != nullptr && pEntityToUnregister->m_isRegisteredForUpdates );
                m_entityUpdateList.erase_first_unsorted( pEntityToUnregister );
                pEntityToUnregister->m_isRegisteredForUpdates = false;
            }
        }

        //-------------------------------------------------------------------------

        {
            KRG_PROFILE_SCOPE_SCENE( "Entity Registration" );

            Entity* pEntityToRegister = nullptr;
            while ( m_activationContext.m_registerForEntityUpdate.try_dequeue( pEntityToRegister ) )
            {
                KRG_ASSERT( pEntityToRegister != nullptr && pEntityToRegister->IsActivated() && !pEntityToRegister->m_isRegisteredForUpdates );
                KRG_ASSERT( !pEntityToRegister->HasSpatialParent() ); // Attached entities are not allowed to be directly updated
                m_entityUpdateList.push_back( pEntityToRegister );
                pEntityToRegister->m_isRegisteredForUpdates = true;
            }
        }
    }

    void EntityWorld::ProcessComponentRegistrationRequests()
    {
        // Create a task that splits per-system registration across multiple threads
        struct ComponentRegistrationTask : public IAsyncTask
        {
            ComponentRegistrationTask( TVector<IWorldEntitySystem*> const& worldSystems, TVector< TPair<Entity*, EntityComponent*> > const& componentsToRegister, TVector< TPair<Entity*, EntityComponent*> > const& componentsToUnregister )
                : m_worldSystems( worldSystems )
                , m_componentsToRegister( componentsToRegister )
                , m_componentsToUnregister( componentsToUnregister )
            {
                m_SetSize = (uint32) worldSystems.size();
            }

            virtual void ExecuteRange( TaskSetPartition range, uint32 threadnum ) override final
            {
                KRG_PROFILE_SCOPE_SCENE( "Component World Registration Task" );

                for ( uint64 i = range.start; i < range.end; ++i )
                {
                    auto pSystem = m_worldSystems[i];

                    // Unregister components
                    //-------------------------------------------------------------------------
 
                    size_t const numComponentsToUnregister = m_componentsToUnregister.size();
                    for ( auto c = 0u; c < numComponentsToUnregister; c++ )
                    {
                        auto pEntity = m_componentsToUnregister[c].first;
                        auto pComponent = m_componentsToUnregister[c].second;

                        KRG_ASSERT( pEntity != nullptr );
                        KRG_ASSERT( pComponent != nullptr && pComponent->IsInitialized() && pComponent->m_isRegisteredWithWorld );
                        pSystem->UnregisterComponent( pEntity, pComponent );
                    }

                    // Register components
                    //-------------------------------------------------------------------------

                    size_t const numComponentsToRegister = m_componentsToRegister.size();
                    for ( auto c = 0u; c < numComponentsToRegister; c++ )
                    {
                        auto pEntity = m_componentsToRegister[c].first;
                        auto pComponent = m_componentsToRegister[c].second;

                        KRG_ASSERT( pEntity != nullptr && pEntity->IsActivated() && !pComponent->m_isRegisteredWithWorld );
                        KRG_ASSERT( pComponent != nullptr && pComponent->IsInitialized() );
                        pSystem->RegisterComponent( pEntity, pComponent );
                    }
                }
            }

        private:

            TVector<IWorldEntitySystem*> const&                       m_worldSystems;
            TVector< TPair<Entity*, EntityComponent*> > const&         m_componentsToRegister;
            TVector< TPair<Entity*, EntityComponent*> > const&         m_componentsToUnregister;
        };

        //-------------------------------------------------------------------------

        {
            KRG_PROFILE_SCOPE_SCENE( "Component World Registration" );

            // Get Components to register/unregister
            //-------------------------------------------------------------------------

            TVector< TPair<Entity*, EntityComponent*>> componentsToUnregister;
            size_t const numComponentsToUnregister = m_activationContext.m_componentsToUnregister.size_approx();
            componentsToUnregister.resize( numComponentsToUnregister );

            size_t numDequeued = m_activationContext.m_componentsToUnregister.try_dequeue_bulk( componentsToUnregister.data(), numComponentsToUnregister );
            KRG_ASSERT( numComponentsToUnregister == numDequeued );

            //-------------------------------------------------------------------------

            TVector< TPair<Entity*, EntityComponent*>> componentsToRegister;
            size_t const numComponentsToRegister = m_activationContext.m_componentsToRegister.size_approx();
            componentsToRegister.resize( numComponentsToRegister );

            numDequeued = m_activationContext.m_componentsToRegister.try_dequeue_bulk( componentsToRegister.data(), numComponentsToRegister );
            KRG_ASSERT( numComponentsToRegister == numDequeued );

            // Run registration task
            //-------------------------------------------------------------------------

            ComponentRegistrationTask componentRegistrationTask( m_worldSystems, componentsToRegister, componentsToUnregister );
            //componentRegistrationTask.Run();
            m_loadingContext.m_pTaskSystem->ScheduleTask( &componentRegistrationTask );
            m_loadingContext.m_pTaskSystem->WaitForTask( &componentRegistrationTask );

            // Update component registration flags
            //-------------------------------------------------------------------------

            for ( auto const& unregisteredComponent : componentsToUnregister )
            {
                unregisteredComponent.second->m_isRegisteredWithWorld = false;
            }

            for ( auto const& registeredComponent : componentsToRegister )
            {
                registeredComponent.second->m_isRegisteredWithWorld = true;
            }
        }
    }

    //-------------------------------------------------------------------------
    // Hot Reload
    //-------------------------------------------------------------------------

    #if KRG_DEVELOPMENT_TOOLS
    void EntityWorld::BeginHotReload( TVector<Resource::ResourceRequesterID> const& usersToReload )
    {
        KRG_ASSERT( !usersToReload.empty() );

        // Fill the hot-reload lists per map and deactivate any active entities
        for ( auto& map : m_maps )
        {
            map.HotReloadPrepareEntities( m_loadingContext, m_activationContext, usersToReload );
        }

        // Process all deactivation requests
        ProcessEntityRegistrationRequests();
        ProcessComponentRegistrationRequests();

        // Unload all entities that require hot reload
        for ( auto& map : m_maps )
        {
            map.HotReloadUnloadEntities( m_loadingContext, m_activationContext );
        }
    }

    void EntityWorld::EndHotReload()
    {
        // Load all entities that require hot reload
        for ( auto& map : m_maps )
        {
            map.HotReloadLoadEntities( m_loadingContext, m_activationContext );
        }
    }
    #endif
}