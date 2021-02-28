#pragma once

#include "Engine/Animation/AnimationPose.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        class TaskPoseStack
        {

        public:

            TaskPoseStack( Skeleton const* pSkeleton );

            void Reset();

            inline Pose const* GetFinalResult() const { KRG_ASSERT( m_firstFreePose == 1 ); return &m_poses[0]; }

            Pose* GetPose();
            Pose* PushPose();
            Pose* PopPose();

            #if KRG_DEVELOPMENT_TOOLS
            void EnablePoseDebugging( bool enabled ) const { m_isPoseDebuggingEnabled = enabled; }
            void RecordDebugPose( Pose const* pPose );
            #endif

        private:

            Skeleton const*                 m_pSkeleton;
            TVector<Pose>                   m_poses;
            S32                             m_firstFreePose;

        private:

            #if KRG_DEVELOPMENT_TOOLS
            TVector<Pose>                   m_debugBuffers;
            S32                             m_firstFreeDebugBuffer;
            mutable bool                    m_isPoseDebuggingEnabled;
            #endif
        };
    }
}