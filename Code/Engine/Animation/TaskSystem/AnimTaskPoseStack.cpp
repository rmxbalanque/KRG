#include "AnimTaskPoseStack.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        namespace
        {
            static int32 const g_numDebugBuffers = 20;
            static int32 const g_numWorkingBuffers = 1;
        }

        //-------------------------------------------------------------------------

        TaskPoseStack::TaskPoseStack( Skeleton const* pSkeleton )
            : m_pSkeleton( pSkeleton )
            , m_firstFreePose( 0 )
        {
            KRG_ASSERT( pSkeleton != nullptr );

            // Add at least one pose
            m_poses.emplace_back( Pose( m_pSkeleton ) );

            #if KRG_DEVELOPMENT_TOOLS
            m_firstFreeDebugBuffer = 0;
            for ( auto i = 0; i < g_numDebugBuffers; i++ )
            {
                m_debugBuffers.emplace_back( Pose( pSkeleton, Pose::InitialState::None ) );
            }
            m_isPoseDebuggingEnabled = true;
            #endif
        }

        void TaskPoseStack::Reset()
        {
            m_firstFreePose = 0;

            for ( auto& pose : m_poses )
            {
                pose.ClearGlobalTransforms();
            }

            #if KRG_DEVELOPMENT_TOOLS
            m_firstFreeDebugBuffer = 0;
            #endif
        }

        Pose* TaskPoseStack::GetPose()
        {
            KRG_ASSERT( m_firstFreePose > 0 );
            return &m_poses[m_firstFreePose - 1];
        }

        Pose* TaskPoseStack::PushPose()
        {
            Pose* pPose = nullptr;
            if ( m_firstFreePose == m_poses.size() )
            {
                m_poses.emplace_back( Pose( m_pSkeleton ) );
                pPose = &m_poses.back();
            }
            else
            {
                pPose = &m_poses[m_firstFreePose];
            }

            KRG_ASSERT( pPose != nullptr );
            m_firstFreePose++;
            return pPose;
        }

        Pose* TaskPoseStack::PopPose()
        {
            KRG_ASSERT( m_firstFreePose > 1 ); // Should never pop the last pose
            m_firstFreePose--;
            return &m_poses[m_firstFreePose];
        }

        #if KRG_DEVELOPMENT_TOOLS
        void TaskPoseStack::RecordDebugPose( Pose const* pPose )
        {
            if ( !m_isPoseDebuggingEnabled )
            {
                return;
            }

            // If we are out of buffers, add additional debug buffers
            if ( m_firstFreeDebugBuffer == m_debugBuffers.size() )
            {
                for ( auto i = 0; i < g_numDebugBuffers; i++ )
                {
                    m_debugBuffers.emplace_back( Pose( m_pSkeleton, Pose::InitialState::None ) );
                }
            }

            m_debugBuffers[m_firstFreeDebugBuffer++].CopyFrom( pPose );
        }
        #endif
    }
}