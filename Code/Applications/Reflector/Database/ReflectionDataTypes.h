#if _WIN32
#pragma once

#include "ReflectionProjectTypes.h"
#include "System/TypeSystem/TypeInfo.h"
#include "System/TypeSystem/CoreTypeIDs.h"
#include "System/Resource/ResourceTypeID.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem
    {
        namespace Reflection
        {
            struct PropertyDescriptor
            {

            public:

                PropertyDescriptor()
                    : m_propertyID()
                    , m_arraySize( -1 )
                {}

                PropertyDescriptor( String const& name )
                    : m_name( name )
                    , m_propertyID( name )
                    , m_arraySize( -1 )
                {}

                PropertyDescriptor( String const& name, String const& typeName )
                    : m_name( name )
                    , m_propertyID( name )
                    , m_typeName( typeName )
                    , m_arraySize( -1 )
                {}

                inline bool IsEnumProperty() const { return m_flags.IsFlagSet( PropertyInfo::Flags::IsEnum ); }
                inline bool IsArrayProperty() const { return m_flags.IsFlagSet( PropertyInfo::Flags::IsArray ) || m_flags.IsFlagSet( PropertyInfo::Flags::IsDynamicArray ); }
                inline bool IsStaticArrayProperty() const { return m_flags.IsFlagSet( PropertyInfo::Flags::IsArray ) && !m_flags.IsFlagSet( PropertyInfo::Flags::IsDynamicArray ); }
                inline bool IsDynamicArrayProperty() const { return m_flags.IsFlagSet( PropertyInfo::Flags::IsDynamicArray ); }
                inline U32 GetArraySize() const { KRG_ASSERT( m_arraySize > 0 ); return (U32) m_arraySize; }

                inline bool operator==( PropertyDescriptor const& RHS ) const { return m_propertyID == RHS.m_propertyID; }
                inline bool operator!=( PropertyDescriptor const& RHS ) const { return m_propertyID != RHS.m_propertyID; }
                inline bool operator<( PropertyDescriptor const& RHS ) const { return (U32) m_propertyID < (U32) RHS.m_propertyID; }
                inline bool operator<=( PropertyDescriptor const& RHS ) const { return (U32) m_propertyID <= (U32) RHS.m_propertyID; }
                inline bool operator>( PropertyDescriptor const& RHS ) const { return (U32) m_propertyID > (U32) RHS.m_propertyID; }
                inline bool operator>=( PropertyDescriptor const& RHS ) const { return (U32) m_propertyID >= (U32) RHS.m_propertyID; }

            public:

                StringID                                m_propertyID;
                TypeID                                  m_typeID;
                String                                  m_name;
                String                                  m_typeName;
                String                                  m_templateArgTypeName;
                S32                                     m_arraySize;
                TFlags<PropertyInfo::Flags>             m_flags;
            };

            //-------------------------------------------------------------------------

            struct EnumConstantDescriptor
            {
                String                                  m_label;
                S32                                     m_value;
            };

            struct TypeDescriptor
            {
                enum class Flags
                {
                    IsAbstract = 0,
                    IsEnum,
                    IsEntity,
                    IsEntityComponent,
                    IsEntitySystem
                };

            public:

                TypeDescriptor()
                    : m_name( "Invalid" )
                {}

                TypeDescriptor( TypeID ID, String const& name ) 
                    : m_ID( ID )
                    , m_name( name )
                {}

                inline bool IsAbstract() const { return m_flags.IsFlagSet( Flags::IsAbstract ); }
                inline bool IsEnum() const { return m_flags.IsFlagSet( Flags::IsEnum ); }
                inline bool IsEntity() const { return m_flags.IsFlagSet( Flags::IsEntity ); }
                inline bool IsEntityComponent() const { return m_flags.IsFlagSet( Flags::IsEntityComponent ); }
                inline bool IsEntitySystem() const { return m_flags.IsFlagSet( Flags::IsEntitySystem ); }

                // Structure functions
                inline bool IsDerivedType() const { return !m_parents.empty(); }
                PropertyDescriptor const* GetPropertyDescriptor( StringID propertyID ) const;

                // Enum functions
                void AddValue( EnumConstantDescriptor const& constant );
                bool IsValidLabelID( StringID labelID ) const { return m_constants.find( labelID ) != m_constants.end(); }
                bool GetValueFromLabel( StringID labelID, U32& value ) const;

            public:

                TypeID                                          m_ID;
                HeaderID                                        m_headerID;
                String                                          m_name;
                String                                          m_namespace;
                TFlags<Flags>                                   m_flags;

                // Structures
                TVector<TypeID>                                 m_parents;
                TVector<PropertyDescriptor>                     m_properties;

                // Enums
                CoreTypes                                       m_underlyingType = CoreTypes::U8;
                THashMap<StringID, EnumConstantDescriptor>      m_constants;
            };

            //-------------------------------------------------------------------------

            struct ResourceDesc
            {
                TypeID                                          m_typeID;
                ResourceTypeID                                  m_resourceTypeID;
                HeaderID                                        m_headerID;
                String                                          m_className;
                String                                          m_namespace;
                bool                                            m_isVirtual;
            };
        }
    }
}

#endif