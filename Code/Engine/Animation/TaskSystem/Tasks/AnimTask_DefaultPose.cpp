#include "AnimTask_DefaultPose.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        namespace Tasks
        {
            DefaultPoseTask::DefaultPoseTask( S32 sourceNodeIdx, PoseType type )
                : Task( sourceNodeIdx )
                , m_type( type )
            {}

            void DefaultPoseTask::Execute( TaskPoseStack& poseStack ) const
            {
                auto pPose = poseStack.PushPose();

                if ( m_type == PoseType::Zero )
                {
                    pPose->SetZeroPose();
                }
                else
                {
                    pPose->SetReferencePose();
                }

                #if KRG_DEVELOPMENT_TOOLS
                poseStack.RecordDebugPose( pPose );
                #endif
            }
        }
    }

}