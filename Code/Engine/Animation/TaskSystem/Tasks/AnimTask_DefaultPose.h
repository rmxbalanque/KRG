#pragma once

#include "../AnimTask.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        namespace Tasks
        {
            class DefaultPoseTask : public Task
            {

            public:

                enum class PoseType
                {
                    Zero,
                    Reference
                };

            public:

                DefaultPoseTask( S32 sourceNodeIdx, PoseType type );
                virtual void Execute( TaskPoseStack& poseStack ) const override;

                #if KRG_DEVELOPMENT_TOOLS
                virtual String GetDebugText() const override { return "Default Pose Task"; }
                virtual S32 GetDependencyCount() const { return 0; }
                virtual Color GetDebugColor() const { return Colors::LightGray; }
                #endif

            private:

                PoseType m_type;
            };
        }
    }
}