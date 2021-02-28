#pragma once

#include "../AnimTask.h"
#include "Engine/Animation/AnimationBoneMask.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        namespace Tasks
        {
            class BlendTask : public Task
            {

            public:

                BlendTask( S32 sourceNodeIdx, F32 const blendWeight, U32 const blendOptions, BoneMask const* pBoneMask = nullptr );
                virtual void Execute( TaskPoseStack& poseStack ) const override;

                #if KRG_DEVELOPMENT_TOOLS
                virtual String GetDebugText() const override { return String().sprintf( "Blend Task: %.2f", m_blendWeight ); }
                virtual S32 GetDependencyCount() const { return 2; }
                virtual Color GetDebugColor() const { return Colors::Red; }
                #endif

            private:

                BoneMask const*     m_pBoneMask;
                F32                 m_blendWeight;
                U32                m_blendOptions;
            };
        }
    }

}