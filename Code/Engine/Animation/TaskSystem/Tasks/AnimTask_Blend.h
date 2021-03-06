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

                BlendTask( int32 sourceNodeIdx, float const blendWeight, uint32 const blendOptions, BoneMask const* pBoneMask = nullptr );
                virtual void Execute( TaskPoseStack& poseStack ) const override;

                #if KRG_DEVELOPMENT_TOOLS
                virtual String GetDebugText() const override { return String().sprintf( "Blend Task: %.2f", m_blendWeight ); }
                virtual int32 GetDependencyCount() const { return 2; }
                virtual Color GetDebugColor() const { return Colors::Red; }
                #endif

            private:

                BoneMask const*     m_pBoneMask;
                float                 m_blendWeight;
                uint32                m_blendOptions;
            };
        }
    }

}