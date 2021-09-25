#pragma once

#include "EntityComponent.h"
#include "EntityWorldSystem.h"
#include "Entity.h"
#include "Map/EntityMap.h"
#include "System/Resource/ResourcePtr.h"
#include "System/Core/Types/Containers.h"
#include "System/Core/Systems/ISystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class TaskSystem;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_CORE_API EntityWorld : public ISystem
    {
        friend class EntityDebugViewController;

    public:

        KRG_SYSTEM_ID( EntityWorld );

    public:

        ~EntityWorld();

        void Initialize( SystemRegistry const& systemsRegistry );
        void Shutdown();

        void Update( UpdateContext const& context );

        //-------------------------------------------------------------------------
        // World Systems
        //-------------------------------------------------------------------------

        void RegisterWorldSystem( IWorldEntitySystem* pSystem );
        void UnregisterWorldSystem( IWorldEntitySystem* pSystem );

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

        // This function will handle all actual loading/unloading operations for the world/maps.
        // Any queued requests will be handled here as will any requests to the resource system.
        void UpdateLoading();

        // Process entity registration/unregistration requests occurring during map loading
        void ProcessEntityRegistrationRequests();

        // Process component registration/unregistration requests occurring during map loading
        void ProcessComponentRegistrationRequests();

        //-------------------------------------------------------------------------
        // Events
        //-------------------------------------------------------------------------

        inline TMultiUserEvent<EntityModel::EntityMap*> OnCreatePersistentEntities() { return m_createPersistentEntitiesEvent; }

    private:

        SystemRegistry const*                                       m_pSystemsRegistry = nullptr;
        TaskSystem*                                                 m_pTaskSystem = nullptr;
        EntityModel::LoadingContext                                 m_loadingContext;
        EntityModel::ActivationContext                              m_activationContext;
        TVector<IWorldEntitySystem*>                                m_worldSystems;

        // Events
        TMultiUserEventInternal<EntityModel::EntityMap*>            m_createPersistentEntitiesEvent;

        // Maps
        TInlineVector<EntityModel::EntityMap,3>                     m_maps;

        // Entities
        THashMap<UUID, Entity*>                                     m_entityLookupMap;
        TVector<Entity*>                                            m_entityUpdateList;
        TVector<IWorldEntitySystem*>                                m_systemUpdateLists[(int8) UpdateStage::NumStages];
        bool                                                        m_initialized = false;
    };
}