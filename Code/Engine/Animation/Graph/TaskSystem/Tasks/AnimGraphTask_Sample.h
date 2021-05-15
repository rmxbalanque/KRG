#pragma once

#include "../AnimGraphTask.h"
#include "Engine/Animation/AnimationClip.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph::Tasks
{
    class SampleTask : public Task
    {

    public:

        SampleTask( int16 sourceNodeIdx, AnimationClip const* pAnimation, Percentage time );
        virtual void Execute( TaskContext const& context ) override;

        #if KRG_DEVELOPMENT_TOOLS
        virtual Color GetDebugColor() const { return Colors::SpringGreen; }
        virtual String GetDebugText() const override;
        #endif

    private:

        AnimationClip const*    m_pAnimation;
        Percentage              m_time;
    };
}