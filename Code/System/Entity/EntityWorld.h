#pragma once

#include "_Module/API.h"
#include "EntityLoader.h"
#include "EntityComponent.h"
#include "EntityGlobalSystem.h"
#include "Entity.h"
#include "System/Core/Types/Containers.h"
#include "System/Resource/ResourcePtr.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class EntityCollection;
    class EntityMap;
    class EntityMapInstance;
    class TaskSystem;

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_ENTITY_API EntityWorld
    {
        friend class EntityDebugViewController;

        // This function will generate a tree of all attached entities ordered in a depth first manner.
        inline static void GenerateEntitySpatialAttachmentHierarchy( Entity* pEntity, TInlineVector<Entity*, 5>& outHierarchy )
        {
            outHierarchy.emplace_back( pEntity );
            for ( auto pAttachedEntity : pEntity->m_attachedEntities )
            {
                GenerateEntitySpatialAttachmentHierarchy( pAttachedEntity, outHierarchy );
            }
        }

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
        // Loading
        //-------------------------------------------------------------------------

        inline bool IsBusyLoading() const { return ( m_loadingMaps.size() + m_mapLoadRequests.size() + m_mapUnloadRequests.size() ) > 0; }
        inline bool HasLoadedMaps() const { return !m_loadedMaps.empty(); }
        bool IsMapLoaded( ResourceID const& mapResourceID ) const;
        bool IsMapLoadedOrLoading( ResourceID const& mapResourceID ) const;

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
        EntityLoadingContext                                m_loadingContext;
        TVector<IGlobalEntitySystem*>                       m_globalSystems;

        // Map loading
        TVector<ResourceID>                                 m_mapLoadRequests;
        TVector<ResourceID>                                 m_mapUnloadRequests;
        TVector<EntityMapInstance*>                         m_loadingMaps;
        TVector<EntityMapInstance*>                         m_loadedMaps;

        // Entity loading
        TVector<EntityLoader*>                              m_activeLoaders;

        // Entities
        THashMap<UUID, Entity*>                             m_entityLookupMap;
        TVector<Entity*>                                    m_entityUpdateLists[(S8) UpdateStage::NumStages];
        TVector<IGlobalEntitySystem*>                       m_systemUpdateLists[(S8) UpdateStage::NumStages];
        bool                                                m_initialized = false;
    };
}