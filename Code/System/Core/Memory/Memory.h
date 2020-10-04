#pragma once

#include "System/Core/_Module/API.h"
#include "System/Core/Core.h"
#include <algorithm>
#include <malloc.h>

//-------------------------------------------------------------------------

#define KRG_USE_CUSTOM_ALLOCATOR 1
#define KRG_DEFAULT_ALIGNMENT 8

//-------------------------------------------------------------------------

#ifdef _WIN32

    #define KRG_STACK_ALLOC(x) alloca( x )
    #define KRG_STACK_ARRAY_ALLOC(type, numElements) reinterpret_cast<type*>( alloca( sizeof(type) * numElements ) );

#else

    #define KRG_STACK_ALLOC(x)
    #define KRG_STACK_ARRAY_ALLOC(type, numElements)

#endif

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Memory
    {
        KRG_SYSTEM_CORE_API void Initialize();
        KRG_SYSTEM_CORE_API void Shutdown();

        KRG_SYSTEM_CORE_API void InitializeThreadHeap();
        KRG_SYSTEM_CORE_API void ShutdownThreadHeap();

        //-------------------------------------------------------------------------

        void KRG_FORCE_INLINE MemsetZero( void* ptr, size_t size )
        {
            memset( ptr, 0, size );
        }

        template <typename T>
        void KRG_FORCE_INLINE MemsetZero( T* ptr )
        {
            memset( ptr, 0, sizeof( T ) );
        }

        //-------------------------------------------------------------------------

        inline bool IsAligned( void const* p, size_t n )
        {
            return ( reinterpret_cast<uintptr_t>( p ) % n ) == 0;
        }

        template <typename T>
        inline bool IsAligned( T const* p )
        {
            return ( reinterpret_cast<uintptr_t>( p ) % alignof( T ) ) == 0;
        }

        //-------------------------------------------------------------------------

        KRG_SYSTEM_CORE_API size_t GetTotalRequestedMemory();
        KRG_SYSTEM_CORE_API size_t GetTotalAllocatedMemory();
    }

    //-------------------------------------------------------------------------
    // Global Memory Management Functions
    //-------------------------------------------------------------------------

    KRG_SYSTEM_CORE_API void* Alloc( size_t size, size_t alignment = KRG_DEFAULT_ALIGNMENT );
    KRG_SYSTEM_CORE_API void* Realloc( void* pMemory, size_t newSize, size_t originalAlignment = KRG_DEFAULT_ALIGNMENT );
    KRG_SYSTEM_CORE_API void Free( void*& pMemory );

    //-------------------------------------------------------------------------

    template< typename T, typename ... ConstructorParams >
    KRG_FORCE_INLINE T* New( ConstructorParams&&... params )
    {
        void* pMemory = Alloc( sizeof( T ), alignof( T ) );
        KRG_ASSERT( pMemory != nullptr );
        return new( pMemory ) T( std::forward<ConstructorParams>( params )... );
    }

    template< typename T >
    KRG_FORCE_INLINE void Delete( T*& pType )
    {
        if ( pType != nullptr )
        {
            pType->~T();
            Free( (void*&) pType );
        }
    }

    template< typename T >
    KRG_FORCE_INLINE void Free( T*& pType )
    {
        Free( (void*&) pType );
    }

    //-------------------------------------------------------------------------

    template< typename T, typename ... ConstructorParams >
    KRG_FORCE_INLINE T* NewArray( size_t const numElements )
    {
        size_t const requiredAlignment = std::max( alignof( T ), size_t( 16 ) );
        size_t const requiredExtraMemory = std::max( requiredAlignment, size_t( 4 ) );
        size_t const requiredMemory = sizeof( T ) * numElements + requiredExtraMemory;

        Byte* pOriginalAddress = pOriginalAddress = (Byte*) Alloc( requiredMemory, requiredAlignment );
        KRG_ASSERT( pOriginalAddress != nullptr );

        // Call required type constructors
        T* pArrayAddress = reinterpret_cast<T*>( pOriginalAddress + requiredExtraMemory );
        for ( size_t i = 0; i < numElements; i++ )
        {
            new( &pArrayAddress[i] ) T( std::forward<ConstructorParams>( params )... );
        }

        // Record the number of array elements
        U32* pNumElements = reinterpret_cast<U32*>( pArrayAddress ) - 1;
        *pNumElements = U32( numElements );

        return pArrayAddress;
    }

    template< typename T >
    KRG_FORCE_INLINE T* NewArray( size_t const numElements, T const& value )
    {
        size_t const requiredAlignment = std::max( alignof( T ), size_t( 16 ) );
        size_t const requiredExtraMemory = std::max( requiredAlignment, size_t( 4 ) );
        size_t const requiredMemory = sizeof( T ) * numElements + requiredExtraMemory;

        Byte* pOriginalAddress = pOriginalAddress = (Byte*) Alloc( requiredMemory, requiredAlignment );
        KRG_ASSERT( pOriginalAddress != nullptr );

        // Call required type constructors
        T* pArrayAddress = reinterpret_cast<T*>( pOriginalAddress + requiredExtraMemory );
        for ( size_t i = 0; i < numElements; i++ )
        {
            new( &pArrayAddress[i] ) T( value );
        }

        // Record the number of array elements
        U32* pNumElements = reinterpret_cast<U32*>( pArrayAddress ) - 1;
        *pNumElements = U32( numElements );

        return pArrayAddress;
    }

    template< typename T >
    KRG_FORCE_INLINE void DeleteArray( T*& pArray )
    {
        size_t const requiredAlignment = std::max( alignof( T ), size_t( 16 ) );
        size_t const requiredExtraMemory = std::max( requiredAlignment, size_t( 4 ) );

        // Get number of elements in array and call destructor on each entity
        U32 const numElements = *( reinterpret_cast<U32*>( pArray ) - 1 );
        for ( U32 i = 0; i < numElements; i++ )
        {
            pArray[i].~T();
        }

        Byte* pOriginalAddress = reinterpret_cast<Byte*>( pArray ) - requiredExtraMemory;
        Free( (void*&) pOriginalAddress );
        pArray = nullptr;
    }
}