#pragma once

#include "_Module/API.h"
#include "EntityComponentModel.h"
#include "EntitySystemModel.h"
#include "EntityPropertyInstanceModel.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    class KRG_TOOLS_ENTITY_API EntityInstanceModel
    {
        friend class EntityCollectionModelConverter;

    public:

        EntityInstanceModel() = default;
        EntityInstanceModel( TypeSystem::TypeRegistry const& typeRegistry, TypeSystem::TypeInfo const* pTypeInfo, UUID const& ID = UUID::GenerateID(), StringID name = StringID( "Entity" ) );
        EntityInstanceModel( TypeSystem::TypeRegistry const& typeRegistry, UUID const& ID = UUID::GenerateID(), StringID name = StringID( "Entity" ) );
        ~EntityInstanceModel();

        bool IsValid() const;

        inline UUID GetID() const { return m_ID; }
        inline StringID const& GetName() const { return m_name; }
        inline char const* GetNameAsCStr() const { return m_name.c_str(); }
        inline UUID GetParentEntityID() const { return m_parentEntityID; }
        inline bool IsOwnedByEntity() const { return m_parentEntityID.IsValid(); }

        inline TypeSystem::TypeInfo const* GetTypeInfo() const { return m_typeInstance.GetTypeInfo(); }
        inline TypeSystem::TypeID const& GetTypeID() const { return m_typeInstance.GetTypeID(); }

        // Spatial Entity Interface
        //-------------------------------------------------------------------------

        inline bool IsSpatialEntity() const { return m_isSpatialEntity; }
        inline StringID GetAttachmentSocketID() const { return m_attachmentSocketID; }
        inline void SetAttachmentSocketID( StringID socketID ) { m_attachmentSocketID = socketID; }
        inline void ClearAttachmentSocketID() { m_attachmentSocketID = StringID(); }
        void UpdateWorldTransforms( Transform const& parentTransform = Transform::Identity );

        // Component Interface
        //-------------------------------------------------------------------------

        uint32 GetNumComponents() const { return (uint32) m_components.size(); }
        TVector<EntityComponentModel*> const& GetComponents() const { return m_components; }
        TVector<EntityComponentModel*>& GetComponents() { return m_components; }

        void GetAllComponentsOfType( TypeSystem::TypeID componentTypeID, bool allowDerivedTypes, TVector<EntityComponentModel const*>& outFoundComponents ) const;

        inline EntityComponentModel const* GetRootComponent() const
        {
            KRG_ASSERT( IsValid() && IsSpatialEntity() );
            auto pRootComponent = FindComponent( m_rootComponentID );
            KRG_ASSERT( pRootComponent != nullptr && pRootComponent->IsSpatialComponent() ); // All spatial entities MUST have a root component
            return pRootComponent;
        }

        inline EntityComponentModel const* FindComponent( UUID const& componentID ) const
        {
            KRG_ASSERT( componentID.IsValid() );

            for ( auto& pComponent : m_components )
            {
                if ( pComponent->GetID() == componentID )
                {
                    return pComponent;
                }
            }

            return nullptr;
        }

        inline EntityComponentModel* FindMutableComponent( UUID const& componentID ) { return const_cast<EntityComponentModel*>( FindComponent( componentID ) ); }

        void AddComponent( EntityComponentModel* pComponent );
        void CreateComponent( TypeSystem::TypeID componentTypeID, StringID const& name = StringID() );
        EntityComponentModel* RemoveComponent( int32 componentIdx );
        void DestroyComponent( int32 componentIdx );

        // Systems Interface
        //-------------------------------------------------------------------------

        uint32 GetNumSystems() const { return (uint32) m_systems.size(); }
        inline TVector<EntitySystemModel*> const& GetSystems() const { return m_systems; }
        void AddSystem( EntitySystemModel* pSystem );
        EntitySystemModel* RemoveSystem( int32 systemIdx );
        void DestroySystem( int32 systemIdx );

        // Child Entities
        //-------------------------------------------------------------------------

        TVector<EntityInstanceModel*>& GetChildEntities() { return m_childEntities; }
        TVector<EntityInstanceModel*> const& GetChildEntities() const { return m_childEntities; }
        EntityInstanceModel* CreateChildEntity( UUID const& ID = UUID::GenerateID(), StringID name = StringID( "Entity" ) );
        void AddChildEntity( EntityInstanceModel* pEntity );
        EntityInstanceModel* RemoveChildEntity( int32 childEntityIdx );
        void DestroyChildEntity( int32 childEntityIdx );

    private:

        TypeSystem::TypeRegistry const*                             m_pTypeRegistry = nullptr;
        TypeSystem::TypeInstanceModel                                m_typeInstance;
        UUID                                                        m_ID;
        StringID                                                    m_name;
        StringID                                                    m_attachmentSocketID;
        UUID                                                        m_rootComponentID;
        UUID                                                        m_parentEntityID;
        TVector<EntityComponentModel*>                               m_components;
        TVector<EntitySystemModel*>                                  m_systems;
        TVector<EntityInstanceModel*>                                        m_childEntities;
        bool                                                        m_isSpatialEntity = false;
    };
}