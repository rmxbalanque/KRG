#pragma once

#include "_Module/API.h"
#include "ToolEntityComponent.h"
#include "ToolEntitySystem.h"
#include "Tools/Core/TypeSystem/DynamicTypeInstance.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Serialization { class ToolEntityCollectionReader; class ToolEntityCollectionWriter; }

    //-------------------------------------------------------------------------

    class KRG_TOOLS_ENTITY_API ToolEntity
    {
        friend Serialization::ToolEntityCollectionReader;
        friend Serialization::ToolEntityCollectionWriter;

    public:

        ToolEntity() = default;
        ToolEntity( TypeSystem::TypeRegistry const& typeRegistry, TypeSystem::TypeInfo const* pTypeInfo, UUID const& ID = UUID::GenerateID(), StringID name = StringID( "Entity" ) );
        ToolEntity( TypeSystem::TypeRegistry const& typeRegistry, UUID const& ID = UUID::GenerateID(), StringID name = StringID( "Entity" ) );

        inline bool IsValid() const 
        { 
            if ( !m_typeInstance.IsValid() )
            {
                return false;
            }

            if ( !m_ID.IsValid() || !m_name.IsValid() )
            {
                return false;
            }

            if ( m_isSpatialEntity && !m_rootComponentID.IsValid() )
            {
                return false;
            }
            
            return true;
        }

        inline UUID GetID() const
        {
            return m_ID;
        }

        inline TypeSystem::TypeInfo const* GetTypeInfo() const
        {
            return m_typeInstance.GetTypeInfo();
        }

        inline TypeSystem::TypeID const& GetTypeID() const
        {
            return m_typeInstance.GetTypeID();
        }

        inline StringID const& GetName() const
        {
            return m_name;
        }

        inline char const* GetNameAsCStr() const
        {
            return m_name.c_str();
        }

        // Spatial Entity Interface
        //-------------------------------------------------------------------------

        inline bool IsSpatialEntity() const 
        {
            return m_isSpatialEntity;
        }

        inline StringID GetAttachmentSocketID() const
        {
            return m_attachmentSocketID;
        }

        inline void SetAttachmentSocketID( StringID socketID )
        {
            m_attachmentSocketID = socketID;
        }

        inline void ClearAttachmentSocketID()
        {
            m_attachmentSocketID = StringID();
        }

        // Component Interface
        //-------------------------------------------------------------------------

        U32 GetNumComponents() const
        {
            return (U32) m_components.size();
        }

        TVector<ToolEntityComponent> const& GetComponents() const
        {
            return m_components;
        }

        TVector<ToolEntityComponent>& GetComponents()
        {
            return m_components;
        }

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

            for ( auto& component : m_components )
            {
                if ( component.GetID() == componentID )
                {
                    return &component;
                }
            }

            return nullptr;
        }

        inline ToolEntityComponent* FindMutableComponent( UUID const& componentID )
        {
            KRG_ASSERT( IsValid() );
            return const_cast<ToolEntityComponent*>( FindComponent( componentID ) ); 
        }

        // Use this function to add an existing component to this entity
        void AddComponent( ToolEntityComponent const& component );

        // Use this function to create a new component to this entity
        void CreateComponent( TypeSystem::TypeID componentTypeID, StringID const& name = StringID() );

        // Systems Interface
        //-------------------------------------------------------------------------

        U32 GetNumSystems() const
        {
            KRG_ASSERT( IsValid() );
            return (U32) m_systems.size();
        }

        inline TVector<ToolEntitySystem> const& GetSystems() const
        {
            KRG_ASSERT( IsValid() );
            return m_systems;
        }

        void AddSystem( ToolEntitySystem const& system );

        // Child Entities
        //-------------------------------------------------------------------------

        TVector<ToolEntity>& GetChildEntities() { return m_childEntities; }
        TVector<ToolEntity> const& GetChildEntities() const { return m_childEntities; }
        ToolEntity& CreateChildEntity( UUID const& ID = UUID::GenerateID(), StringID name = StringID( "Entity" ) );
        void DestroyChildEntity( S32 childEntityIdx );

    private:

        TypeSystem::TypeRegistry const*                             m_pTypeRegistry = nullptr;
        DynamicTypeInstance                                         m_typeInstance;
        UUID                                                        m_ID;
        StringID                                                    m_name;
        StringID                                                    m_attachmentSocketID;
        UUID                                                        m_rootComponentID;
        TVector<ToolEntityComponent>                                m_components;
        TVector<ToolEntitySystem>                                   m_systems;
        TVector<ToolEntity>                                         m_childEntities;
        bool                                                        m_isSpatialEntity = false;
    };
}