#pragma once

#include "../_Module/API.h"
#include "EntityMapDescriptor.h"
#include "System/Entity/Collections/EntityCollection.h"
#include "System/Resource/ResourcePtr.h"
#include "System/Core/Types/Event.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class Entity;
    class EntityWorld;

    //-------------------------------------------------------------------------

    namespace EntityModel
    {
        struct LoadingContext;
        class EntityCollectionDescriptor;

        //-------------------------------------------------------------------------
        // Entity Map
        //-------------------------------------------------------------------------
        // This is a logical grouping of entities whose lifetime is linked
        // e.g. the game level, a cell in a open world game, etc.
        //
        // Maps manage lifetime, loading and activation of entities
        //-------------------------------------------------------------------------

        class KRG_SYSTEM_ENTITY_API EntityMap
        {
            friend EntityWorld;

            enum class Status
            {
                LoadFailed = -1,
                Unloaded = 0,
                MapLoading,
                EntitiesLoading,
                Loaded,
                Activated,
            };

        public:

            EntityMap( EntityMap const& map );
            EntityMap( EntityMap&& map );
            ~EntityMap();

            EntityMap& operator=( EntityMap const& map );
            EntityMap& operator=( EntityMap&& map );

            // Map Info
            //-------------------------------------------------------------------------

            inline ResourceID const& GetMapResourceID() const { return m_pMapDesc.GetResourceID(); }
            inline UUID GetMapID() const { return m_pCollection->GetID(); }
            inline bool IsTransientMap() const { return m_isTransientMap; }

            // Map Status
            //-------------------------------------------------------------------------

            bool IsLoading() const { return m_status == Status::MapLoading || m_status == Status::EntitiesLoading; }
            inline bool IsLoaded() const { return m_status == Status::Loaded; }
            inline bool IsUnloaded() const { return m_status == Status::Unloaded; }
            inline bool HasLoadingFailed() const { return m_status == Status::LoadFailed; }
            inline bool IsActivated() const { return m_status == Status::Activated; }

            // Entity API
            //-------------------------------------------------------------------------

            inline TVector<Entity*> const& GetEntities() const { KRG_ASSERT( m_pCollection != nullptr ); return m_pCollection->GetEntities(); }
            Entity* FindEntity( UUID entityID ) const;

            // Add a newly created entity to the map - Transfers ownership of the entity to the map
            // Will take 1 frame to be fully added, as the addition occurs during the loading update
            void AddEntity( Entity* pEntity );

            // Unload and remove an entity from the map - Transfer ownership of the entity to the calling code
            // Will take 1 frame to be fully removed, as the removal occurs during the loading update
            Entity* RemoveEntity( UUID entityID );

        private:

            EntityMap(); // Default constructor creates a transient map
            EntityMap( ResourceID mapResourceID );

            // Loading and Activation
            //-------------------------------------------------------------------------
            // Note: Transient maps cannot be loaded/unloaded

            void Load( LoadingContext const& loadingContext );
            void Unload( LoadingContext const& loadingContext );

            void Activate( LoadingContext const& loadingContext );
            void Deactivate( LoadingContext const& loadingContext );

            //-------------------------------------------------------------------------

            void OnEntityStateUpdated( Entity* pEntity );

            // Updates map loading and entity state, returns true if all loading/state changes are complete, false otherwise
            bool UpdateState( LoadingContext const& loadingContext );

        private:

            TResourcePtr<EntityMapDescriptor>           m_pMapDesc;
            EntityCollection*                           m_pCollection = nullptr;
            TVector<Entity*>                            m_loadingEntities;
            TVector<Entity*>                            m_entitiesToReload;
            TInlineVector<Entity*, 5>                   m_entitiesToAdd;
            TInlineVector<Entity*, 5>                   m_entitiesToRemove;
            Status                                      m_status = Status::Unloaded;
            EventBindingID                              m_entityUpdateEventBindingID;
            bool                                        m_unloadRequested = false;
            bool const                                  m_isTransientMap = false; // If this is set, then this is a transient map i.e.created and managed at runtime and not loaded from disk
        };
    }
}