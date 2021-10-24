#pragma once

#include "PropertyInfo.h"
#include "ITypeHelper.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    enum class ETypeInfoMetaData
    {
        Abstract,
        Entity,
        EntitySystem,
        EntityComponent,
    };

    //-------------------------------------------------------------------------

    struct KRG_SYSTEM_TYPESYSTEM_API TypeInfo
    {

    public:

        TypeInfo() = default;

        inline char const* GetTypeName() const { return m_ID.ToStringID().c_str(); }

        bool IsAbstractType() const { return m_metadata.IsFlagSet( ETypeInfoMetaData::Abstract ); }

        bool IsDerivedFrom( TypeID const parentTypeID ) const;

        template<typename T>
        inline bool IsDerivedFrom() const { return IsDerivedFrom( T::GetStaticTypeID() ); }

        inline IRegisteredType const* GetDefaultInstance() const { return m_pTypeHelper->GetDefaultTypeInstancePtr(); }

        // Properties
        //-------------------------------------------------------------------------

        PropertyInfo const* GetPropertyInfo( StringID propertyID ) const;

        // Function declaration for generated property registration functions
        template<typename T>
        void RegisterProperties( IRegisteredType const* pDefaultTypeInstance )
        {
            KRG_UNIMPLEMENTED_FUNCTION(); // Default implementation should never be called
        }

    public:

        TypeID                                  m_ID;
        int32                                   m_size = -1;
        int32                                   m_alignment = -1;
        ITypeHelper*                            m_pTypeHelper = nullptr;
        TVector<TypeInfo const*>                m_parentTypes;
        TVector<PropertyInfo>                   m_properties;
        THashMap<StringID, int32>               m_propertyMap;
        TBitFlags<ETypeInfoMetaData>            m_metadata;
    };
}

//-------------------------------------------------------------------------

namespace KRG
{
    template<typename T>
    bool IsOfType( IRegisteredType const* pType )
    {
        KRG_ASSERT( pType != nullptr );
        return pType->GetTypeInfo()->IsDerivedFrom( T::GetStaticTypeID() );
    }

    // This is a assumed safe cast, it will validate the cast only in dev builds. Doesnt accept null arguments
    template<typename T>
    T* Cast( IRegisteredType* pType )
    {
        KRG_ASSERT( pType != nullptr );
        KRG_ASSERT( pType->GetTypeInfo()->IsDerivedFrom( T::GetStaticTypeID() ) );
        return static_cast<T*>( pType );
    }

    // This is a assumed safe cast, it will validate the cast only in dev builds. Doesnt accept null arguments
    template<typename T>
    T const* Cast( IRegisteredType const* pType )
    {
        KRG_ASSERT( pType != nullptr );
        KRG_ASSERT( pType->GetTypeInfo()->IsDerivedFrom( T::GetStaticTypeID() ) );
        return static_cast<T const*>( pType );
    }

    // This will try to cast to the specified type but can fail. Also accepts null arguments
    template<typename T>
    T* TryCast( IRegisteredType* pType )
    {
        if ( pType != nullptr && pType->GetTypeInfo()->IsDerivedFrom( T::GetStaticTypeID() ) )
        {
            return static_cast<T*>( pType );
        }

        return nullptr;
    }

    // This will try to cast to the specified type but can fail. Also accepts null arguments
    template<typename T>
    T const* TryCast( IRegisteredType const* pType )
    {
        if ( pType != nullptr && pType->GetTypeInfo()->IsDerivedFrom( T::GetStaticTypeID() ) )
        {
            return static_cast<T const*>( pType );
        }

        return nullptr;
    }
}