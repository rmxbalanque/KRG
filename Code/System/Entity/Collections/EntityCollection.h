#pragma once
#include "EntityDescriptors.h"

//-------------------------------------------------------------------------

namespace KRG { class Entity; }
namespace KRG::TypeSystem { class TypeRegistry; }

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    struct LoadingContext;
    class EntityCollectionDescriptor;

    //-------------------------------------------------------------------------
    // An instance of an entity collection
    //-------------------------------------------------------------------------
    // This is runtime and mutable version of a entity collection
    //
    //  * Owns the memory for all entities
    //-------------------------------------------------------------------------

    class KRG_SYSTEM_ENTITY_API EntityCollection
    {

    public:

        EntityCollection() : m_ID( UUID::GenerateID() ) {}
        EntityCollection( TypeSystem::TypeRegistry const& typeRegistry, UUID ID, EntityCollectionDescriptor const& entityCollectionTemplate );
        
        ~EntityCollection();

        inline UUID GetID() const { return m_ID; }

        // Entity API
        //-------------------------------------------------------------------------

        TVector<Entity*> const& GetEntities() const { return m_entities; }

        inline Entity* FindEntity( UUID entityID ) const
        {
            auto iter = m_entityLookupMap.find( entityID );
            return ( iter != m_entityLookupMap.end() ) ? iter->second : nullptr;
        }

        inline bool ContainsEntity( UUID entityID ) const { return FindEntity( entityID ) != nullptr; }

        // Add a newly created entity to this collection
        void AddEntity( Entity* pEntity );

        // Remove an entity from this collection
        void RemoveEntity( UUID entityID );

    private:

        Entity* CreateEntityFromDescriptor( TypeSystem::TypeRegistry const& typeRegistry, EntityDescriptor const& entityDesc );

    private:

        UUID                                                        m_ID;
        TVector<Entity*>                                            m_entities;
        THashMap<UUID, Entity*>                                     m_entityLookupMap;
    };
}