#pragma once

#include "System/Core/Memory/Memory.h"
#include <EASTL/vector.h>
#include <EASTL/array.h>
#include <EASTL/list.h>
#include <EASTL/set.h>
#include <EASTL/vector_set.h>
#include <EASTL/map.h>
#include <EASTL/queue.h>
#include <EASTL/deque.h>
#include <EASTL/hash_map.h>
#include <EASTL/hash_set.h>
#include <EASTL/bitset.h>
#include <EASTL/stack.h>
#include <EASTL/bitvector.h>
#include <EASTL/sort.h>
#include <EASTL/fixed_vector.h>
#include "EASTL/allocator.h"

//-------------------------------------------------------------------------

namespace KRG
{
    //-------------------------------------------------------------------------
    // Type aliases for STL
    //-------------------------------------------------------------------------

    template<typename T> using TVector = eastl::vector<T>;
    template<typename T> using TSortedVector = eastl::vector_set<T>;
    template<typename T, eastl_size_t S> using TInlineVector = eastl::fixed_vector<T, S, true>;
    template<typename T, eastl_size_t S> using TArray = eastl::array<T, S>;
    template<typename T> using TList = eastl::list<T>;
    template<typename T> using TSet = eastl::set<T>;
    template<typename T> using TStack = eastl::stack<T>;
    template<typename T> using TQueue = eastl::queue<T>;
    template<typename T> using TDeque = eastl::deque<T>;
    template<typename T> using TMultiSet = eastl::multiset<T>;
    template<typename K, typename V> using TMap = eastl::map<K, V>;
    template<typename K, typename V, typename H = eastl::hash<K>> using THashMap = eastl::hash_map<K, V, H>;
    template<typename V, typename H = eastl::hash<V>> using THashSet = eastl::hash_set<V, H>;
    template<typename K, typename V> using TPair = eastl::pair<K, V>;

    //-------------------------------------------------------------------------
    // Simple utility functions to improve syntactic usage of container types
    //-------------------------------------------------------------------------

    template<typename T>
    inline typename TVector<T>::const_iterator VectorFind( TVector<T> const& vector, T const& value )
    {
        return eastl::find( vector.begin(), vector.end(), value );
    }

    template<typename T>
    inline typename TVector<T>::iterator VectorFind( TVector<T>& vector, T const& value )
    {
        return eastl::find( vector.begin(), vector.end(), value );
    }

    // Usage: vectorFind( vector, value, [] ( T const& typeRef, V const& valueRef ) { ... } );
    template<typename T, typename V, typename Predicate>
    inline typename TVector<T>::iterator VectorFind( TVector<T>& vector, V const& value, Predicate predicate )
    {
        return eastl::find( vector.begin(), vector.end(), value, eastl::forward<Predicate>( predicate ) );
    }

    // Usage: vectorFind( vector, value, [] ( T const& typeRef, V const& valueRef ) { ... } );
    template<typename T, typename V, typename Predicate>
    inline typename TVector<T>::const_iterator VectorFind( TVector<T> const& vector, V const& value, Predicate predicate )
    {
        return eastl::find( vector.begin(), vector.end(), value, eastl::forward<Predicate>( predicate ) );
    }

    template<typename T>
    inline bool VectorContains( TVector<T> const& vector, T const& value )
    {
        return eastl::find( vector.begin(), vector.end(), value ) != vector.end();
    }

    template<typename T>
    inline bool VectorContains( TVector<T>& vector, T const& value )
    {
        return eastl::find( vector.begin(), vector.end(), value ) != vector.end();
    }

    // Usage: VectorContains( vector, value, [] ( T const& typeRef, V const& valueRef ) { ... } );
    template<typename T, typename V, typename Predicate>
    inline bool VectorContains( TVector<T>& vector, V const& value, Predicate predicate )
    {
        return eastl::find( vector.begin(), vector.end(), value, eastl::forward<Predicate>( predicate ) ) != vector.end();
    }

