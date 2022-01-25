#pragma once
#include "Engine/Core/_Module/API.h"
#include "EntityIDs.h"
#include "System/Resource/IResource.h"
#include "System/TypeSystem/TypeDescriptors.h"

namespace KRG
{
    namespace TypeSystem { class TypeRegistry; }
    class Entity;
    class TaskSystem;
}

//-------------------------------------------------------------------------
// Serialized Entity Descriptors
//-------------------------------------------------------------------------
// A custom format for serialized entities
// Very similar to the type descriptor format in the type-system

namespace KRG::EntityModel
{
    struct KRG_ENGINE_CORE_API ComponentDescriptor : public TypeSystem::TypeDescriptor
    {
        KRG_SERIALIZE_MEMBERS( KRG_SERIALIZE_BASE( TypeSystem::TypeDescriptor ), m_spatialParentName, m_attachmentSocketID, m_name, m_isSpatialComponent );

    public:

        inline bool IsValid() const { return TypeSystem::TypeDescriptor::IsValid() && m_name.IsValid(); }

        // Spatial Components
        inline bool IsSpatialComponent() const { return m_isSpatialComponent; }
        inline bool IsRootComponent() const { KRG_ASSERT( m_isSpatialComponent ); return !m_spatialParentName.IsValid(); }
        inline bool HasSpatialParent() const { KRG_ASSERT( m_isSpatialComponent ); return m_spatialParentName.IsValid(); }

    public:

        StringID                                                    m_name;
        StringID                                                    m_spatialParentName;
        StringID                                                    m_attachmentSocketID;
        bool                                                        m_isSpatialComponent = false;
    };

    //-------------------------------------------------------------------------

    struct KRG_ENGINE_CORE_API SystemDescriptor
    {
        KRG_SERIALIZE_MEMBERS( m_typeID );

    public:

        inline bool IsValid() const { return m_typeID.IsValid(); }

    public:

        TypeSystem::TypeID                                          m_typeID;
    };

    //-------------------------------------------------------------------------

    struct KRG_ENGINE_CORE_API EntityDescriptor
    {
        KRG_SERIALIZE_MEMBERS( m_spatialParentName, m_name, m_attachmentSocketID, m_systems, m_components, m_numSpatialComponents );

    public:

        inline bool IsValid() const { return m_name.IsValid(); }
        inline bool IsSpatialEntity() const { return m_numSpatialComponents > 0; }
        inline bool HasSpatialParent() const { return m_spatialParentName.IsValid(); }

        int32 FindComponentIndex( StringID const& componentName ) const;

        inline ComponentDescriptor const* FindComponent( StringID const& componentName ) const
        {
            int32 const componentIdx = FindComponentIndex( componentName );
            return ( componentIdx != InvalidIndex ) ? &m_components[componentIdx] : nullptr;
        }

    public:

        StringID                                                    m_name;
        StringID                                                    m_spatialParentName;
        StringID                                                    m_attachmentSocketID;
        TInlineVector<SystemDescriptor, 5>                           m_systems;
        TVector<ComponentDescriptor>                                m_components; // Ordered list of components: spatial components are first, followed by regular components
        int32                                                       m_numSpatialComponents = 0;
    };
}

