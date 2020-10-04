#pragma once

#include "../_Module/API.h"
#include "System/Entity/EntityCollection.h"
#include "System/Entity/Serialization/EntityArchive.h"
#include "System/Resource/ResourcePtr.h"

//-------------------------------------------------------------------------

namespace KRG
{
    //-------------------------------------------------------------------------
    // A compiled entity map
    //-------------------------------------------------------------------------
    // This is a read-only resource that contains the serialized entities for a given map
    // This is not directly used in the game, instead we create an entity map instance from this map
    //-------------------------------------------------------------------------

    class KRG_SYSTEM_ENTITY_API EntityMap : public Resource::IResource
    {
        KRG_REGISTER_RESOURCE( 'MAP' );
        KRG_SERIALIZE_MEMBERS( KRG_NVP( m_ID ), KRG_NVP( m_entityArchive ) );

        friend class EntityMapCompiler;
        friend class EntityMapLoader;

    public:

        inline UUID GetID() const { return m_ID; }
        virtual bool IsValid() const override;

        Serialization::EntityArchive const& GetEntityArchive() const { return m_entityArchive; }

    private:

        UUID                            m_ID;
        Serialization::EntityArchive    m_entityArchive;
    };

    //-------------------------------------------------------------------------
    // An instance of an entity map
    //-------------------------------------------------------------------------
    // This is runtime and mutable version of a serialized entity map
    //-------------------------------------------------------------------------

    class KRG_SYSTEM_ENTITY_API EntityMapInstance
    {
        friend class EntityDebugView;

    public:

        EntityMapInstance( ResourceID mapResourceID );
        ~EntityMapInstance();

        inline ResourceID const& GetResourceID() const { return m_pMap.GetResourceID(); }
        inline TVector<Entity*> const& GetEntities() const { KRG_ASSERT( IsLoaded() ); return m_pCollection->GetEntities(); }

        void Load( EntityLoadingContext const& loadingContext );
        void Unload( EntityLoadingContext const& loadingContext );
        void UpdateLoading( EntityLoadingContext const& loadingContext );

        bool IsLoading() const;
        inline bool IsLoaded() const { return m_pMap.IsLoaded(); }
        inline bool IsUnloaded() const { return m_pMap.IsUnloaded(); }
        inline bool HasLoadingFailed() const { return m_pMap.HasLoadingFailed(); }

    private:

        EntityMapInstance( EntityMapInstance const& ) = delete;
        EntityMapInstance& operator=( EntityMapInstance const& ) = delete;

    private:

        TResourcePtr<EntityMap>         m_pMap;
        EntityCollection*               m_pCollection = nullptr;
    };
}