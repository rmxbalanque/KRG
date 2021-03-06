#pragma once
#include "Containers.h"

// Simple Pool Helper
//-------------------------------------------------------------------------
// Notes:
//  * Pool value type must be default constructible
//  * Pool ordering is non-deterministic
//  * Pool will grow to match desired size
//  * DO NOT rely on the address of any item in this pool. Always use the index to access the items.

namespace KRG
{
    template<typename T, size_t MinimumPoolSize = 10>
    class TPool
    {
    private:

        static constexpr float const GrowthRate = 1.5f;

        struct Buffer
        {
            T       m_value;
            bool    m_isUsed = false;
        };

    public:

        TPool( size_t initialSize = MinimumPoolSize )
            : m_pool( initialSize )
        {}

        ~TPool()
        {
            #if KRG_DEVELOPMENT_TOOLS
            for ( auto const& buffer : m_pool )
            {
                KRG_ASSERT( !buffer.m_isUsed );
            }
            #endif
        }

        inline T& GetBuffer( size_t idx )
        {
            KRG_ASSERT( idx < m_pool.size() && m_pool[idx].m_isUsed );
            return m_pool[idx].m_value;
        }

        inline size_t RequestBuffer( T&& value )
        {
            KRG_ASSERT( m_firstFreeBufferIdx < m_pool.size() && !m_pool[m_firstFreeBufferIdx].m_isUsed );
            m_pool[m_firstFreeBufferIdx].value = std::move( value );
            m_pool[m_firstFreeBufferIdx].m_isUsed = true;

            // Find the next free buffer
            while ( m_firstFreeBufferIdx < m_pool.size() && m_pool[m_firstFreeBufferIdx].m_isUsed )
            {
                m_firstFreeBufferIdx++;
            }

            // If we've completely filled the pool, grow the pool
            if ( m_firstFreeBufferIdx == m_pool.size() )
            {
                size_t const newSize = (size_t) Math::Ceiling( GrowthRate * m_pool.size() );
                m_pool.resize( newSize );
            }
        }

        inline void ReleaseBuffer( size_t idx )
        {
            KRG_ASSERT( idx < m_pool.size() && m_pool[idx].m_isUsed );

            m_pool[idx] = T();
            m_pool[idx].m_isUsed = false;

            if ( idx < m_firstFreeBufferIdx )
            {
                m_firstFreeBufferIdx = (int32) idx;
            }
        }

        void Reset()
        {
            m_pool.clear();
            m_firstFreeBufferIdx = InvalidIndex;
        }

        inline T const& operator[]( size_t idx ) const { return GetBuffer( idx ); }
        inline T& operator[]( size_t idx ) { return GetBuffer( idx ); }

    private:

        int32                                             m_firstFreeBufferIdx = InvalidIndex;
        TInlineVector<Buffer, MinimumPoolSize>          m_pool;
    };
}