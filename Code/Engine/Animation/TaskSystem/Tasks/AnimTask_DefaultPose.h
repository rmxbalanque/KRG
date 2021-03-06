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

                DefaultPoseTask( int32 sourceNodeIdx, PoseType type );
                virtual void Execute( TaskPoseStack& poseStack ) const override;

                #if KRG_DEVELOPMENT_TOOLS
                virtual String GetDebugText() const override { return "Default Pose Task"; }
                virtual int32 GetDependencyCount() const { return 0; }
                virtual Color GetDebugColor() const { return Colors::LightGray; }
                #endif

            private:

                PoseType m_type;
            };
        }
    }
}