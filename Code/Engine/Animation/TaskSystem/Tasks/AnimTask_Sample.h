#pragma once

#include "../AnimTask.h"
#include "Engine/Animation/AnimationClip.h"

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

                Sample( int32 sourceNodeIdx, AnimationClip const* pAnimation, Percentage time );
                virtual void Execute( TaskPoseStack& poseStack ) const override;

                #if KRG_DEVELOPMENT_TOOLS
                virtual int32 GetDependencyCount() const { return 0; }
                virtual Color GetDebugColor() const { return Colors::SpringGreen; }
                virtual String GetDebugText() const override;
                #endif

            private:

                AnimationClip const*    m_pAnimation;
                Percentage           m_time;
            };
        }
    }
}