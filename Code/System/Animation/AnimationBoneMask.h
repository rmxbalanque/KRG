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

        class KRG_SYSTEM_ANIMATION_API BoneMask
        {

        public:

            BoneMask() : m_pSkeleton( nullptr ) {}
            BoneMask( Skeleton const* pSkeleton );
            BoneMask( Skeleton const* pSkeleton, F32 const defaultWeight );

            void ResetWeights() { Memory::MemsetZero( m_weights.data(), m_weights.size() ); }
            void ResetWeights( F32 weight );

            void CopyFrom( BoneMask const& rhs );
            void BlendFrom( BoneMask const& source, F32 blendWeight );
            void BlendTo( BoneMask const& target, F32 blendWeight );

            inline Skeleton const* GetSkeleton() const { return m_pSkeleton; }

            inline bool IsValid() const { return !m_weights.empty(); }
            inline S32 GetNumWeights() const { return (S32) m_weights.size(); }
            inline F32 GetWeight( U32 i ) const { KRG_ASSERT( i < (U32) m_weights.size() ); return m_weights[i]; }
            inline F32 operator[]( U32 i ) const { return GetWeight( i ); }

        private:

            Skeleton const*         m_pSkeleton;
            TVector<F32>            m_weights;
        };
    }
}