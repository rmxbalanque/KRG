#pragma once
#include "System/Entity/_Module/API.h"
#include "System/TypeSystem/PropertyInfo.h"
#include "System/TypeSystem/CoreTypeIDs.h"
#include "System/TypeSystem/PropertyPath.h"
#include "System/TypeSystem/TypeSerialization.h"
#include "System/Core/Types/UUID.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class Entity;
    class EntityComponent;

    //-------------------------------------------------------------------------

    namespace Serialization
    {
        struct KRG_SYSTEM_ENTITY_API SerializedEntityComponent
        {
            KRG_SERIALIZE_MEMBERS( m_typeID, m_ID, m_name, m_propertyValues );

        public:

            inline bool IsValid() const { return m_typeID.IsValid() && m_ID.IsValid() && m_name.IsValid(); }

        public:

            TypeSystem::TypeID                                          m_typeID;
            UUID                                                        m_ID;
            StringID                                                    m_name;
            TVector<SerializedPropertyValue>                            m_propertyValues;
        };

        //-------------------------------------------------------------------------

        struct KRG_SYSTEM_ENTITY_API SerializedSpatialEntityComponent : public SerializedEntityComponent
        {
            KRG_SERIALIZE_MEMBERS( KRG_SERIALIZE_BASE( SerializedEntityComponent ), m_spatialParentID, m_attachmentSocketID );

        public:

            inline bool IsRootComponent() const { return !m_spatialParentID.IsValid(); }
            inline bool HasSpatialParent() const { return m_spatialParentID.IsValid(); }

        public:

            UUID                                                        m_spatialParentID;
            StringID                                                    m_attachmentSocketID;
        };

        //-------------------------------------------------------------------------

        struct KRG_SYSTEM_ENTITY_API SerializedEntitySystem
        {
            KRG_SERIALIZE_MEMBERS( m_typeID );

        public:

            inline bool IsValid() const { return m_typeID.IsValid(); }

        public:

            TypeSystem::TypeID                                          m_typeID;
        };

        //-------------------------------------------------------------------------

        struct KRG_SYSTEM_ENTITY_API SerializedEntity
        {
            KRG_SERIALIZE_MEMBERS( m_ID, m_name, m_spatialParentID, m_attachmentSocketID, m_components, m_spatialComponents, m_systems );

        public:

            inline bool IsValid() const { return m_ID.IsValid() && m_name.IsValid(); }
            inline bool IsSpatialEntity() const { return m_spatialComponents.size() > 0; }
            inline bool HasSpatialParent() const { return m_spatialParentID.IsValid(); }

            inline SerializedEntityComponent const* FindComponent( UUID const& componentID ) const
            {
                KRG_ASSERT( componentID.IsValid() );

                for ( auto& component : m_components )
                {
                    if ( component.m_ID == componentID )
                    {
                        return &component;
                    }
                }

                return nullptr;
            }

            inline SerializedSpatialEntityComponent const* FindSpatialComponent( UUID const& componentID ) const
            {
                KRG_ASSERT( componentID.IsValid() );

                for ( auto& component : m_spatialComponents )
                {
                    if ( component.m_ID == componentID )
                    {
                        return &component;
                    }
                }

                return nullptr;
            }

            inline SerializedEntityComponent* FindMutableComponent( UUID const& componentID ) { return const_cast<SerializedEntityComponent*>( FindComponent( componentID ) ); }
            inline SerializedSpatialEntityComponent* FindMutableSpatialComponent( UUID const& componentID ) { return const_cast<SerializedSpatialEntityComponent*>( FindSpatialComponent( componentID ) ); }

        public:

            UUID                                                        m_ID;
            StringID                                                    m_name;
            UUID                                                        m_spatialParentID;
            StringID                                                    m_attachmentSocketID;
            TVector<SerializedEntityComponent>                          m_components;
            TVector<SerializedSpatialEntityComponent>                   m_spatialComponents;
            TVector<SerializedEntitySystem>                             m_systems;
        };
    }
}