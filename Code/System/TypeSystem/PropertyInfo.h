#pragma once

#include "_Module/API.h"
#include "TypeID.h"
#include "System/Core/Types/BitFlags.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    struct KRG_SYSTEM_TYPESYSTEM_API PropertyInfo
    {
        enum class Flags
        {
            IsArray = 0,
            IsDynamicArray,
            IsEnum,
            IsBitFlags,
            IsStructure,
        };

    public:

        PropertyInfo() = default;

        inline bool IsValid() const { return m_ID.IsValid(); }

        // General queries
        //-------------------------------------------------------------------------

        inline bool IsStructureProperty() const { return m_flags.IsFlagSet( Flags::IsStructure ); }
        inline bool IsEnumProperty() const { return m_flags.IsFlagSet( Flags::IsEnum ); }
        inline bool IsBitFlagsProperty() const { return m_flags.IsFlagSet( Flags::IsBitFlags ); }
        inline bool IsArrayProperty() const { return m_flags.IsFlagSet( Flags::IsArray ) || m_flags.IsFlagSet( Flags::IsDynamicArray ); }
        inline bool IsStaticArrayProperty() const { return m_flags.IsFlagSet( Flags::IsArray ) && !m_flags.IsFlagSet( Flags::IsDynamicArray ); }
        inline bool IsDynamicArrayProperty() const { return m_flags.IsFlagSet( Flags::IsDynamicArray ); }

        // Addressing functions
        //-------------------------------------------------------------------------
        // Warning! These are only valid if the type address is the immediate parent type!

        template<typename T>
        inline T* GetPropertyAddress( void* pTypeAddress ) const
        {
            KRG_ASSERT( IsValid() );
            return reinterpret_cast<T*>( ( reinterpret_cast<Byte*>( pTypeAddress ) + m_offset ) );
        }

        template<typename T>
        inline T const* GetPropertyAddress( void const* pTypeAddress ) const
        {
            KRG_ASSERT( IsValid() );
            return reinterpret_cast<T const*>( ( reinterpret_cast<Byte const*>( pTypeAddress ) + m_offset ) );
        }

        inline void* GetPropertyAddress( void* pTypeAddress ) const
        {
            KRG_ASSERT( IsValid() );
            return reinterpret_cast<Byte*>( pTypeAddress ) + m_offset;
        }

        inline void const* GetPropertyAddress( void const* pTypeAddress ) const
        {
            KRG_ASSERT( IsValid() );
            return reinterpret_cast<Byte const*>( pTypeAddress ) + m_offset;
        }

        // Default value
        //-------------------------------------------------------------------------
        // NB! no type-safety here, these functions are not for general use!

        template<typename T>
        inline T const& GetDefaultValue() const
        {
            KRG_ASSERT( !IsArrayProperty() );
            T const& defaultValue = *reinterpret_cast<T const*>( m_pDefaultValue );
            return defaultValue;
        }

        inline void const* GetArrayDefaultElementPtr( int32 elementIdx ) const
        {
            KRG_ASSERT( IsArrayProperty() && m_pDefaultArrayData != nullptr );
            KRG_ASSERT( elementIdx >= 0 && elementIdx < m_arraySize );
            KRG_ASSERT( m_arraySize > 0 && m_arrayElementSize > 0 );
            Byte const* arrayDataPtr = (Byte const*) m_pDefaultArrayData;
            return arrayDataPtr + ( m_arrayElementSize * elementIdx );
        }

    public:

        StringID                    m_ID;                                   // Property ID
        TypeID                      m_typeID;                               // Property Type ID
        TypeID                      m_parentTypeID;                         // The type ID for the parent type that this property belongs to
        TypeID                      m_templateArgumentTypeID;               // A property's contained TypeID for templatized types i.e. the specialization type for a TResourcePtr
        int32                       m_size = -1;                            // Byte size of property / total array byte size for static array properties
        int32                       m_offset = -1;                          // The byte offset from its owning type
        int32                       m_arraySize = -1;                       // Number of elements the array (for static arrays this is the static dimensions, for dynamic arrays this is the default size)
        int32                       m_arrayElementSize = -1;                // Byte size of an individual array element
        void const*                 m_pDefaultValue = nullptr;              // A ptr to the default value of the property
        void const*                 m_pDefaultArrayData = nullptr;          // A ptr to the contained data within the default value array
        TBitFlags<Flags>            m_flags;                                // Info about property type
    };
}