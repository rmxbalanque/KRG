#pragma once

#include "Engine/Animation/AnimationPose.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    struct PoseBuffer
    {
        friend class PoseBufferPool;

    public:

        PoseBuffer( Skeleton const* pSkeleton );
        void Reset();

        void CopyFrom( PoseBuffer const& RHS );
        inline void CopyFrom( PoseBuffer const* RHS ) { CopyFrom( *RHS ); }

    public:

        Pose								m_pose;

    private:

        bool								m_isUsed = false;
    };

    //-------------------------------------------------------------------------

    struct CachedPoseBuffer : PoseBuffer
    {
        friend class PoseBufferPool;

    public:

        using PoseBuffer::PoseBuffer;

        inline void Reset()
        {
            PoseBuffer::Reset();
            m_ID.Reset();
            m_shouldBeDestroyed = false;
        }

    public:

        UUID							m_ID;
        bool							m_shouldBeDestroyed = false;
    };

    //-------------------------------------------------------------------------

    class PoseBufferPool
    {
        constexpr static int8 const s_numInitialBuffers = 6;
        constexpr static int8 const s_bufferGrowAmount = 3;

    public:

        PoseBufferPool( Skeleton const* pSkeleton );
        ~PoseBufferPool();

        void Reset();

        // Poses
        //-------------------------------------------------------------------------

        int8 RequestPoseBuffer();
        void ReleasePoseBuffer( int8 bufferIdx );

        inline PoseBuffer* GetBuffer( int8 bufferIdx )
        {
            KRG_ASSERT( m_poseBuffers[bufferIdx].m_isUsed );
            return &m_poseBuffers[bufferIdx];
        }

        // Cached Poses
        //-------------------------------------------------------------------------

        UUID CreateCachedPoseBuffer();
        void DestroyCachedPoseBuffer( UUID const& cachedPoseID );
        PoseBuffer* GetCachedPoseBuffer( UUID const& cachedPoseID );

        // Debug
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        inline bool IsDebugRecordingEnabled() const { return m_isDebugRecordingEnabled; }
        inline void SetDebugRecordingState( bool enabled ) const { m_isDebugRecordingEnabled = enabled; }
        void RecordPose( int8 poseBufferIdx );
        PoseBuffer const* GetRecordedPose( int8 debugBufferIdx ) const;
        #endif

    private:

        Skeleton const*                             m_pSkeleton = nullptr;
        TVector<PoseBuffer>                         m_poseBuffers;
        TVector<CachedPoseBuffer>                   m_cachedBuffers;
        TInlineVector<UUID, 5>                      m_cachedPoseBuffersToDestroy;
        int8                                        m_firstFreeCachedBuffer = 0;
        int8                                        m_firstFreeBuffer = 0;

        #if KRG_DEVELOPMENT_TOOLS
        TVector<PoseBuffer>                         m_debugBuffers;
        int8                                        m_firstFreeDebugBuffer = 0;
        mutable bool                                m_isDebugRecordingEnabled = false;
        #endif
    };
}