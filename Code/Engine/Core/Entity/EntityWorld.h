#pragma once

#include "EntityWorldSystem.h"
#include "EntityActivationContext.h"
#include "EntityLoadingContext.h"
#include "Entity.h"
#include "EntityMap.h"
#include "System/Render/RenderViewport.h"
#include "System/Core/Types/Containers.h"
#include "System/Core/Drawing/DebugDrawingSystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class TaskSystem;
    class UpdateContext;
    class EntityWorldDebugView;

    //-------------------------------------------------------------------------

    enum class EntityWorldType : uint8
    {
        Game,
        Editor
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_CORE_API EntityWorld
    {
        friend class EntityDebugView;
        friend class EntityWorldUpdateContext;

    public:

        EntityWorld( EntityWorldType worldType = EntityWorldType::Game ) : m_worldType( worldType ) {}
        ~EntityWorld();

        inline EntityWorldID const& GetID() const { return m_worldID; }
        inline bool IsGameWorld() const { return m_worldType == EntityWorldType::Game; }

        void Initialize( SystemRegistry const& systemsRegistry, TVector<TypeSystem::TypeInfo const*> worldSystemTypeInfos );
        void Shutdown();

        //-------------------------------------------------------------------------
        // Updates
        //-------------------------------------------------------------------------

        // Have the world updates been suspended?
        inline bool IsSuspended() const { return m_isSuspended; }

        // Put this world to sleep, no entity/system updates will be run
        void SuspendUpdates() { m_isSuspended = true; }

        // Wake this world, resumes execution of entity/system updates
        void ResumeUpdates() { m_isSuspended = false; }

        // Run entity and system updates
        void Update( UpdateContext const& context );

        // This function will handle all actual loading/unloading operations for the world/maps.
        // Any queued requests will be handled here as will any requests to the resource system.
        void UpdateLoading();

        //-------------------------------------------------------------------------
        // Time Management
        //-------------------------------------------------------------------------

        // Is the current world paused
        inline bool IsPaused() const { return m_timeScale <= 0.0f; }

        // Get the current time scale for this world
        inline float GetTimeScale() const { return m_timeScale; }

        // Get the current time scale for this world
        inline void SetTimeScale( float newTimeScale ) { m_timeScale = newTimeScale; }

        // Request a time step - only applicable to paused worlds
        inline void RequestTimeStep() { KRG_ASSERT( IsPaused() ); m_timeStepRequested = true; }

        // Has a time-step for a paused world been requested?
        inline bool IsTimeStepRequested() const { return m_timeStepRequested; }

        //-------------------------------------------------------------------------
        // Systems
        //-------------------------------------------------------------------------

        IWorldEntitySystem* GetWorldSystem( uint32 worldSystemID ) const;

        template<typename T>
        inline T* GetWorldSystem() const { return static_cast<T*>( GetWorldSystem( T::s_entitySystemID ) ); }

        //-------------------------------------------------------------------------
        // Viewport
        //-------------------------------------------------------------------------

        inline Render::Viewport* GetViewport() { return &m_viewport; }
        inline Render::Viewport const* GetViewport() const { return &m_viewport; }

        //-------------------------------------------------------------------------
        // Map Management
        //-------------------------------------------------------------------------

        // Get the persistent map
        EntityModel::EntityMap* GetPersistentMap() { return &m_maps[0]; }
        EntityModel::EntityMap const* GetPersistentMap() const { return &m_maps[0]; }

        // Do we have any loaded maps (maps can be either loaded or still loading)
        inline bool HasLoadedMaps() const { return !m_maps.empty(); }

        // Are we currently loading anything
        bool IsBusyLoading() const;

        // Have we requested this map to be loaded (doesnt actually return the load status of the map, as that info should not be needed externally)
        bool IsMapLoaded( ResourceID const& mapResourceID ) const;

        // These functions queue up load and unload requests to be processed during the next loading update for the world
        void LoadMap( ResourceID const& mapResourceID );
        void UnloadMap( ResourceID const& mapResourceID );

        // Find an entity in the map
        inline Entity* FindEntity( EntityID entityID ) const
        {
            Entity* pEntity = nullptr;
            for ( auto const& map : m_maps )
            {
                pEntity = map.FindEntity( entityID );
                if ( pEntity != nullptr )
                {
                    break;
                }
            }

            return pEntity;
        }

        //-------------------------------------------------------------------------
        // Editor
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        
        // Get a specific loaded map
        EntityModel::EntityMap* GetMap( ResourceID const& mapResourceID );

        // This function will immediately unload the specified component so that its properties can be edited
        void PrepareComponentForEditing( ResourceID const& mapID, EntityID const& entityID, ComponentID const& componentID );

        // Get all the registered components of the specified type
        inline TVector<EntityComponent const*> const& GetAllRegisteredComponentsOfType( TypeSystem::TypeID typeID ) { return m_componentTypeLookup[typeID]; }

        // Get all the registered components of the specified type
        template<typename T>
        inline TInlineVector<T const*, 20> GetAllRegisteredComponentsOfType() 
        {
            TInlineVector<T const*, 20> results;
            for ( auto pComponent : m_componentTypeLookup[T::GetStaticTypeID()] )
            {
                results.emplace_back( static_cast<T const*>( pComponent ) );
            }
            return results;
        }

        #endif

        //-------------------------------------------------------------------------
        // Debug
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        inline TVector<EntityWorldDebugView*> const& GetDebugViews() const { return m_debugViews; }
        void InitializeDebugViews( SystemRegistry const& systemsRegistry, TVector<TypeSystem::TypeInfo const*> debugViewTypeInfos );
        void ShutdownDebugViews();

        inline Drawing::DrawingSystem* GetDebugDrawingSystem() { return &m_debugDrawingSystem; }
        inline void ResetDebugDrawingSystem() { m_debugDrawingSystem.Reset(); }
        #endif

        //-------------------------------------------------------------------------
        // Hot Reload
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        // Starts the hot-reload process - deactivates and unloads all specified entities
        void BeginHotReload( TVector<Resource::ResourceRequesterID> const& usersToReload );

        // Ends the hot-reload process - starts re-loading of unloaded entities
        void EndHotReload();
        #endif

    private:

        // Process entity registration/unregistration requests occurring during map loading
        void ProcessEntityRegistrationRequests();

        // Process component registration/unregistration requests occurring during map loading
        void ProcessComponentRegistrationRequests();

    private:

        EntityWorldID                                                           m_worldID = UUID::GenerateID();
        TaskSystem*                                                             m_pTaskSystem = nullptr;
        EntityModel::EntityLoadingContext                                       m_loadingContext;
        EntityModel::ActivationContext                                          m_activationContext;
        TVector<IWorldEntitySystem*>                                            m_worldSystems;
        EntityWorldType                                                         m_worldType = EntityWorldType::Game;
        Render::Viewport                                                        m_viewport = Render::Viewport( Int2::Zero, Int2( 640, 480 ), Math::ViewVolume( Float2( 640, 480 ), FloatRange( 0.1f, 100.0f ) ) );
        float                                                                   m_timeScale = 1.0f; // <= 0 means that the world is paused
        bool                                                                    m_timeStepRequested = false;

        // Maps
        TInlineVector<EntityModel::EntityMap, 3>                                m_maps;

        // Entities
        TVector<Entity*>                                                        m_entityUpdateList;
        TVector<IWorldEntitySystem*>                                            m_systemUpdateLists[(int8) UpdateStage::NumStages];
        bool                                                                    m_initialized = false;
        bool                                                                    m_isSuspended = false;

        #if KRG_DEVELOPMENT_TOOLS
        THashMap<TypeSystem::TypeID, TVector<EntityComponent const*>>           m_componentTypeLookup;
        Drawing::DrawingSystem                                                  m_debugDrawingSystem;
        TVector<EntityWorldDebugView*>                                          m_debugViews;
        #endif
    };
}