    template<typename T>
    inline int32 VectorFindIndex( TVector<T> const& vector, T const& value )
    {
        auto iter = eastl::find( vector.begin(), vector.end(), value );
        if ( iter == vector.end() )
        {
            return InvalidIndex;
        }
        else
        {
            return (int32) ( iter - vector.begin() );
        }
    }

    template<typename T, typename V, typename Predicate>
    inline int32 VectorFindIndex( TVector<T> const& vector, V const& value, Predicate predicate )
    {
        auto iter = eastl::find( vector.begin(), vector.end(), value, predicate );
        if ( iter == vector.end() )
        {
            return InvalidIndex;
        }
        else
        {
            return (int32) ( iter - vector.begin() );
        }
    }

    //-------------------------------------------------------------------------

    template<typename T, eastl_size_t S>
    inline typename TInlineVector<T, S>::const_iterator VectorFind( TInlineVector<T, S> const& vector, T const& value )
    {
        return eastl::find( vector.begin(), vector.end(), value );
    }

    template<typename T, eastl_size_t S>
    inline typename TInlineVector<T, S>::iterator VectorFind( TInlineVector<T, S>& vector, T const& value )
    {
        return eastl::find( vector.begin(), vector.end(), value );
    }

    template<typename T, eastl_size_t S, typename V, typename Predicate>
    inline typename TInlineVector<T, S>::iterator VectorFind( TInlineVector<T, S>& vector, V const& value, Predicate predicate )
    {
        return eastl::find( vector.begin(), vector.end(), value, eastl::forward<Predicate>( predicate ) );
    }

    template<typename T, eastl_size_t S>
    inline bool VectorContains( TInlineVector<T, S> const& vector, T const& value )
    {
        return eastl::find( vector.begin(), vector.end(), value ) != vector.end();
    }

    template<typename T, eastl_size_t S>
    inline bool VectorContains( TInlineVector<T, S>& vector, T const& value )
    {
        return eastl::find( vector.begin(), vector.end(), value ) != vector.end();
    }

    template<typename T, eastl_size_t S, typename V, typename Predicate>
    inline bool VectorContains( TInlineVector<T, S>& vector, V const& value, Predicate predicate )
    {
        return eastl::find( vector.begin(), vector.end(), value, eastl::forward<Predicate>( predicate ) ) != vector.end();
    }

    template<typename T, eastl_size_t S>
    inline int32 VectorFindIndex( TInlineVector<T, S> const& vector, T const& value )
    {
        auto iter = eastl::find( vector.begin(), vector.end(), value );
        if ( iter == vector.end() )
        {
            return InvalidIndex;
        }
        else
        {
            return ( int32) ( iter - vector.begin() );
        }
    }

    template<typename T, eastl_size_t S, typename V, typename Predicate>
    inline int32 VectorFindIndex( TInlineVector<T, S> const& vector, V const& value, Predicate predicate )
    {
        auto iter = eastl::find( vector.begin(), vector.end(), value, predicate );
        if ( iter == vector.end() )
        {
            return InvalidIndex;
        }
        else
        {
            return ( int32) ( iter - vector.begin() );
        }
    }

    //-------------------------------------------------------------------------

    template<typename T>
    inline void VectorEmplaceBackUnique( TVector<T>& vector, T&& item )
    {
        if ( !VectorContains( vector, item ) )
        {
            vector.emplace_back( eastl::forward( item ) );
        }
    }

    template<typename T>
    inline void VectorEmplaceBackUnique( TVector<T>& vector, T const& item )
    {
        if ( !VectorContains( vector, item ) )
        {
            vector.emplace_back( item );
        }
    }

    template<typename T, eastl_size_t S>
    inline void VectorEmplaceBackUnique( TInlineVector<T,S>& vector, T&& item )
    {
        if ( !VectorContains( vector, item ) )
        {
            vector.emplace_back( eastl::forward( item ) );
        }
    }

    template<typename T, eastl_size_t S>
    inline void VectorEmplaceBackUnique( TInlineVector<T, S>& vector, T const& item )
    {
        if ( !VectorContains( vector, item ) )
        {
            vector.emplace_back( item );
        }
    }
}