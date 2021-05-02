#include "AnimationBoneMask.h"
#include "AnimationSkeleton.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    BoneMask::BoneMask( Skeleton const* pSkeleton )
        : m_pSkeleton( pSkeleton )
    {
        KRG_ASSERT( pSkeleton != nullptr );
        m_weights.resize( pSkeleton->GetNumBones(), 0.0f );
    }

    BoneMask::BoneMask( Skeleton const* pSkeleton, float const defaultWeight )
        : m_pSkeleton( pSkeleton )
    {
        KRG_ASSERT( pSkeleton != nullptr );
        m_weights.resize( pSkeleton->GetNumBones(), defaultWeight );
    }

    BoneMask::BoneMask( BoneMask const& rhs )
    {
        KRG_ASSERT( rhs.m_pSkeleton != nullptr );

        if ( rhs.m_pSkeleton != m_pSkeleton )
        {
            m_weights.resize( rhs.m_weights.size() );
        }

        KRG_ASSERT( m_weights.size() == rhs.m_weights.size() );
        memcpy( m_weights.data(), rhs.m_weights.data(), m_weights.size() );
    }

    BoneMask::BoneMask( BoneMask&& rhs )
    {
        KRG_ASSERT( rhs.m_pSkeleton != nullptr );

        m_pSkeleton = rhs.m_pSkeleton;
        m_weights.swap( rhs.m_weights );
    }

    //-------------------------------------------------------------------------

    void BoneMask::ResetWeights( float defaultWeight )
    {
        for ( auto& weight : m_weights )
        {
            weight = defaultWeight;
        }
    }

    BoneMask& BoneMask::operator*=( BoneMask const& rhs )
    {
        int32 const numWeights = (int32) m_weights.size();
        for ( auto i = 0; i < numWeights; i++ )
        {
            m_weights[i] *= rhs.m_weights[i];
        }

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
    }

    void BoneMask::BlendTo( BoneMask const& target, float blendWeight )
    {
        KRG_ASSERT( target.m_pSkeleton == m_pSkeleton && m_weights.size() == target.m_weights.size() && blendWeight >= 0.0f && blendWeight <= 1.0f );

        auto const numWeights = m_weights.size();
        for ( auto i = 0; i < numWeights; i++ )
        {
            m_weights[i] = Math::Lerp( m_weights[i], target[i], blendWeight );
        }
    }
}