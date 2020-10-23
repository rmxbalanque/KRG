#pragma once
#include "EntityDescriptors.h"

//-------------------------------------------------------------------------

namespace KRG { class Entity; }
namespace KRG::TypeSystem { class TypeRegistry; }

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    struct LoadingContext;
    class EntityCollectionTemplate;

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
        EntityCollection( TypeSystem::TypeRegistry const& typeRegistry, UUID ID, EntityCollectionTemplate const& entityCollectionTemplate );
        
        ~EntityCollection();

        inline UUID GetID() const { return m_ID; }

        // Entity Access
        //-------------------------------------------------------------------------

        TVector<Entity*> const& GetEntities() const { return m_entities; }

        inline Entity* FindEntity( UUID entityID ) const
        {
            auto iter = m_entityLookupMap.find( entityID );
            return ( iter != m_entityLookupMap.end() ) ? iter->second : nullptr;
        }

        inline bool ContainsEntity( UUID entityID ) const { return FindEntity( entityID ) != nullptr; }

        // Entity Lifetime
        //-------------------------------------------------------------------------

        Entity* CreateEntity( TypeSystem::TypeRegistry const& typeRegistry, EntityDescriptor const& entityDesc );
        void DestroyEntity( UUID entityID );

    private:

        UUID                                                        m_ID;
        TVector<Entity*>                                            m_entities;
        THashMap<UUID, Entity*>                                     m_entityLookupMap;
    };
}