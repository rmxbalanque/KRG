#pragma once

#include "_Module/API.h"
#include "System/Core/Types/Containers.h"
#include "System/Core/Types/StringID.h"
#include "System/Core/Serialization/Serialization.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class Skeleton;

    //-------------------------------------------------------------------------

    struct KRG_ENGINE_ANIMATION_API BoneWeight
    {
        KRG_SERIALIZE_MEMBERS( m_boneID, m_weight );

        StringID        m_boneID;
        float           m_weight;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API BoneMask
    {

    public:

        static inline BoneMask SetFromBlend( BoneMask const& source, BoneMask const& target, float blendWeight )
        {
            BoneMask result( source );
            result.BlendTo( target, blendWeight );
            return result;
        }

    public:

        BoneMask() : m_pSkeleton( nullptr ) {}
        BoneMask( Skeleton const* pSkeleton );
        BoneMask( Skeleton const* pSkeleton, float fixedWeight );
        BoneMask( Skeleton const* pSkeleton, TVector<BoneWeight> const& weights, float rootMotionWeight );

        BoneMask( BoneMask const& rhs );
        BoneMask( BoneMask&& rhs );

        BoneMask& operator=( BoneMask const& rhs );
        BoneMask& operator=( BoneMask&& rhs );

        inline void CopyFrom( BoneMask const& rhs ) { *this = rhs; }

        //-------------------------------------------------------------------------

        inline bool IsValid() const;
        inline Skeleton const* GetSkeleton() const { return m_pSkeleton; }
        inline int32 GetNumWeights() const { return (int32) m_weights.size(); }
        inline float GetWeight( uint32 i ) const { KRG_ASSERT( i < (uint32) m_weights.size() ); return m_weights[i]; }
        inline float operator[]( uint32 i ) const { return GetWeight( i ); }
        BoneMask& operator*=( BoneMask const& rhs );

        // Set all weights to zero
        void ResetWeights() { Memory::MemsetZero( m_weights.data(), m_weights.size() ); m_rootMotionWeight = 0.0f; }

        // Set all weights to a fixed weight
        void ResetWeights( float fixedWeight, float rootMotionWeight );

        // Set all weights to a fixed weight
        void ResetWeights( float fixedWeight ) { ResetWeights( fixedWeight, fixedWeight ); }

        // Set all weights to the supplied weights, note that the number of supplied weights MUST match the expected num of weights!!
        void ResetWeights( TVector<float> const& weights, float rootMotionWeight );

        // Calculate bone weights based on a set of supplied bone IDs and weights, user can specify whether we should feather any weights between set bones
        // i.e. if head is set to 1.0f and pelvis is set to 0.2f, the intermediate spine bone weights will be gradually increased till they match they head weight
        void ResetWeights( TVector<BoneWeight> const& weights, float rootMotionWeight, bool shouldFeatherIntermediateBones = false );

        // Multiple the supplied bone mask into the current bone mask
        inline void CombineWith( BoneMask const& rhs ) { operator*=( rhs ); }

        // Blend from the supplied mask weight towards our weights with the supplied blend weight
        void BlendFrom( BoneMask const& source, float blendWeight );

        // Blend towards the supplied mask weights from our current weights with the supplied blend weight
        void BlendTo( BoneMask const& target, float blendWeight );

    private:

        Skeleton const*             m_pSkeleton = nullptr;
        TVector<float>              m_weights;
        float                       m_rootMotionWeight = 0.0f;
    };

    //-------------------------------------------------------------------------

    class BoneMaskPool
    {
        constexpr static int32 const s_initialPoolSize = 4;

    public:

        BoneMaskPool() : m_firstFreePoolIdx( InvalidIndex ) {}
        ~BoneMaskPool();

        inline bool IsEmpty() const { return m_pool.size() == 0; }
        void Initialize( Skeleton const* pSkeleton );
        void Shutdown();

        BoneMask* GetBoneMask();
        void Reset() { m_firstFreePoolIdx = 0; }

    private:

        TVector<BoneMask*>          m_pool;
        int32                       m_firstFreePoolIdx = InvalidIndex;
    };
}