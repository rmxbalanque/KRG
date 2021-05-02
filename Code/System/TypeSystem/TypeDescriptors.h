#pragma once
#include "_Module/API.h"
#include "System/TypeSystem/PropertyPath.h"
#include "System/TypeSystem/CoreTypeIDs.h"
#include "System/TypeSystem/CoreTypeConversions.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    class TypeRegistry;
    struct TypeInfo;

    //-------------------------------------------------------------------------
    // Basic descriptor of a KRG reflected property
    //-------------------------------------------------------------------------

    struct KRG_SYSTEM_TYPESYSTEM_API PropertyDescriptor
    {
        KRG_SERIALIZE_MEMBERS( m_path, m_byteValue );

        PropertyDescriptor() = default;

        PropertyDescriptor( TypeRegistry const& typeRegistry, TypeSystem::PropertyPath const& path, PropertyInfo const& propertyInfo, String const& stringValue )
            : m_path( path )
            , m_stringValue( stringValue )
        {
            KRG_ASSERT( m_path.IsValid() && !stringValue.empty() );
            Conversion::ConvertStringToBinary( typeRegistry, propertyInfo, stringValue, m_byteValue );
        }

        PropertyDescriptor( TypeRegistry const& typeRegistry, TypeSystem::PropertyPath const& path, TypeID propertyTypeID, TypeID propertyTemplatedArgumentTypeID, String const& stringValue )
            : m_path( path )
            , m_stringValue( stringValue )
        {
            KRG_ASSERT( m_path.IsValid() && !stringValue.empty() );
            Conversion::ConvertStringToBinary( typeRegistry, propertyTypeID, propertyTemplatedArgumentTypeID, stringValue, m_byteValue );
        }

        inline bool IsValid() const { return m_path.IsValid() && !m_byteValue.empty(); }

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

    public:

        TypeSystem::PropertyPath                                    m_path;
        TVector<Byte>                                               m_byteValue;

        // Not-serialized - used in tooling
        String                                                      m_stringValue;
    };

    //-------------------------------------------------------------------------
    // Basic Descriptor of a KRG reflected type
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

    //-------------------------------------------------------------------------
    // Type Creator
    //-------------------------------------------------------------------------

    class KRG_SYSTEM_TYPESYSTEM_API TypeCreator
    {
    public:

        template<typename T>
        inline static T* CreateTypeFromDescriptor( TypeRegistry const& typeRegistry, TypeDescriptor const& typeDesc )
        {
            auto pTypeInfo = typeRegistry.GetTypeInfo( typeDesc.m_typeID );
            KRG_ASSERT( pTypeInfo != nullptr );
            KRG_ASSERT( pTypeInfo->IsDerivedFrom<T>() );

            // Create new instance
            void* pTypeInstance = pTypeInfo->m_pTypeHelper->CreateType();
            KRG_ASSERT( pTypeInstance != nullptr );

            // Set properties
            SetPropertyValues( typeRegistry, *pTypeInfo, typeDesc, pTypeInstance );
            return reinterpret_cast<T*>( pTypeInstance );
        }

        template<typename T>
        inline static T* CreateTypeFromDescriptor( TypeRegistry const& typeRegistry, TypeDescriptor const& typeDesc, void* pAllocatedMemoryForInstance )
        {
            auto pTypeInfo = typeRegistry.GetTypeInfo( typeDesc.m_typeID );
            KRG_ASSERT( pTypeInfo != nullptr );
            KRG_ASSERT( pTypeInfo->IsDerivedFrom<T>() );

            // Create new instance
            KRG_ASSERT( pAllocatedMemoryForInstance != nullptr );
            pTypeInfo->m_pTypeHelper->CreateTypeInPlace( pAllocatedMemoryForInstance );

            // Set properties
            SetPropertyValues( typeRegistry, *pTypeInfo, typeDesc, pAllocatedMemoryForInstance );
            return reinterpret_cast<T*>( pAllocatedMemoryForInstance );
        }

    private:

        static void* SetPropertyValues( TypeRegistry const& typeRegistry, TypeInfo const& typeInfo, TypeDescriptor const& typeDesc, void* pTypeInstance );
    };
}