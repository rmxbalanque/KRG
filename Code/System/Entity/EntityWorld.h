#pragma once

#include "_Module/API.h"
#include "EntityComponent.h"
#include "EntityGlobalSystem.h"
#include "Entity.h"
#include "Map/EntityMap.h"
#include "System/Core/Types/Containers.h"
#include "System/Resource/ResourcePtr.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class TaskSystem;

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_ENTITY_API EntityWorld
    {
        friend class EntityDebugViewController;

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

    private:

        void InitializeEntity( Entity* pEntity );
        void ShutdownEntity( Entity* pEntity );

        // Hot-reload support
        void ReloadEntities( TVector<Entity*> const& entities );

    private:

        SystemRegistry const*                               m_pSystemsRegistry = nullptr;
        TaskSystem*                                         m_pTaskSystem = nullptr;
        EntityModel::LoadingContext                         m_loadingContext;
        TVector<IGlobalEntitySystem*>                       m_globalSystems;

        // Maps
        TInlineVector<EntityModel::EntityMap,1>             m_maps;

        // Entities
        THashMap<UUID, Entity*>                             m_entityLookupMap;
        TVector<Entity*>                                    m_entityUpdateLists[(S8) UpdateStage::NumStages];
        TVector<IGlobalEntitySystem*>                       m_systemUpdateLists[(S8) UpdateStage::NumStages];
        bool                                                m_initialized = false;
    };
}