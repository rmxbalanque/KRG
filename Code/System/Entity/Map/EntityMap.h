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
        class EntityCollectionTemplate;

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
                Unloaded = 0,
                Loading,
                Loaded,
                UnloadRequested,
            };

        public:

            EntityMap( ResourceID mapResourceID );
            EntityMap( EntityMap const& map );
            EntityMap( EntityMap&& map );
            ~EntityMap();

            EntityMap& operator=( EntityMap const& map );

            inline ResourceID const& GetMapResourceID() const { return m_pMapDesc.GetResourceID(); }

            inline TVector<Entity*> const& GetEntities() const { KRG_ASSERT( IsLoaded() ); return m_pCollection->GetEntities(); }
            Entity* FindEntity( UUID entityID ) const;

            // Map
            void Load( LoadingContext const& loadingContext );
            void Unload( LoadingContext const& loadingContext );
            bool UpdateLoading( LoadingContext const& loadingContext );

            bool IsLoading() const;
            inline bool IsLoaded() const { return m_pMapDesc.IsLoaded(); }
            inline bool IsUnloading() const { return m_status == Status::UnloadRequested; }
            inline bool IsUnloaded() const { return m_pMapDesc.IsUnloaded(); }
            inline bool HasLoadingFailed() const { return m_pMapDesc.HasLoadingFailed(); }

            // Entities
            void LoadEntities( LoadingContext const& loadingContext );
            void UnloadEntities( LoadingContext const& loadingContext );

        private:

            TResourcePtr<EntityMapDescriptor>           m_pMapDesc;
            EntityCollection*                           m_pCollection = nullptr;
            Status                                      m_status = Status::Unloaded;
        };
    }
}