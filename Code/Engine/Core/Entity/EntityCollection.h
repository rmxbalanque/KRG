#pragma once
#include "Engine/Core/Entity/EntityDescriptors.h"

//-------------------------------------------------------------------------

namespace KRG 
{ 
    class Entity; 
    class TaskSystem; 
    namespace TypeSystem { class TypeRegistry; }
}

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    struct EntityLoadingContext;
    class EntityCollectionDescriptor;

    //-------------------------------------------------------------------------
    // An instance of an entity collection
    //-------------------------------------------------------------------------
    // This is runtime and mutable version of a entity collection
    //
    //  * Owns the memory for all entities
    //-------------------------------------------------------------------------

    class KRG_ENGINE_CORE_API EntityCollection
    {
        // Instantiate a single entity from a descriptor
        static Entity* CreateEntityFromDescriptor( TypeSystem::TypeRegistry const& typeRegistry, EntityDescriptor const& entityDesc );

    public:

        EntityCollection() : m_ID( UUID::GenerateID() ) {}
        EntityCollection( TypeSystem::TypeRegistry const& typeRegistry, EntityCollectionID const& ID, EntityCollectionDescriptor const& entityCollectionDesc );
        
        ~EntityCollection();

        inline EntityCollectionID GetID() const { return m_ID; }

        //-------------------------------------------------------------------------

        TVector<Entity*> const& GetEntities() const { return m_entities; }

        inline Entity* FindEntity( EntityID entityID ) const
        {
            auto iter = m_entityIDLookupMap.find( entityID );
            return ( iter != m_entityIDLookupMap.end() ) ? iter->second : nullptr;
        }

        inline bool ContainsEntity( EntityID entityID ) const { return FindEntity( entityID ) != nullptr; }

        // Add a newly created entity to this collection
        void AddEntityToCollection( Entity* pEntity );

        // Remove an entity from this collection
        void RemoveEntityFromCollection( EntityID entityID );

        // Transfers ownership of all entities
        void TransferEntities( TVector<Entity*>& outEntities );

    protected:

        // Creates all entities from a descriptor on a single thread.
        void CreateAllEntitiesSequential( TypeSystem::TypeRegistry const& typeRegistry, EntityCollectionDescriptor const& entityCollectionTemplate );

        // Creates all entities from a descriptor using multiple threads, blocks until complete
        void CreateAllEntitiesParallel( TaskSystem& taskSystem, TypeSystem::TypeRegistry const& typeRegistry, EntityCollectionDescriptor const& entityCollectionTemplate );
        
        // Resolves all inter-entity attachments
        void ResolveEntitySpatialAttachments( EntityCollectionDescriptor const& entityCollectionTemplate );

        // Destroy all created entity instances
        void DestroyAllEntities();

    protected:

        EntityCollectionID                                          m_ID;
        TVector<Entity*>                                            m_entities;
        THashMap<EntityID, Entity*>                                 m_entityIDLookupMap;
    };
}