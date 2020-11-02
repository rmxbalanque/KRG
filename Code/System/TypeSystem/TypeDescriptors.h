#pragma once
#include "_Module/API.h"
#include "System/TypeSystem/PropertyPath.h"
#include "System/TypeSystem/CoreTypeValidation.h"
#include "System/TypeSystem/TypeValueConverter.h"
#include "System/Core/Types/UUID.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    struct KRG_SYSTEM_TYPESYSTEM_API PropertyDescriptor
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

        PropertyDescriptor( TypeSystem::PropertyPath const& path, char const* pStringValue )
            : m_path( path )
            , m_stringValue( pStringValue )
        {
            KRG_ASSERT( m_path.IsValid() && pStringValue != nullptr );
        }

        PropertyDescriptor( TypeSystem::PropertyPath const& path, String const& stringValue )
            : m_path( path )
            , m_stringValue( stringValue )
        {
            KRG_ASSERT( m_path.IsValid() && !stringValue.empty() );
        }

        PropertyDescriptor( TypeSystem::PropertyPath const& path, TypeSystem::TypeID typeID, String const& stringValue )
            : m_path( path )
            , m_stringValue( stringValue )
        {
            KRG_ASSERT( m_path.IsValid() && !stringValue.empty() );
            TypeSystem::TypeValueConverter::ConvertStringToByteArray( typeID, stringValue, m_byteValue );
        }

        PropertyDescriptor( TypeSystem::CoreTypes coreType, TypeSystem::PropertyPath const& path, String const& stringValue )
            : m_path( path )
            , m_stringValue( stringValue )
        {
            KRG_ASSERT( m_path.IsValid() && !stringValue.empty() );
            TypeSystem::TypeValueConverter::ConvertStringToByteArray( TypeSystem::GetCoreTypeID( coreType ), stringValue, m_byteValue );
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

    struct KRG_SYSTEM_TYPESYSTEM_API TypeDescriptor
    {
        KRG_SERIALIZE_MEMBERS(  m_typeID, m_propertyValues );

    public:

        inline bool IsValid() const { return m_typeID.IsValid(); }

        // Properties
        void RemovePropertyValue( TypeSystem::PropertyPath const& path );

    public:

        TypeSystem::TypeID                                          m_typeID;
        TInlineVector<PropertyDescriptor, 6>                        m_propertyValues;
    };
}