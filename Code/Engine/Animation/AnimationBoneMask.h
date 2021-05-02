#pragma once

#include "_Module/API.h"
#include "System/Core/Types/Containers.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class Skeleton;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API BoneMask
    {

    public:

        static inline BoneMask Blend( BoneMask const& source, BoneMask const& target, float blendWeight )
        {
            BoneMask result( source );
            result.BlendTo( target, blendWeight );
            return result;
        }

    public:

        BoneMask() : m_pSkeleton( nullptr ) {}
        BoneMask( Skeleton const* pSkeleton );
        BoneMask( Skeleton const* pSkeleton, float const defaultWeight );

        BoneMask( BoneMask const& rhs );
        BoneMask( BoneMask&& rhs );

        //-------------------------------------------------------------------------

        inline bool IsValid() const { return m_pSkeleton != nullptr && !m_weights.empty(); }
        inline Skeleton const* GetSkeleton() const { return m_pSkeleton; }
        inline int32 GetNumWeights() const { return (int32) m_weights.size(); }
        inline float GetWeight( uint32 i ) const { KRG_ASSERT( i < (uint32) m_weights.size() ); return m_weights[i]; }
        inline float operator[]( uint32 i ) const { return GetWeight( i ); }

        void ResetWeights() { Memory::MemsetZero( m_weights.data(), m_weights.size() ); }
        void ResetWeights( float weight );

        BoneMask& operator*=( BoneMask const& RHS );
        void BlendFrom( BoneMask const& Source, float BlendWeight );
        void BlendTo( BoneMask const& Target, float BlendWeight );

    private:

        Skeleton const*             m_pSkeleton = nullptr;
        TVector<float>              m_weights;
    };
}