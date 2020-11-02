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
        KRG_ASSERT( !m_createPersistentEntitiesEvent.HasBoundUsers() );
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

        // Bind delegates
        m_loadingContext.m_registerWithGlobalSystems = TFunction<void( Entity*, EntityComponent* )>( [this] ( Entity* pEntity, EntityComponent* pComponent ) { RegisterComponentWithGlobalSystems( pEntity, pComponent ); } );
        m_loadingContext.m_unregisterFromGlobalSystems = TFunction<void( Entity*, EntityComponent* )>( [this] ( Entity* pEntity, EntityComponent* pComponent ) { UnregisterComponentFromGlobalSystems( pEntity, pComponent ); } );
        m_loadingContext.m_registerEntityUpdate = TFunction<void( Entity* )>( [this] ( Entity* pEntity ) { RegisterEntityUpdate( pEntity ); } );
        m_loadingContext.m_unregisterEntityUpdate = TFunction<void( Entity* )>( [this] ( Entity* pEntity ) { UnregisterEntityUpdate( pEntity ); } );

        //-------------------------------------------------------------------------

        m_pTaskSystem = systemsRegistry.GetSystem<TaskSystem>();
        KRG_ASSERT( m_pTaskSystem != nullptr );

        // Initialize all global systems
        for ( auto pSystem : m_globalSystems )
        {
            pSystem->InitializeEntitySystem( systemsRegistry );
        }

        // Create and activate the persistent map
        m_maps.emplace_back( EntityModel::EntityMap() );
        m_maps[0].Load( m_loadingContext );
        m_maps[0].Activate( m_loadingContext );
        m_createPersistentEntitiesEvent.Execute( &m_maps[0] );

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

        UpdateLoading();

        KRG_ASSERT( m_maps.empty() );

        auto pEvent = Entity::OnEntityStateUpdated();

        // Shutdown all global systems
        //-------------------------------------------------------------------------

        for( auto pSystem : m_globalSystems )
        {
            pSystem->ShutdownEntitySystem();
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
            pSystem->UpdateEntitySystem( context );
        }
    }

    //-------------------------------------------------------------------------
    // Systems
    //-------------------------------------------------------------------------

    void EntityWorld::RegisterGlobalSystem( IGlobalEntitySystem* pSystem )
    {
        KRG_ASSERT( pSystem != nullptr && m_initialized == false );

        // Add system
        //-------------------------------------------------------------------------

        // Ensure that we only allow a single system of a specific type - global systems are essentially singletons
        for ( auto pExistingSystem : m_globalSystems )
        {
            if ( pExistingSystem->GetEntitySystemID() == pSystem->GetEntitySystemID() )
            {
                KRG_HALT();
            }
        }
        
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

        for ( S32 i = (S32) m_maps.size() - 1; i >= 0; i-- )
        {
            if ( m_maps[i].UpdateState( m_loadingContext ) )
            {
                if ( m_maps[i].IsLoaded() )
                {
                    m_maps[i].Activate( m_loadingContext );
                }
                else if ( m_maps[i].IsUnloaded() )
                {
                    VectorEraseUnsorted( m_maps, i );
                }
            }
        }
    }

    //-------------------------------------------------------------------------
    // Entity operations
    //-------------------------------------------------------------------------

    void EntityWorld::RegisterEntityUpdate( Entity* pEntity )
    {
        KRG_ASSERT( pEntity != nullptr && pEntity->IsActivated() );

        // Add to update lists
        for ( S8 i = 0; i < (S8) UpdateStage::NumStages; i++ )
        {
            if ( pEntity->RequiresUpdate( (UpdateStage) i ) )
            {
                m_entityUpdateLists[i].push_back( pEntity );
            }
        }
    }

    void EntityWorld::UnregisterEntityUpdate( Entity* pEntity )
    {
        KRG_ASSERT( pEntity != nullptr && pEntity->IsActivated() );

        // Remove from update lists
        for ( S8 i = 0; i < (S8) UpdateStage::NumStages; i++ )
        {
            if ( pEntity->RequiresUpdate( (UpdateStage) i ) )
            {
                m_entityUpdateLists[i].erase_first( pEntity );
            }
        }
    }

    void EntityWorld::RegisterComponentWithGlobalSystems( Entity* pEntity, EntityComponent* pComponent )
    {
        KRG_ASSERT( pEntity != nullptr && pEntity->IsActivated() );
        KRG_ASSERT( pComponent != nullptr && pComponent->IsInitialized() );

        for ( auto pGlobalSystem : m_globalSystems )
        {
            pGlobalSystem->RegisterComponent( pEntity, pComponent );
        }
    }

    void EntityWorld::UnregisterComponentFromGlobalSystems( Entity* pEntity, EntityComponent* pComponent )
    {
        KRG_ASSERT( pEntity != nullptr && pEntity->IsActivated() );
        KRG_ASSERT( pComponent != nullptr && pComponent->IsInitialized() );

        for ( auto pGlobalSystem : m_globalSystems )
        {
            pGlobalSystem->UnregisterComponent( pEntity, pComponent );
        }
    }
}