#pragma once

#include "_Module/API.h"
#include "EntityComponent.h"
#include "EntityGlobalSystem.h"
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

    class KRG_SYSTEM_ENTITY_API EntityWorld : public ISystem
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

        void RegisterGlobalSystem( IGlobalEntitySystem* pSystem );
        void UnregisterGlobalSystem( IGlobalEntitySystem* pSystem );

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

        //-------------------------------------------------------------------------
        // Events
        //-------------------------------------------------------------------------

        inline TMultiUserEvent<EntityModel::EntityMap*> OnCreatePersistentEntities() { return m_createPersistentEntitiesEvent; }

    private:

        void RegisterEntityUpdate( Entity* pEntity );
        void UnregisterEntityUpdate( Entity* pEntity );

        void RegisterComponentWithGlobalSystems( Entity* pEntity, EntityComponent* pComponent );
        void UnregisterComponentFromGlobalSystems( Entity* pEntity, EntityComponent* pComponent );

    private:

        SystemRegistry const*                                       m_pSystemsRegistry = nullptr;
        TaskSystem*                                                 m_pTaskSystem = nullptr;
        EntityModel::LoadingContext                                 m_loadingContext;
        TVector<IGlobalEntitySystem*>                               m_globalSystems;

        // Events
        TMultiUserEventInternal<EntityModel::EntityMap*>            m_createPersistentEntitiesEvent;

        // Maps
        TInlineVector<EntityModel::EntityMap,3>                     m_maps;

        // Entities
        THashMap<UUID, Entity*>                                     m_entityLookupMap;
        TVector<Entity*>                                            m_entityUpdateLists[(S8) UpdateStage::NumStages];
        TVector<IGlobalEntitySystem*>                               m_systemUpdateLists[(S8) UpdateStage::NumStages];
        bool                                                        m_initialized = false;
    };
}