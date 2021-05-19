#include "AnimationGraphBoneMaskPool.h"
#include "Engine/Animation/AnimationSkeleton.h"
#include "Engine/Animation/AnimationBoneMask.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    BoneMaskPool::~BoneMaskPool()
    {
        KRG_ASSERT( m_pool.empty() && m_firstFreePoolIdx == InvalidIndex );
    }

    void BoneMaskPool::Initialize( Skeleton const* pSkeleton )
    {
        KRG_ASSERT( m_pool.empty() );

        for ( auto i = 0; i < s_initialPoolSize; i++ )
        {
            m_pool.emplace_back( KRG::New<BoneMask>( pSkeleton ) );
        }

        m_firstFreePoolIdx = 0;
    }

    void BoneMaskPool::Shutdown()
    {
        for ( auto pMask : m_pool )
        {
            KRG::Delete( pMask );
        }

        m_pool.clear();
        m_firstFreePoolIdx = InvalidIndex;
    }

    BoneMask* BoneMaskPool::GetBoneMask()
    {
        KRG_ASSERT( m_firstFreePoolIdx < m_pool.size() );
        auto pMask = m_pool[m_firstFreePoolIdx];
        m_firstFreePoolIdx++;

        // If we reached the end of the pool, grow it
        if ( m_firstFreePoolIdx == m_pool.size() )
        {
            size_t const numMasksToAdd = m_pool.size() / 2;
            for ( auto i = 0; i < numMasksToAdd; i++ )
            {
                m_pool.emplace_back( KRG::New<BoneMask>( m_pool[0]->GetSkeleton() ) );
            }
        }

        KRG_ASSERT( pMask != nullptr );
        return pMask;
    }
}