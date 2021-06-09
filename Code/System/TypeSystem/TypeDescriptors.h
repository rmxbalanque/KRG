#pragma once
#include "_Module/API.h"
#include "TypeRegistrationMacros.h"
#include "PropertyPath.h"
#include "CoreTypeIDs.h"
#include "CoreTypeConversions.h"

//-------------------------------------------------------------------------
// Basic descriptor of a KRG reflected property
//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    class TypeRegistry;
    struct TypeInfo;

    struct KRG_SYSTEM_TYPESYSTEM_API PropertyDescriptor
    {
        KRG_SERIALIZE_MEMBERS( m_path, m_byteValue );

        PropertyDescriptor() = default;

        PropertyDescriptor( TypeRegistry const& typeRegistry, PropertyPath const& path, PropertyInfo const& propertyInfo, String const& stringValue )
            : m_path( path )
        {
            KRG_ASSERT( m_path.IsValid() );
            Conversion::ConvertStringToBinary( typeRegistry, propertyInfo, stringValue, m_byteValue );

            #if KRG_DEVELOPMENT_TOOLS
            m_stringValue = stringValue;
            #endif
        }

        PropertyDescriptor( TypeRegistry const& typeRegistry, PropertyPath const& path, TypeID propertyTypeID, TypeID propertyTemplatedArgumentTypeID, String const& stringValue )
            : m_path( path )
        {
            KRG_ASSERT( m_path.IsValid() && !stringValue.empty() );
            Conversion::ConvertStringToBinary( typeRegistry, propertyTypeID, propertyTemplatedArgumentTypeID, stringValue, m_byteValue );

            #if KRG_DEVELOPMENT_TOOLS
            m_stringValue = stringValue;
            #endif
        }

        inline bool IsValid() const { return m_path.IsValid() && !m_byteValue.empty(); }

        // Tools only constructors
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        PropertyDescriptor( PropertyPath const& path, char const* pStringValue )
            : m_path( path )
            , m_stringValue( pStringValue )
        {
            KRG_ASSERT( m_path.IsValid() && pStringValue != nullptr );
        }

        PropertyDescriptor( PropertyPath const& path, String const& stringValue )
            : m_path( path )
            , m_stringValue( stringValue )
        {
            KRG_ASSERT( m_path.IsValid() && !stringValue.empty() );
        }

        PropertyDescriptor( PropertyPath const& path, String const& value, TypeID typeID = TypeID(), TypeID templatedArgumentTypeID = TypeID() )
            : m_path( path )
            , m_stringValue( value )
            , m_typeID( typeID )
            , m_templatedArgumentTypeID( templatedArgumentTypeID )
        {
            KRG_ASSERT( path.IsValid() );
        }
        #endif

    public:

        PropertyPath                                                m_path;
        TVector<Byte>                                               m_byteValue;

        // Not-serialized - used in tooling
        #if KRG_DEVELOPMENT_TOOLS
        String                                                      m_stringValue;
        TypeID                                                      m_typeID;
        TypeID                                                      m_templatedArgumentTypeID;
        #endif
    };
}

//-------------------------------------------------------------------------
// Type Descriptor
//-------------------------------------------------------------------------
// A serialized description of a KRG type with all property overrides

namespace KRG::TypeSystem
{
    struct KRG_SYSTEM_TYPESYSTEM_API TypeDescriptor
    {
        KRG_SERIALIZE_MEMBERS( m_typeID, m_properties );

    public:

        inline bool IsValid() const { return m_typeID.IsValid(); }

        // Properties
        void RemovePropertyValue( PropertyPath const& path );

    public:

        TypeID                                                      m_typeID;
        TInlineVector<PropertyDescriptor, 6>                        m_properties;
    };
}

