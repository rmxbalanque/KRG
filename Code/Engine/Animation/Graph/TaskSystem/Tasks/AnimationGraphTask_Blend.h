#pragma once

#include "../AnimationGraphTask.h"
#include "Engine/Animation/AnimationBoneMask.h"
#include "Engine/Animation/AnimationBlender.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph::Tasks
{
    class BlendTask : public Task
    {

    public:

        BlendTask( NodeIndex sourceNodeIdx, TaskIndex sourceTaskIdx, TaskIndex targetTaskIdx, float const blendWeight, TBitFlags<PoseBlendOptions> const blendOptions, BoneMask const* pBoneMask = nullptr );
        virtual void Execute( TaskContext const& context ) override;

        #if KRG_DEVELOPMENT_TOOLS
        virtual String GetDebugText() const override { return String().sprintf( "Blend Task: %.2f", m_blendWeight ); }
        virtual Color GetDebugColor() const { return Colors::Red; }
        #endif

    private:

        BoneMask const*                         m_pBoneMask = nullptr;
        float                                   m_blendWeight = 0.0f;
        TBitFlags<PoseBlendOptions>             m_blendOptions;
    };
}