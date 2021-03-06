#pragma once
#include "_Module/API.h"
#include "System/TypeSystem/TypeDescriptors.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    struct KRG_SYSTEM_ENTITY_API EntityComponentDescriptor : public TypeSystem::TypeDescriptor
    {
        KRG_SERIALIZE_MEMBERS( KRG_SERIALIZE_BASE( TypeSystem::TypeDescriptor ), m_ID, m_spatialParentID, m_attachmentSocketID, m_name, m_isSpatialComponent );

    public:

        inline bool IsValid() const { return TypeSystem::TypeDescriptor::IsValid() && m_ID.IsValid() && m_name.IsValid(); }

        // Spatial Components
        inline bool IsSpatialComponent() const { return m_isSpatialComponent; }
        inline bool IsRootComponent() const { KRG_ASSERT( m_isSpatialComponent ); return !m_spatialParentID.IsValid(); }
        inline bool HasSpatialParent() const { KRG_ASSERT( m_isSpatialComponent ); return m_spatialParentID.IsValid(); }

    public:

        UUID                                                        m_ID;
        UUID                                                        m_spatialParentID;
        StringID                                                    m_attachmentSocketID;
        StringID                                                    m_name;
        bool                                                        m_isSpatialComponent = false;
    };

    //-------------------------------------------------------------------------

    struct KRG_SYSTEM_ENTITY_API EntitySystemDescriptor
    {
        KRG_SERIALIZE_MEMBERS( m_typeID );

    public:

        inline bool IsValid() const { return m_typeID.IsValid(); }

    public:

        TypeSystem::TypeID                                          m_typeID;
    };

    //-------------------------------------------------------------------------

    struct KRG_SYSTEM_ENTITY_API EntityDescriptor
    {
        KRG_SERIALIZE_MEMBERS( m_ID, m_spatialParentID, m_name, m_attachmentSocketID, m_systems, m_components, m_numSpatialComponents );

    public:

        inline bool IsValid() const { return m_ID.IsValid() && m_name.IsValid(); }
        inline bool IsSpatialEntity() const { return m_numSpatialComponents > 0; }
        inline bool HasSpatialParent() const { return m_spatialParentID.IsValid(); }

        int32 FindComponentIndex( UUID const& componentID ) const;

        inline EntityComponentDescriptor const* FindComponent( UUID const& componentID ) const 
        {
            int32 const componentIdx = FindComponentIndex( componentID );
            return ( componentIdx != InvalidIndex ) ? &m_components[componentIdx] : nullptr;
        }

        inline EntityComponentDescriptor* FindMutableComponent( UUID const& componentID ) { return const_cast<EntityComponentDescriptor*>( FindComponent( componentID ) ); }

    public:

        UUID                                                        m_ID;
        UUID                                                        m_spatialParentID;
        StringID                                                    m_name;
        StringID                                                    m_attachmentSocketID;
        TInlineVector<EntitySystemDescriptor,5>                     m_systems;
        TVector<EntityComponentDescriptor>                          m_components; // Ordered list of components: spatial components are first, followed by regular components
        int32                                                         m_numSpatialComponents = 0;
    };
}