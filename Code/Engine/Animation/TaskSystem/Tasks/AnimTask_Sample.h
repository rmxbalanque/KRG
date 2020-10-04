#pragma once

#include "../AnimTask.h"
#include "System/Animation/AnimationData.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        namespace Tasks
        {
            class Sample : public Task
            {

            public:

                Sample( S32 sourceNodeIdx, AnimationData const* pAnimation, Percentage time );
                virtual void Execute( TaskPoseStack& poseStack ) const override;

                #if KRG_DEBUG_INSTRUMENTATION
                virtual S32 GetDependencyCount() const { return 0; }
                virtual Color GetDebugColor() const { return Colors::SpringGreen; }
                virtual String GetDebugText() const override;
                #endif

            private:

                AnimationData const*    m_pAnimation;
                Percentage           m_time;
            };
        }
    }
}