#pragma once

#include "../AnimTask.h"
#include "Engine/Animation/AnimationBoneMask.h"
#include "Engine/Animation/AnimationBlender.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Tasks
{
    class BlendTask : public Task
    {

    public:

        BlendTask( int32 sourceNodeIdx, float const blendWeight, TBitFlags<Blender::PoseBlendOptions> const blendOptions, BoneMask const* pBoneMask = nullptr );
        virtual void Execute( TaskPoseStack& poseStack ) const override;

        #if KRG_DEVELOPMENT_TOOLS
        virtual String GetDebugText() const override { return String().sprintf( "Blend Task: %.2f", m_blendWeight ); }
        virtual int32 GetDependencyCount() const { return 2; }
        virtual Color GetDebugColor() const { return Colors::Red; }
        #endif

    private:

        BoneMask const*                         m_pBoneMask = nullptr;
        float                                   m_blendWeight = 0.0f;
        TBitFlags<Blender::PoseBlendOptions>    m_blendOptions;
    };
}