#pragma once

#include "_Module/API.h"
#include "System/Core/Types/Containers.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        class Skeleton;

        //-------------------------------------------------------------------------

        class KRG_ENGINE_ANIMATION_API BoneMask
        {

        public:

            BoneMask() : m_pSkeleton( nullptr ) {}
            BoneMask( Skeleton const* pSkeleton );
            BoneMask( Skeleton const* pSkeleton, float const defaultWeight );

            void ResetWeights() { Memory::MemsetZero( m_weights.data(), m_weights.size() ); }
            void ResetWeights( float weight );

            void CopyFrom( BoneMask const& rhs );
            void BlendFrom( BoneMask const& source, float blendWeight );
            void BlendTo( BoneMask const& target, float blendWeight );

            inline Skeleton const* GetSkeleton() const { return m_pSkeleton; }

            inline bool IsValid() const { return !m_weights.empty(); }
            inline int32 GetNumWeights() const { return (int32) m_weights.size(); }
            inline float GetWeight( uint32 i ) const { KRG_ASSERT( i < (uint32) m_weights.size() ); return m_weights[i]; }
            inline float operator[]( uint32 i ) const { return GetWeight( i ); }

        private:

            Skeleton const*         m_pSkeleton;
            TVector<float>            m_weights;
        };
    }
}