#pragma once

#include "../_Module/API.h"
#include "EntityMapDescriptor.h"
#include "System/Entity/Collections/EntityCollection.h"
#include "System/Resource/ResourcePtr.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class Entity;

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

        public:

            enum class Status
            {
                LoadFailed = -1,
                Unloaded = 0,
                MapDescLoading,
                MapDescLoaded,
                EntitiesLoading,
                Loaded,
                Activated,
            };

        public:

            EntityMap(); // Default constructor creates a transient map
            EntityMap( ResourceID mapResourceID ) : m_pMapDesc( mapResourceID ) {}
            EntityMap( EntityMap const& map ) { operator=( map ); }
            EntityMap( EntityMap&& map ) { operator=( eastl::move( map ) ); }
            ~EntityMap();

            EntityMap& operator=( EntityMap const& map );
            EntityMap& operator=( EntityMap&& map );

            // Map Info
            //-------------------------------------------------------------------------

            inline ResourceID const& GetMapResourceID() const { return m_pMapDesc.GetResourceID(); }
            inline bool IsTransientMap() const { return m_isTransientMap; }
            inline UUID GetMapID() const { return m_pCollection->GetID(); }

            // Entity access
            //-------------------------------------------------------------------------

            inline TVector<Entity*> const& GetEntities() const { KRG_ASSERT( m_pCollection != nullptr ); return m_pCollection->GetEntities(); }
            Entity* FindEntity( UUID entityID ) const;

            // Loading
            //-------------------------------------------------------------------------
            // Note: Transient maps cannot be loaded/unloaded

            bool IsLoading() const { return m_status == Status::MapDescLoading || m_status == Status::EntitiesLoading; }
            inline bool IsLoaded() const { return m_status == Status::Loaded; }
            inline bool IsUnloaded() const { return m_status == Status::Unloaded; }
            inline bool HasLoadingFailed() const { return m_status == Status::LoadFailed; }

            void Load( LoadingContext const& loadingContext );
            void Unload( LoadingContext const& loadingContext );

            // Updates map loading state, returns true if all loading is complete, false otherwise
            bool UpdateLoading( LoadingContext const& loadingContext );

            // Activation
            //-------------------------------------------------------------------------

            inline bool IsActivated() const { return m_status == Status::Activated; }
            void Activate( LoadingContext const& loadingContext );
            void Deactivate( LoadingContext const& loadingContext );

        private:

            // Entities
            void LoadEntities( LoadingContext const& loadingContext );
            void UnloadEntities( LoadingContext const& loadingContext );

        private:

            TResourcePtr<EntityMapDescriptor>           m_pMapDesc;
            EntityCollection*                           m_pCollection = nullptr;
            TVector<Entity*>                            m_loadingEntities;
            TVector<Entity*>                            m_entitiesToReload;
            Status                                      m_status = Status::Unloaded;
            bool                                        m_unloadRequested = false;
            bool const                                  m_isTransientMap = false; // If this is set, then this is a transient map i.e.created and managed at runtime and not loaded from disk
        };
    }
}