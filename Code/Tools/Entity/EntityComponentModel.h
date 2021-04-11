#pragma once

#include "_Module/API.h"
#include "Tools/Core/TypeSystem/TypeInstanceModel.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/Core/Math/Transform.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    struct EntityComponentDescriptor;

    //-------------------------------------------------------------------------

    class KRG_TOOLS_ENTITY_API EntityComponentModel
    {
        friend class EntityInstanceModel;
        friend class EntityCollectionModelConverter;

    public:

        EntityComponentModel() = default;
        EntityComponentModel( TypeSystem::TypeInstanceModel const& typeInstance, UUID const& ID = UUID::GenerateID(), StringID const& name = StringID() );
        EntityComponentModel( TypeSystem::TypeRegistry const& typeRegistry, TypeSystem::TypeInfo const* pTypeInfo, UUID const& ID = UUID::GenerateID(), StringID const& name = StringID() );
        EntityComponentModel( TypeSystem::TypeRegistry const& typeRegistry, TypeSystem::TypeInfo const* pTypeInfo, EntityComponentDescriptor const& componentDesc );
        ~EntityComponentModel();

        //-------------------------------------------------------------------------

        inline bool IsValid() const { return m_typeInstance.IsValid() && m_ID.IsValid() && m_name.IsValid(); }
        inline bool IsSpatialComponent() const { return m_isSpatialComponent; }

        inline UUID GetID() const  { return m_ID; }
        inline StringID const& GetName() const { return m_name; }
        inline char const* GetNameAsCStr() const { return m_name.c_str(); }
        inline bool IsOwnedByEntity() const { return m_parentEntityID.IsValid(); }

        inline TypeSystem::TypeInfo const* GetTypeInfo() const { return m_typeInstance.GetTypeInfo(); }
        inline TypeSystem::TypeID const& GetTypeID() const { return m_typeInstance.GetTypeID(); }
        inline TypeSystem::TypeInstanceModel const& GetTypeInstance() const { return m_typeInstance; }
        inline TypeSystem::TypeInstanceModel& GetTypeInstance() { return m_typeInstance; }
        inline bool IsTypeDerivedFrom( TypeSystem::TypeID parentComponentTypeID ) const { return m_typeInstance.GetTypeInfo()->IsDerivedFrom( parentComponentTypeID ); }

        // Spatial Components
        //-------------------------------------------------------------------------

        inline StringID GetAttachmentSocketID() const { return m_attachmentSocketID; }
        inline void SetAttachmentSocketID( StringID const& socketID ) { m_attachmentSocketID = socketID; }

        inline Transform const& GetLocalTransform() const
        {
            KRG_ASSERT( m_isSpatialComponent );
            return m_transform;
        }

        inline void SetLocalTransform( Transform const& transform )
        {
            KRG_ASSERT( m_isSpatialComponent );
            SetTransformPropertyValue( transform );

            Transform const& parentWorldTransform = m_worldTransform * m_transform.GetInverse();
            m_transform = transform;
            UpdateWorldTransform( parentWorldTransform );
        }

        inline Transform const& GetWorldTransform() const
        {
            KRG_ASSERT( m_isSpatialComponent );
            return m_worldTransform;
        }

        Transform const& UpdateWorldTransform( Transform const& parentTransform )
        {
            KRG_ASSERT( m_isSpatialComponent );
            m_worldTransform = m_transform * parentTransform;

            for ( auto pChildComponent : m_childComponents )
            {
                pChildComponent->UpdateWorldTransform( m_worldTransform );
            }

            return m_worldTransform;
        }

        // Properties
        //-------------------------------------------------------------------------

        inline TypeSystem::PropertyInstanceModel* GetProperty( StringID propertyID ) { return m_typeInstance.GetProperty( propertyID ); }
        inline TypeSystem::PropertyInstanceModel* GetProperty( TypeSystem::PropertyPath const& path, bool allowDynamicArrayElementCreation = false ) { return m_typeInstance.GetProperty( path, allowDynamicArrayElementCreation ); }
        inline TypeSystem::PropertyInstanceModel const* GetProperty( StringID propertyID ) const { return m_typeInstance.GetProperty( propertyID ); }
        inline TypeSystem::PropertyInstanceModel const* GetProperty( TypeSystem::PropertyPath const& path ) const { return m_typeInstance.GetProperty( path ); }
        TVector<TypeSystem::PropertyInstanceModel*> const& GetProperties() const { return m_typeInstance.GetProperties(); }

        // Child components
        //-------------------------------------------------------------------------

        TVector<EntityComponentModel*>& GetChildComponents() { return m_childComponents; }
        TVector<EntityComponentModel*> const& GetChildComponents() const { return m_childComponents; }
        
        void GetAllChildComponentsOfType( TypeSystem::TypeID componentTypeID, bool allowDerivedTypes, TVector<EntityComponentModel const*>& outFoundComponents ) const;
        
        EntityComponentModel* CreateChildComponent( TypeSystem::TypeInstanceModel const& typeInstance, UUID const& ID = UUID::GenerateID(), StringID const& name = StringID() );
        EntityComponentModel* CreateChildComponent( TypeSystem::TypeRegistry const& typeRegistry, TypeSystem::TypeInfo const* pTypeInfo, UUID const& ID = UUID::GenerateID(), StringID const& name = StringID() );
        
        void AddChildComponent( EntityComponentModel* pComponent);
        EntityComponentModel* RemoveChildComponent( int32 childComponentIdx );
        void DestroyChildComponent( int32 childComponentIdx );

    private:

        void SetParentEntityID( UUID parentEntityID );
        void ClearParentEntityID();

        Transform GetTransformPropertyValue() const;
        void SetTransformPropertyValue( Transform const& transform );

    private:

        TypeSystem::TypeInstanceModel                        m_typeInstance;
        UUID                                                m_ID;
        UUID                                                m_parentEntityID;
        StringID                                            m_name;
        StringID                                            m_attachmentSocketID;
        Transform                                           m_transform = Transform::Identity;
        Transform                                           m_worldTransform = Transform::Identity;
        TVector<EntityComponentModel*>                       m_childComponents;
        bool                                                m_isSpatialComponent;
    };
}