//-------------------------------------------------------------------------
// Entity Collection Template
//-------------------------------------------------------------------------
// This is a read-only resource that contains a collection of serialized entity descriptors
// We used this to instantiate a collection of entities
//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    class KRG_ENGINE_CORE_API EntityCollectionDescriptor : public Resource::IResource
    {
        KRG_REGISTER_RESOURCE( 'EC', "Entity Collection" );
        KRG_SERIALIZE_MEMBERS( m_entityDescriptors, m_entityLookupMap, m_entitySpatialAttachmentInfo );

        friend class EntityCollectionLoader;

    public:

        struct SearchResult
        {
            EntityDescriptor*               m_pEntity = nullptr;
            ComponentDescriptor*            m_pComponent = nullptr;
        };

    protected:

        struct SpatialAttachmentInfo
        {
            KRG_SERIALIZE_MEMBERS( m_entityIdx, m_parentEntityIdx );

            int32                           m_entityIdx = InvalidIndex;
            int32                           m_parentEntityIdx = InvalidIndex;
        };

    public:

        virtual bool IsValid() const override
        {
            if ( m_entityDescriptors.empty() )
            {
                return true;
            }

            return m_entityDescriptors.size() == m_entityLookupMap.size();
        }

        TVector<Entity*> InstantiateCollection( TaskSystem* pTaskSystem, TypeSystem::TypeRegistry const& typeRegistry ) const;

        // Template Creation
        //-------------------------------------------------------------------------

        void Reserve( int32 numEntities );

        inline void AddEntity( EntityDescriptor const& entityDesc )
        {
            KRG_ASSERT( entityDesc.IsValid() );
            m_entityLookupMap.insert( TPair<StringID, int32>( entityDesc.m_name, (int32) m_entityDescriptors.size() ) );
            m_entityDescriptors.emplace_back( entityDesc );
        }

        void GenerateSpatialAttachmentInfo();

        void Clear() { m_entityDescriptors.clear(); m_entityLookupMap.clear(); m_entitySpatialAttachmentInfo.clear(); }

        // Entity Access
        //-------------------------------------------------------------------------

        inline int32 GetNumEntityDescriptors() const
        {
            return (int32) m_entityDescriptors.size();
        }

        inline TVector<EntityDescriptor> const& GetEntityDescriptors() const
        {
            return m_entityDescriptors;
        }

        inline TVector<SpatialAttachmentInfo> const& GetEntitySpatialAttachmentInfo() const
        {
            return m_entitySpatialAttachmentInfo;
        }

        inline EntityDescriptor const* FindEntityDescriptor( StringID const& entityName ) const
        {
            KRG_ASSERT( entityName.IsValid() );

            auto const foundEntityIter = m_entityLookupMap.find( entityName );
            if ( foundEntityIter != m_entityLookupMap.end() )
            {
                return &m_entityDescriptors[foundEntityIter->second];
            }
            else
            {
                return nullptr;
            }
        }

        inline int32 FindEntityIndex( StringID const& entityName ) const
        {
            KRG_ASSERT( entityName.IsValid() );

            auto const foundEntityIter = m_entityLookupMap.find( entityName );
            if ( foundEntityIter != m_entityLookupMap.end() )
            {
                return foundEntityIter->second;
            }
            else
            {
                return InvalidIndex;
            }
        }

        // Component Access
        //-------------------------------------------------------------------------

        TVector<SearchResult> GetComponentsOfType( TypeSystem::TypeRegistry const& typeRegistry, TypeSystem::TypeID typeID, bool allowDerivedTypes = true );

        inline TVector<SearchResult> GetComponentsOfType( TypeSystem::TypeRegistry const& typeRegistry, TypeSystem::TypeID typeID, bool allowDerivedTypes = true ) const
        {
            return const_cast<EntityCollectionDescriptor*>( this )->GetComponentsOfType( typeRegistry, typeID, allowDerivedTypes );
        }

        template<typename T>
        inline TVector<SearchResult> GetComponentsOfType( TypeSystem::TypeRegistry const& typeRegistry, bool allowDerivedTypes = true )
        {
            return GetComponentsOfType( typeRegistry, T::GetStaticTypeID(), allowDerivedTypes );
        }

        template<typename T>
        inline TVector<SearchResult> GetComponentsOfType( TypeSystem::TypeRegistry const& typeRegistry, bool allowDerivedTypes = true ) const
        {
            return const_cast<EntityCollectionDescriptor*>( this )->GetComponentsOfType( typeRegistry, T::GetStaticTypeID(), allowDerivedTypes );
        }

        bool HasComponentsOfType( TypeSystem::TypeRegistry const& typeRegistry, TypeSystem::TypeID typeID, bool allowDerivedTypes = true ) const { return !const_cast<EntityCollectionDescriptor*>( this )->GetComponentsOfType( typeRegistry, typeID, allowDerivedTypes ).empty(); }

        template<typename T>
        inline bool HasComponentsOfType( TypeSystem::TypeRegistry const& typeRegistry, bool allowDerivedTypes = true ) const
        {
            return HasComponentsOfType( typeRegistry, T::GetStaticTypeID(), allowDerivedTypes );
        }

    protected:

        TVector<EntityDescriptor>                                   m_entityDescriptors;
        THashMap<StringID, int32>                                   m_entityLookupMap;
        TVector<SpatialAttachmentInfo>                              m_entitySpatialAttachmentInfo;
    };
}

//-------------------------------------------------------------------------
// A compiled entity map template
//-------------------------------------------------------------------------
// This is a read-only resource that contains the serialized entities for a given map
// This is not directly used in the game, instead we create an entity map instance from this map
//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    struct EntityLoadingContext;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_CORE_API EntityMapDescriptor final : public EntityCollectionDescriptor
    {
        KRG_REGISTER_RESOURCE( 'MAP', "Map" );
        KRG_SERIALIZE_MEMBERS( KRG_SERIALIZE_BASE( EntityCollectionDescriptor ) );

        friend class EntityCollectionCompiler;
        friend class EntityCollectionLoader;
    };
}