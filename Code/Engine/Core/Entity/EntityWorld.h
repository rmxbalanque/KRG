#pragma once

#include "EntityComponent.h"
#include "EntityWorldSystem.h"
#include "Entity.h"
#include "Map/EntityMap.h"
#include "System/Render/RenderViewport.h"
#include "System/Resource/ResourcePtr.h"
#include "System/Core/Types/Containers.h"
#include "System/Core/Debug/DebugDrawingSystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class TaskSystem;
    class UpdateContext;
    class EntityWorldDebugView;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_CORE_API EntityWorld
    {
        friend class EntityDebugView;
        friend class EntityUpdateContext;

    public:

        ~EntityWorld();

        inline UUID const& GetID() const { return m_worldID; }

        void Initialize( SystemRegistry const& systemsRegistry, TVector<TypeSystem::TypeInfo const*> worldSystemTypeInfos );
        void Shutdown();

        //-------------------------------------------------------------------------
        // Updates
        //-------------------------------------------------------------------------

        // Has the world updates been suspended?
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
        // Misc
        //-------------------------------------------------------------------------

        IWorldEntitySystem* GetWorldSystem( uint32 worldSystemID ) const;

        template<typename T>
        inline T* GetWorldSystem() const { return static_cast<T*>( GetWorldSystem( T::s_entitySystemID ) ); }

        #if KRG_DEVELOPMENT_TOOLS
        inline Debug::DrawingSystem* GetDebugDrawingSystem() { return &m_debugDrawingSystem; }
        #endif

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

        // Do we have any loaded maps (maps can be either loaded or still loading)
        inline bool HasLoadedMaps() const { return !m_maps.empty(); }

        // Are we currently loading anything
        bool IsBusyLoading() const;

        // Have we requested this map to be loaded (doesnt actually return the load status of the map, as that info should not be needed externally)
        bool IsMapLoaded( ResourceID const& mapResourceID ) const;

        // These functions queue up load and unload requests to be processed during the next loading update for the world
        void LoadMap( ResourceID const& mapResourceID );
        void UnloadMap( ResourceID const& mapResourceID );

        #if KRG_DEVELOPMENT_TOOLS
        EntityModel::EntityMap* GetMap( ResourceID const& mapResourceID );
        #endif

        //-------------------------------------------------------------------------
        // Debug Views
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        inline TVector<EntityWorldDebugView*> const& GetDebugViews() const { return m_debugViews; }
        void InitializeDebugViews( SystemRegistry const& systemsRegistry, TVector<TypeSystem::TypeInfo const*> debugViewTypeInfos );
        void ShutdownDebugViews();
        #endif

        //-------------------------------------------------------------------------
        // Editing / Hot Reload
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        // This function will immediately unload the specified component so that its properties can be edited
        void PrepareComponentForEditing( ResourceID const& mapID, UUID const& entityID, UUID const& componentID );

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

        UUID                                                        m_worldID = UUID::GenerateID();
        TaskSystem*                                                 m_pTaskSystem = nullptr;
        EntityModel::LoadingContext                                 m_loadingContext;
        EntityModel::ActivationContext                              m_activationContext;
        TVector<IWorldEntitySystem*>                                m_worldSystems;
        Render::Viewport                                            m_viewport = Render::Viewport( Int2::Zero, Int2( 640, 480 ), Math::ViewVolume( Float2( 640, 480 ), FloatRange( 0.1f, 100.0f ) ) );

        // Maps
        TInlineVector<EntityModel::EntityMap,3>                     m_maps;

        // Entities
        TVector<Entity*>                                            m_entityUpdateList;
        TVector<IWorldEntitySystem*>                                m_systemUpdateLists[(int8) UpdateStage::NumStages];
        bool                                                        m_initialized = false;
        bool                                                        m_isSuspended = false;

        #if KRG_DEVELOPMENT_TOOLS
        Debug::DrawingSystem                                        m_debugDrawingSystem;
        TVector<EntityWorldDebugView*>                              m_debugViews;
        #endif
    };
}