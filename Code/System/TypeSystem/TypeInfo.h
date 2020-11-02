#pragma once

#include "PropertyInfo.h"
#include "ITypeHelper.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    enum class ETypeInfoMetaData
    {
        Entity,
        EntitySystem,
        EntityComponent,
    };

    //-------------------------------------------------------------------------

    struct KRG_SYSTEM_TYPESYSTEM_API TypeInfo
    {

    public:

        TypeInfo() = default;

        inline char const* GetTypeName() const { return m_ID.GetAsStringID().c_str(); }

        bool IsDerivedFrom( TypeID const parentTypeID ) const;

        template<typename T>
        inline bool IsDerivedFrom() const { return IsDerivedFrom( T::GetStaticTypeID() ); }

        // Properties
        //-------------------------------------------------------------------------

        PropertyInfo const* GetPropertyInfo( StringID propertyID ) const;

        // Function declaration for generated property registration functions
        template<typename T>
        void RegisterProperties( void const* pDefaultTypeInstance )
        {
            KRG_UNIMPLEMENTED_FUNCTION(); // Default implementation should never be called
        }

    public:

        TypeID                              m_ID;
        S32                                 m_size = -1;
        S32                                 m_alignment = -1;
        ITypeHelper*                        m_pTypeHelper = nullptr;
        TVector<TypeInfo const*>            m_parentTypes;
        THashMap<StringID, PropertyInfo>    m_properties;
        TFlags<ETypeInfoMetaData>           m_metadata;
    };
}