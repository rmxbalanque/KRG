#pragma once
#include "System/Entity/_Module/API.h"
#include "System/TypeSystem/PropertyPath.h"
#include "System/Core/Types/UUID.h"
#include "System/TypeSystem/CoreTypeValidation.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    struct KRG_SYSTEM_ENTITY_API PropertyDescriptor
    {
        KRG_SERIALIZE_MEMBERS( m_path, m_byteValue );

        PropertyDescriptor() = default;

        // Default byte value ctor
        //-------------------------------------------------------------------------

        PropertyDescriptor( TypeSystem::PropertyPath const& path, TVector<Byte> const& byteValue )
            : m_path( path )
            , m_byteValue( byteValue )
        {
            KRG_ASSERT( m_path.IsValid() && byteValue.size() > 0 );
        }

        // Tools only - string value ctor
        //-------------------------------------------------------------------------

        PropertyDescriptor( TypeSystem::PropertyPath const& path, String const& stringValue )
            : m_path( path )
            , m_stringValue( stringValue )
        {
            KRG_ASSERT( m_path.IsValid() && !stringValue.empty() );
        }

        PropertyDescriptor( TypeSystem::PropertyPath const& path, char const* pStringValue )
            : m_path( path )
            , m_stringValue( pStringValue )
        {
            KRG_ASSERT( m_path.IsValid() && pStringValue != nullptr );
        }

        // Enum property descriptor ctors
        //-------------------------------------------------------------------------

        PropertyDescriptor( TypeSystem::PropertyPath const& path, StringID ID )
            : m_path( path )
        {
            KRG_ASSERT( m_path.IsValid() && ID.IsValid() );
            SetEnumValueID( ID );
        }

        // Enum values are serialized as a raw StringID
        void SetEnumValueID( StringID enumValueID );
        StringID GetEnumValueID() const;

        // Set byte value
        template<typename T>
        void SetByteValue( T const& value )
        {
            KRG_ASSERT( TypeSystem::IsCoreType<T>() );
            TypeSystem::TypeValueConverter::ValueToByteArray( value, m_byteValue );
        }

    public:

        TypeSystem::PropertyPath                                    m_path;
        TVector<Byte>                                               m_byteValue;

        // Not-serialized - used in tooling
        String                                                      m_stringValue;
    };

    //-------------------------------------------------------------------------

    struct KRG_SYSTEM_ENTITY_API EntityComponentDescriptor
    {
        KRG_SERIALIZE_MEMBERS( m_ID, m_spatialParentID, m_typeID, m_attachmentSocketID, m_name, m_isSpatialComponent, m_propertyValues );

    public:

        inline bool IsValid() const { return m_typeID.IsValid() && m_ID.IsValid() && m_name.IsValid(); }

        // Spatial Components
        inline bool IsSpatialComponent() const { return m_isSpatialComponent; }
        inline bool IsRootComponent() const { KRG_ASSERT( m_isSpatialComponent ); return !m_spatialParentID.IsValid(); }
        inline bool HasSpatialParent() const { KRG_ASSERT( m_isSpatialComponent ); return m_spatialParentID.IsValid(); }

    public:

        UUID                                                        m_ID;
        UUID                                                        m_spatialParentID;
        TypeSystem::TypeID                                          m_typeID;
        StringID                                                    m_attachmentSocketID;
        StringID                                                    m_name;
        bool                                                        m_isSpatialComponent = false;
        TInlineVector<PropertyDescriptor, 6>                        m_propertyValues;
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

        S32 FindComponentIndex( UUID const& componentID ) const;

        inline EntityComponentDescriptor const* FindComponent( UUID const& componentID ) const 
        {
            S32 const componentIdx = FindComponentIndex( componentID );
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
        S32                                                         m_numSpatialComponents = 0;
    };
}