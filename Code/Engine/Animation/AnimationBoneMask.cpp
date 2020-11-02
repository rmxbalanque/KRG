#include "AnimationBoneMask.h"
#include "AnimationSkeleton.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        /*static void FillBoneMask()
        {
            //// Set common bone weights
            //auto const numWeights = commonBoneWeights.size();
            //for ( auto i = 0; i < numWeights; i++ )
            //{
            //    S32 boneIdx = m_pSkeleton->GetBoneIndex( commonBoneWeights[i].BoneName );
            //    if ( boneIdx != InvalidIndex )
            //    {
            //        m_weights[boneIdx] = commonBoneWeights[i].Weight;
            //    }
            //}

            //// Fill in weights for intermediate bones in the skeleton
            //S32* pParentChain = (S32*) appAlloca( sizeof( S32 ) * 15 );

            //F32 startWeight = -1, endWeight = -1;
            //auto const numBones = m_pSkeleton->GetNumBones();
            //for ( auto i = numBones - 1; i >= 0; i-- )
            //{
            //    // Find the chain of bones with -1 weights
            //    if ( m_weights[i] == -1 )
            //    {
            //        S32 chainCount = 0;
            //        auto boneIdx = i;
            //        while ( m_weights[boneIdx] == -1 )
            //        {
            //            pParentChain[chainCount++] = i;
            //            KRG_ASSERT( chainCount < 15 ); // If this triggers, increase the size of the stack array
            //            boneIdx = m_pSkeleton->GetParentIndexForBone( boneIdx );
            //        }

            //        KRG_ASSERT( boneIdx >= 0 );
            //        endWeight = m_weights[boneIdx];
            //        KRG_ASSERT( endWeight != -1.0f );

            //        // For extremity bones, use the first weight encountered
            //        if ( startWeight == -1 || startWeight == endWeight )
            //        {
            //            for ( auto c = 0; c < chainCount; c++ )
            //            {
            //                m_weights[pParentChain[c]] = endWeight;
            //            }
            //        }
            //        else // Interpolate the weights on the intermediate bones
            //        {
            //            for ( auto c = 0; c < chainCount; c++ )
            //            {
            //                m_weights[pParentChain[c]] = Lerp( startWeight, endWeight, F32( c + 1 ) / ( chainCount + 1 ) );
            //            }
            //        }
            //    }
            //    else
            //    {
            //        startWeight = m_weights[i];
            //    }
            //}
        }*/

        BoneMask::BoneMask( Skeleton const* pSkeleton )
            : m_pSkeleton( pSkeleton )
        {
            KRG_ASSERT( pSkeleton != nullptr );
            m_weights.resize( pSkeleton->GetNumBones(), 0.0f );
        }

        BoneMask::BoneMask( Skeleton const* pSkeleton, F32 const defaultWeight )
            : m_pSkeleton( pSkeleton )
        {
            KRG_ASSERT( pSkeleton != nullptr );
            m_weights.resize( pSkeleton->GetNumBones(), defaultWeight );
        }

        void BoneMask::ResetWeights( F32 defaultWeight )
        {
            for ( auto& weight : m_weights )
            {
                weight = defaultWeight;
            }
        }

        void BoneMask::CopyFrom( BoneMask const& rhs )
        {
            if ( rhs.m_pSkeleton != m_pSkeleton )
            {
                m_weights.resize( rhs.m_weights.size() );
            }

            KRG_ASSERT( m_weights.size() == rhs.m_weights.size() );
            memcpy( m_weights.data(), rhs.m_weights.data(), m_weights.size() );
        }

        void BoneMask::BlendFrom( BoneMask const& source, F32 blendWeight )
        {
            KRG_ASSERT( source.m_pSkeleton == m_pSkeleton && m_weights.size() == source.m_weights.size() && blendWeight >= 0.0f && blendWeight <= 1.0f );

            auto const numWeights = m_weights.size();
            for ( auto i = 0; i < numWeights; i++ )
            {
                m_weights[i] = Math::Lerp( source.m_weights[i], m_weights[i], blendWeight );
            }
        }

        void BoneMask::BlendTo( BoneMask const& target, F32 blendWeight )
        {
            KRG_ASSERT( target.m_pSkeleton == m_pSkeleton && m_weights.size() == target.m_weights.size() && blendWeight >= 0.0f && blendWeight <= 1.0f );

            auto const numWeights = m_weights.size();
            for ( auto i = 0; i < numWeights; i++ )
            {
                m_weights[i] = Math::Lerp( m_weights[i], target[i], blendWeight );
            }
        }
    }
}
