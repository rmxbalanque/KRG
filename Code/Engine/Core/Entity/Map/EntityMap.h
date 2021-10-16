#pragma once

#include "EntityMapDescriptor.h"
#include "Engine/Core/Entity/Collections/EntityCollection.h"
#include "System/Resource/ResourcePtr.h"
#include "System/Core/Types/Event.h"
#include "System/Core/Threading/Threading.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class Entity;
    class EntityWorld;

    //-------------------------------------------------------------------------

    namespace EntityModel
    {
        struct LoadingContext;
        struct ActivationContext;
        class EntityCollectionDescriptor;

        //-------------------------------------------------------------------------
        // Entity Map
        //-------------------------------------------------------------------------
        // This is a logical grouping of entities whose lifetime is linked
        // e.g. the game level, a cell in a open world game, etc.
        //
        // Maps manage lifetime, loading and activation of entities
        // 
        // Note: All map operations are threadsafe using a standard mutex
        //-------------------------------------------------------------------------

        class KRG_ENGINE_CORE_API EntityMap : EntityCollection
        {
            enum class Status
            {
                LoadFailed = -1,
                Unloaded = 0,
                MapLoading,
                EntitiesLoading,
                Loaded,
                Activated,
            };

            struct RemovalRequest
            {
                RemovalRequest( Entity* pEntity, bool shouldDestroy ) : m_pEntity( pEntity ), m_shouldDestroy( shouldDestroy ) {}

                Entity*     m_pEntity = nullptr;
                bool        m_shouldDestroy = false;
            };

        public:

            EntityMap(); // Default constructor creates a transient map
            EntityMap( ResourceID mapResourceID );
            EntityMap( EntityMap const& map );
            EntityMap( EntityMap&& map );
            ~EntityMap();

            EntityMap& operator=( EntityMap const& map );
            EntityMap& operator=( EntityMap&& map );

            // Map Info
            //-------------------------------------------------------------------------

            inline ResourceID const& GetMapResourceID() const { return m_pMapDesc.GetResourceID(); }
            inline UUID GetMapID() const { return GetID(); }
            inline bool IsTransientMap() const { return m_isTransientMap; }

            // Loading and Activation
            //-------------------------------------------------------------------------

            void Load( LoadingContext const& loadingContext );
            void Unload( LoadingContext const& loadingContext );

            void Activate( LoadingContext const& loadingContext, EntityModel::ActivationContext& activationContext );
            void Deactivate( LoadingContext const& loadingContext, EntityModel::ActivationContext& activationContext );

            // Map State
            //-------------------------------------------------------------------------

            // Updates map loading and entity state, returns true if all loading/state changes are complete, false otherwise
            bool UpdateState( LoadingContext const& loadingContext, EntityModel::ActivationContext& activationContext );

            bool IsLoading() const { return m_status == Status::MapLoading || m_status == Status::EntitiesLoading; }
            inline bool IsLoaded() const { return m_status == Status::Loaded; }
            inline bool IsUnloaded() const { return m_status == Status::Unloaded; }
            inline bool HasLoadingFailed() const { return m_status == Status::LoadFailed; }
            inline bool IsActivated() const { return m_status == Status::Activated; }

            // Entity API
            //-------------------------------------------------------------------------

            using EntityCollection::GetEntities;
            using EntityCollection::FindEntity;
            using EntityCollection::ContainsEntity;

            // Add a newly created entity to the map - Transfers ownership of the entity to the map
            // Will take 1 frame to be fully added, as the addition occurs during the loading update
            void AddEntity( Entity* pEntity );

            // Unload and remove an entity from the map - Transfer ownership of the entity to the calling code
            // May take multiple frame to be fully destroyed, as the removal occurs during the loading update
            Entity* RemoveEntity( UUID entityID );

            // Unload, remove and destroy entity in this map
            // May take multiple frame to be fully destroyed, as the removal occurs during the loading update
            void DestroyEntity( UUID entityID );

            // Hot-reloading
            //-------------------------------------------------------------------------
            // Hot reloading is a blocking process that runs in stages

            #if KRG_DEVELOPMENT_TOOLS
            // 1st Stage: Fills the hot-reload list and request deactivation for any entities that require a hot-reload
            void HotReloadPrepareEntities( LoadingContext const& loadingContext, EntityModel::ActivationContext& activationContext, TVector<Resource::ResourceRequesterID> const& usersToReload );

            // 2nd Stage: Requests unload of all entities required hot-reload
            void HotReloadUnloadEntities( LoadingContext const& loadingContext, EntityModel::ActivationContext& activationContext );

            // 3rd Stage: Requests load of all entities required hot-reload
            void HotReloadLoadEntities( LoadingContext const& loadingContext, EntityModel::ActivationContext& activationContext );
            #endif

        private:

            void OnEntityStateUpdated( Entity* pEntity );

            //-------------------------------------------------------------------------

            bool ProcessMapUnloadRequest( LoadingContext const& loadingContext, EntityModel::ActivationContext& activationContext );
            bool ProcessMapLoading( LoadingContext const& loadingContext, EntityModel::ActivationContext& activationContext );
            void ProcessEntityAdditionAndRemoval( LoadingContext const& loadingContext, EntityModel::ActivationContext& activationContext );
            bool ProcessEntityLoadingAndActivation( LoadingContext const& loadingContext, EntityModel::ActivationContext& activationContext );

        private:

            Threading::RecursiveMutex                   m_mutex;
            TResourcePtr<EntityMapDescriptor>           m_pMapDesc;
            TVector<Entity*>                            m_entitiesToLoad;
            TInlineVector<Entity*, 5>                   m_entitiesToAdd;
            TInlineVector<RemovalRequest, 5>            m_entitiesToRemove;
            TVector<Entity*>                            m_entitiesToHotReload;
            Status                                      m_status = Status::Unloaded;
            EventBindingID                              m_entityUpdateEventBindingID;
            bool                                        m_isUnloadRequested = false;
            bool                                        m_isCollectionInstantiated = false;
            bool const                                  m_isTransientMap = false; // If this is set, then this is a transient map i.e.created and managed at runtime and not loaded from disk
        };
    }
}