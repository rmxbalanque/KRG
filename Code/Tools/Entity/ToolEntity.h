#pragma once

#include "_Module/API.h"
#include "ToolEntityComponent.h"
#include "ToolEntitySystem.h"
#include "Tools/Core/TypeSystem/ToolTypeInstance.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    class KRG_TOOLS_ENTITY_API ToolEntity
    {
        friend class ToolEntityCollectionConverter;

    public:

        ToolEntity() = default;
        ToolEntity( TypeSystem::TypeRegistry const& typeRegistry, TypeSystem::TypeInfo const* pTypeInfo, UUID const& ID = UUID::GenerateID(), StringID name = StringID( "Entity" ) );
        ToolEntity( TypeSystem::TypeRegistry const& typeRegistry, UUID const& ID = UUID::GenerateID(), StringID name = StringID( "Entity" ) );
        ~ToolEntity();

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

        // Component Interface
        //-------------------------------------------------------------------------

        U32 GetNumComponents() const { return (U32) m_components.size(); }
        TVector<ToolEntityComponent*> const& GetComponents() const { return m_components; }
        TVector<ToolEntityComponent*>& GetComponents() { return m_components; }

        void GetAllComponentsOfType( TypeSystem::TypeID componentTypeID, bool allowDerivedTypes, TVector<ToolEntityComponent const*>& outFoundComponents ) const;

        inline ToolEntityComponent const* GetRootComponent() const
        {
            KRG_ASSERT( IsValid() && IsSpatialEntity() );
            auto pRootComponent = FindComponent( m_rootComponentID );
            KRG_ASSERT( pRootComponent != nullptr && pRootComponent->IsSpatialComponent() ); // All spatial entities MUST have a root component
            return pRootComponent;
        }

        inline ToolEntityComponent const* FindComponent( UUID const& componentID ) const
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

        inline ToolEntityComponent* FindMutableComponent( UUID const& componentID ) { return const_cast<ToolEntityComponent*>( FindComponent( componentID ) ); }

        void AddComponent( ToolEntityComponent* pComponent );
        void CreateComponent( TypeSystem::TypeID componentTypeID, StringID const& name = StringID() );
        ToolEntityComponent* RemoveComponent( S32 componentIdx );
        void DestroyComponent( S32 componentIdx );

        // Systems Interface
        //-------------------------------------------------------------------------

        U32 GetNumSystems() const { return (U32) m_systems.size(); }
        inline TVector<ToolEntitySystem*> const& GetSystems() const { return m_systems; }
        void AddSystem( ToolEntitySystem* pSystem );
        ToolEntitySystem* RemoveSystem( S32 systemIdx );
        void DestroySystem( S32 systemIdx );

        // Child Entities
        //-------------------------------------------------------------------------

        TVector<ToolEntity*>& GetChildEntities() { return m_childEntities; }
        TVector<ToolEntity*> const& GetChildEntities() const { return m_childEntities; }
        ToolEntity* CreateChildEntity( UUID const& ID = UUID::GenerateID(), StringID name = StringID( "Entity" ) );
        void AddChildEntity( ToolEntity* pEntity );
        ToolEntity* RemoveChildEntity( S32 childEntityIdx );
        void DestroyChildEntity( S32 childEntityIdx );

    private:

        TypeSystem::TypeRegistry const*                             m_pTypeRegistry = nullptr;
        TypeSystem::ToolTypeInstance                                m_typeInstance;
        UUID                                                        m_ID;
        StringID                                                    m_name;
        StringID                                                    m_attachmentSocketID;
        UUID                                                        m_rootComponentID;
        UUID                                                        m_parentEntityID;
        TVector<ToolEntityComponent*>                               m_components;
        TVector<ToolEntitySystem*>                                  m_systems;
        TVector<ToolEntity*>                                        m_childEntities;
        bool                                                        m_isSpatialEntity = false;
    };
}