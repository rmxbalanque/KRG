#include "AnimationBoneMask.h"
#include "System/Animation/AnimationSkeleton.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    BoneMask::BoneMask( Skeleton const* pSkeleton )
        : m_pSkeleton( pSkeleton )
    {
        KRG_ASSERT( pSkeleton != nullptr );
        m_weights.resize( pSkeleton->GetNumBones(), 0.0f );
    }

    BoneMask::BoneMask( Skeleton const* pSkeleton, float fixedWeight )
        : m_pSkeleton( pSkeleton )
    {
        KRG_ASSERT( pSkeleton != nullptr && fixedWeight >= 0.0f && fixedWeight <= 1.0f );
        m_weights.resize( pSkeleton->GetNumBones(), fixedWeight );
        m_rootMotionWeight = fixedWeight;
    }

    BoneMask::BoneMask( BoneMask const& rhs )
    {
        m_pSkeleton = rhs.m_pSkeleton;
        m_weights = rhs.m_weights;
        m_rootMotionWeight = rhs.m_rootMotionWeight;
    }

    BoneMask::BoneMask( BoneMask&& rhs )
    {
        m_pSkeleton = rhs.m_pSkeleton;
        m_weights.swap( rhs.m_weights );
        m_rootMotionWeight = rhs.m_rootMotionWeight;
    }

    BoneMask::BoneMask( Skeleton const* pSkeleton, TVector<BoneWeight> const& weights, float rootMotionWeight )
        : m_pSkeleton( pSkeleton )
    {
        KRG_ASSERT( pSkeleton != nullptr && rootMotionWeight >= 0.0f && rootMotionWeight <= 1.0f );
        ResetWeights( weights, rootMotionWeight );
    }

    //-------------------------------------------------------------------------

    BoneMask& BoneMask::operator=( BoneMask const& rhs )
    {
        m_pSkeleton = rhs.m_pSkeleton;
        m_weights = rhs.m_weights;
        m_rootMotionWeight = rhs.m_rootMotionWeight;
        return *this;
    }

    BoneMask& BoneMask::operator=( BoneMask&& rhs )
    {
        m_pSkeleton = rhs.m_pSkeleton;
        m_weights.swap( rhs.m_weights );
        m_rootMotionWeight = rhs.m_rootMotionWeight;
        return *this;
    }

    //-------------------------------------------------------------------------

    void BoneMask::ResetWeights( float fixedWeight )
    {
        for ( auto& weight : m_weights )
        {
            weight = fixedWeight;
        }

        m_rootMotionWeight = fixedWeight;
    }

    void BoneMask::ResetWeights( TVector<BoneWeight> const& weights, float rootMotionWeight )
    {
        KRG_UNIMPLEMENTED_FUNCTION();

        m_rootMotionWeight = rootMotionWeight;
    }

    BoneMask& BoneMask::operator*=( BoneMask const& rhs )
    {
        int32 const numWeights = (int32) m_weights.size();
        for ( auto i = 0; i < numWeights; i++ )
        {
            m_weights[i] *= rhs.m_weights[i];
        }

        m_rootMotionWeight *= rhs.m_rootMotionWeight;

        return *this;
    }

    void BoneMask::BlendFrom( BoneMask const& source, float blendWeight )
    {
        KRG_ASSERT( source.m_pSkeleton == m_pSkeleton && m_weights.size() == source.m_weights.size() && blendWeight >= 0.0f && blendWeight <= 1.0f );

        auto const numWeights = m_weights.size();
        for ( auto i = 0; i < numWeights; i++ )
        {
            m_weights[i] = Math::Lerp( source.m_weights[i], m_weights[i], blendWeight );
        }

        m_rootMotionWeight = Math::Lerp( source.m_rootMotionWeight, m_rootMotionWeight, blendWeight );
    }

    void BoneMask::BlendTo( BoneMask const& target, float blendWeight )
    {
        KRG_ASSERT( target.m_pSkeleton == m_pSkeleton && m_weights.size() == target.m_weights.size() && blendWeight >= 0.0f && blendWeight <= 1.0f );

        auto const numWeights = m_weights.size();
        for ( auto i = 0; i < numWeights; i++ )
        {
            m_weights[i] = Math::Lerp( m_weights[i], target[i], blendWeight );
        }

        m_rootMotionWeight = Math::Lerp( m_rootMotionWeight, target.m_rootMotionWeight, blendWeight );
    }

    //-------------------------------------------------------------------------

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