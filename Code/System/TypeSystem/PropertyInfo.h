#pragma once

#include "_Module/API.h"
#include "TypeID.h"
#include "System/Core/Types/Flags.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem
    {
        struct KRG_SYSTEM_TYPESYSTEM_API PropertyInfo
        {
            enum class Flags
            {
                IsArray = 0,
                IsDynamicArray,
                IsEnum,
            };

        public:

            PropertyInfo() = default;

            inline bool IsValid() const { return m_ID.IsValid(); }

            // Array queries
            //-------------------------------------------------------------------------

            inline bool IsEnumProperty() const { return m_flags.IsFlagSet( Flags::IsEnum ); }
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

            inline void const* GetArrayDefaultElementPtr( S32 elementIdx ) const
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
            S32                         m_size = -1;                            // Byte size of property / total array byte size for static array properties
            S32                         m_offset = -1;                          // The byte offset from its owning type
            S32                         m_arraySize = -1;                       // Number of elements the array (for static arrays this is the static dimensions, for dynamic arrays this is the default size)
            S32                         m_arrayElementSize = -1;                // Byte size of an individual array element
            void const*                 m_pDefaultValue = nullptr;              // A ptr to the default value of the property
            void const*                 m_pDefaultArrayData = nullptr;          // A ptr to the contained data within the default value array
            TFlags<Flags>               m_flags;                                // Info about property type
        };

        //-------------------------------------------------------------------------
        // Property info relative to a specific type
        //-------------------------------------------------------------------------
        // This is the property info for a given property in a type, it provide specific addressing functions relative to the initial type provided

        struct ResolvedPropertyInfo
        {
            inline bool IsValid() const { return m_pPropertyInfo != nullptr; }

            // Addressing functions
            //-------------------------------------------------------------------------
            // Warning: These are only valid is the type address the type from where the resolution started

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

        public:

            PropertyInfo const*     m_pPropertyInfo = nullptr;  // The found property info
            S32                     m_offset = -1;              // Byte offset from parent type
        };
    }
}