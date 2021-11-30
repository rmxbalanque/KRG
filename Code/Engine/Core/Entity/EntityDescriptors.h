#pragma once
#include "Engine/Core/_Module/API.h"
#include "EntityIDs.h"
#include "System/TypeSystem/TypeDescriptors.h"

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
        TInlineVector<SystemDescriptor,5>                           m_systems;
        TVector<ComponentDescriptor>                                m_components; // Ordered list of components: spatial components are first, followed by regular components
        int32                                                       m_numSpatialComponents = 0;
    };
}