//-------------------------------------------------------------------------
// Type Creator
//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    class KRG_SYSTEM_TYPESYSTEM_API TypeCreator
    {
    public:

        template<typename T>
        [[nodiscard]] inline static T* CreateTypeFromDescriptor( TypeRegistry const& typeRegistry, TypeInfo const* pTypeInfo, TypeDescriptor const& typeDesc )
        {
            KRG_ASSERT( pTypeInfo != nullptr && pTypeInfo->m_ID == typeDesc.m_typeID );
            KRG_ASSERT( pTypeInfo->IsDerivedFrom<T>() );

            // Create new instance
            void* pTypeInstance = pTypeInfo->m_pTypeHelper->CreateType();
            KRG_ASSERT( pTypeInstance != nullptr );

            // Set properties
            SetPropertyValues( typeRegistry, *pTypeInfo, typeDesc, pTypeInstance );
            return reinterpret_cast<T*>( pTypeInstance );
        }

        template<typename T>
        [[nodiscard]] inline static T* CreateTypeFromDescriptor( TypeRegistry const& typeRegistry, TypeDescriptor const& typeDesc )
        {
            TypeInfo const* pTypeInfo = typeRegistry.GetTypeInfo( typeDesc.m_typeID );
            return CreateTypeFromDescriptor<T>( typeRegistry, pTypeInfo, typeDesc );
        }

        // Warning! Do not use this function on an existing type instance of type T since it will not call the destructor and so will leak, only use on uninitialized memory
        template<typename T>
        [[nodiscard]] inline static T* CreateTypeFromDescriptor( TypeRegistry const& typeRegistry, TypeInfo const* pTypeInfo, TypeDescriptor const& typeDesc, IRegisteredType* pAllocatedMemoryForInstance )
        {
            KRG_ASSERT( pTypeInfo != nullptr && pTypeInfo->m_ID == typeDesc.m_typeID );
            KRG_ASSERT( pTypeInfo->IsDerivedFrom<T>() );

            // Create new instance
            KRG_ASSERT( pAllocatedMemoryForInstance != nullptr );
            pTypeInfo->m_pTypeHelper->CreateTypeInPlace( pAllocatedMemoryForInstance );

            // Set properties
            SetPropertyValues( typeRegistry, *pTypeInfo, typeDesc, pAllocatedMemoryForInstance );
            return reinterpret_cast<T*>( pAllocatedMemoryForInstance );
        }

        // Warning! Do not use this function on an existing type instance of type T since it will not call the destructor and so will leak, only use on uninitialized memory
        template<typename T>
        [[nodiscard]] inline static T* CreateTypeFromDescriptor( TypeRegistry const& typeRegistry, TypeDescriptor const& typeDesc, void* pAllocatedMemoryForInstance )
        {
            TypeInfo const* pTypeInfo = typeRegistry.GetTypeInfo( typeDesc.m_typeID );
            return CreateTypeFromDescriptor<T>( typeRegistry, pTypeInfo, typeDesc, pAllocatedMemoryForInstance );
        }

        template<typename T>
        inline static void CreateTypeFromDescriptor( TypeRegistry const& typeRegistry, TypeDescriptor const& typeDesc, T* pTypeInstance )
        {
            pTypeInstance->~T();
            TypeInfo const* pTypeInfo = typeRegistry.GetTypeInfo( typeDesc.m_typeID );
            T* pCreatedType = CreateTypeFromDescriptor<T>( typeRegistry, pTypeInfo, typeDesc, pTypeInstance );
        }

    private:

        static void* SetPropertyValues( TypeRegistry const& typeRegistry, TypeInfo const& typeInfo, TypeDescriptor const& typeDesc, void* pTypeInstance );
    };
}

//-------------------------------------------------------------------------
// Type Descriptor Collection
//-------------------------------------------------------------------------
// Generally only useful for when serializing a set of types all derived from the same base type
// This collection can be instantiate in one of two ways
// * Statically - all types are created in a single contiguous array of memory, this is immutable
// * Dynamically - each type is individually allocated, these types can be destroyed individually at runtime

namespace KRG::TypeSystem
{
    struct KRG_SYSTEM_TYPESYSTEM_API TypeDescriptorCollection
    {
        KRG_SERIALIZE_MEMBERS( m_descriptors );

    public:

        template<typename T>
        static void* InstantiateStaticCollection( TypeRegistry const& typeRegistry, TypeDescriptorCollection const& collection, TVector<T*>& outTypes )
        {
            KRG_ASSERT( collection.m_typeSizes.size() == collection.m_descriptors.size() ); // Did you forget to run the calculate requirements function?
            KRG_ASSERT( collection.m_typeSizes.size() == collection.m_typePaddings.size() ); // Did you forget to run the calculate requirements function?

            void* pRawMemory = KRG::Alloc( collection.m_totalRequiredSize, collection.m_requiredAlignment );
            Byte* pTypeMemory = (Byte*) pRawMemory;
            int32 const numDescs = (int32) collection.m_descriptors.size();
            for ( int32 i = 0; i < numDescs; i++ )
            {
                pTypeMemory += collection.m_typePaddings[i];
                outTypes.emplace_back( TypeCreator::CreateTypeFromDescriptor<T>( typeRegistry, collection.m_typeInfos[i], collection.m_descriptors[i], (IRegisteredType*) pTypeMemory ) );
                pTypeMemory += collection.m_typeSizes[i];
            }

            return pRawMemory;
        }

        template<typename T>
        static void DestroyStaticCollection( TVector<T*>& types )
        {
            KRG_ASSERT( !types.empty() );
            void* pMemoryBlock = types[0];
            for ( auto pType : types )
            {
                pType->~T();
            }
            KRG::Free( pMemoryBlock );
        }

        template<typename T>
        static void InstantiateDynamicCollection( TypeRegistry const& typeRegistry, TypeDescriptorCollection const& collection, TVector<T*>& outTypes )
        {
            int32 const numDescs = (int32) collection.m_descriptors.size();
            for ( int32 i = 0; i < numDescs; i++ )
            {
                outTypes.emplace_back( TypeCreator::CreateTypeFromDescriptor<T>( typeRegistry, collection.m_typeInfos[i], collection.m_descriptors[i] ) );
            }
        }

        template<typename T>
        static void DestroyDynamicCollection( TVector<T*>& types )
        {
            for ( auto& pType : types )
            {
                KRG::Delete( type )
            }
            types.clear();
        }

    public:

        void Reset();

        // Calculates all the necessary information needed to instantiate this collection statically (aka in a single immutable block)
        void CalculateCollectionRequirements( TypeRegistry const& typeRegistry );

    public:

        TVector<TypeDescriptor>                                     m_descriptors;
        int32                                                       m_totalRequiredSize = -1;
        int32                                                       m_requiredAlignment = -1;
        TVector<TypeInfo const*>                                    m_typeInfos;
        TVector<uint32>                                             m_typeSizes;
        TVector<uint32>                                             m_typePaddings;
    };
}