#pragma once

#include "_Module/API.h"
#include "Tools/Core/TypeSystem/DynamicTypeInstance.h"
#include "System/TypeSystem/TypeRegistry.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Serialization { class ToolEntityCollectionReader; class ToolEntityCollectionWriter; }

    //-------------------------------------------------------------------------

    class KRG_TOOLS_ENTITY_API ToolEntityComponent
    {
        friend class ToolEntity;
        friend class Serialization::ToolEntityCollectionReader;
        friend class Serialization::ToolEntityCollectionWriter;

    public:

        ToolEntityComponent() = default;
        ToolEntityComponent( DynamicTypeInstance const& typeInstance, UUID const& ID = UUID::GenerateID(), StringID const& name = StringID() );
        ToolEntityComponent( TypeSystem::TypeRegistry const& typeRegistry, TypeSystem::TypeInfo const* pTypeInfo, UUID const& ID = UUID::GenerateID(), StringID const& name = StringID() );

        inline bool IsValid() const 
        { 
            return m_typeInstance.IsValid() && m_ID.IsValid() && m_name.IsValid();
        }

        inline bool IsSpatialComponent() const
        {
            return m_isSpatialComponent;
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

        inline DynamicTypeInstance const& GetTypeInstance() const 
        {
            return m_typeInstance;
        }

        inline DynamicTypeInstance& GetTypeInstance()
        {
            return m_typeInstance;
        }

        inline StringID const& GetName() const
        {
            return m_name;
        }

        inline char const* GetNameAsCStr() const
        {
            return m_name.c_str();
        }

        // Spatial Components
        //-------------------------------------------------------------------------

        inline StringID GetAttachmentSocketID() const
        {
            return m_attachmentSocketID;
        }

        inline void SetAttachmentSocketID( StringID const& socketID )
        {
            m_attachmentSocketID = socketID;
        }

        inline Transform const& GetLocalTransform() const
        {
            KRG_ASSERT( m_isSpatialComponent );
            return m_transform;
        }

        inline void SetLocalTransform( Transform const& transform )
        {
            KRG_ASSERT( m_isSpatialComponent );
            KRG_UNIMPLEMENTED_FUNCTION(); // Set property value too
            m_transform = transform;
        }

        inline Transform const& GetWorldTransform() const
        {
            KRG_ASSERT( m_isSpatialComponent );
            return m_transform;
        }

        Transform const& UpdateWorldTransform( Transform const& parentTransform )
        {
            KRG_ASSERT( m_isSpatialComponent );
            KRG_UNIMPLEMENTED_FUNCTION(); // Set property value too
            m_worldTransform = m_transform * parentTransform;
            return m_worldTransform;
        }

        // Properties
        //-------------------------------------------------------------------------

        inline DynamicPropertyInstance* GetProperty( StringID propertyID )
        { 
            return m_typeInstance.GetProperty( propertyID ); 
        }

        inline DynamicPropertyInstance* GetProperty( TypeSystem::PropertyPath const& path ) 
        {
            return m_typeInstance.GetProperty( path );
        }

        inline DynamicPropertyInstance const* GetProperty( StringID propertyID ) const
        {
            return m_typeInstance.GetProperty( propertyID ); 
        }
        inline DynamicPropertyInstance const* GetProperty( TypeSystem::PropertyPath const& path ) const 
        {
            return m_typeInstance.GetProperty( path ); 
        }

        TVector<DynamicPropertyInstance> const& GetProperties() const 
        {
            return m_typeInstance.GetProperties();
        }

        // Child components
        //-------------------------------------------------------------------------

        TVector<ToolEntityComponent>& GetChildComponents() { return m_childComponents; }
        TVector<ToolEntityComponent> const& GetChildComponents() const { return m_childComponents; }
        ToolEntityComponent& CreateChildComponent( DynamicTypeInstance const& typeInstance, UUID const& ID = UUID::GenerateID(), StringID const& name = StringID() );
        ToolEntityComponent& CreateChildComponent( TypeSystem::TypeRegistry const& typeRegistry, TypeSystem::TypeInfo const* pTypeInfo, UUID const& ID = UUID::GenerateID(), StringID const& name = StringID() );
        void DestroyChildComponent( S32 childComponentIdx );

    private:

        DynamicTypeInstance                         m_typeInstance;
        UUID                                        m_ID;
        StringID                                    m_name;
        StringID                                    m_attachmentSocketID;
        Transform                                   m_transform = Transform::Identity;
        Transform                                   m_worldTransform = Transform::Identity;
        TVector<ToolEntityComponent>                m_childComponents;
        bool                                        m_isSpatialComponent;
    };
}