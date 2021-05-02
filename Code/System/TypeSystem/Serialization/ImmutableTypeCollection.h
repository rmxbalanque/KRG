#pragma once
#include "System/TypeSystem/TypeDescriptors.h"

//-------------------------------------------------------------------------
// Immutable Collection Serializer
//-------------------------------------------------------------------------
// This is used to serialize a set of types (all deriving from the same base) that are immutable at runtime
// These types are created in a single contiguous array of memory. 
// This is useful for deserializing authored data like anim events that requires the KRG reflection system

namespace KRG::TypeSystem
{
    struct KRG_SYSTEM_TYPESYSTEM_API ImmutableTypeCollectionHeader
    {
        KRG_SERIALIZE_MEMBERS( m_totalRequiredSize, m_requiredAlignment );

        uint32                                                       m_totalRequiredSize = 0;
        uint32                                                       m_requiredAlignment = 0;
    };

    //-------------------------------------------------------------------------

    struct KRG_SYSTEM_TYPESYSTEM_API ImmutableTypeDescriptor : public TypeDescriptor
    {
        KRG_SERIALIZE_MEMBERS( KRG_SERIALIZE_BASE( TypeDescriptor ), m_byteSize, m_bytePadding );

    public:

        // How large is this type
        uint32                                                      m_byteSize = 0;

        // How much padding do we need between this type and the previous type
        uint32                                                      m_bytePadding = 0;
    };

    //-------------------------------------------------------------------------

    // This function will create all the type instances in place, as well as output a list of pointers to each type. Requires that all types have the same base class
    template<typename T>
    void CreateImmutableTypeCollection( TypeRegistry const& typeRegistry, Byte* pAllocatedMemory, TVector<TypeSystem::ImmutableTypeDescriptor> const& typeDescs, TVector<T*>& outTypes )
    {
        Byte* pTypeMemory = pAllocatedMemory;
        for ( auto const& desc : typeDescs )
        {
            pTypeMemory += desc.m_bytePadding;
            outTypes.emplace_back( TypeCreator::CreateTypeFromDescriptor<T>( typeRegistry, desc, pTypeMemory ) );
            pTypeMemory += desc.m_byteSize;
        }
    }

    //-------------------------------------------------------------------------

    // This function simply calls the destructor
    template<typename T>
    void DestroyImmutableTypeCollection( TVector<T*>& types )
    {
        for ( auto pType : types )
        {
            pType->~T();
        }
    }
}