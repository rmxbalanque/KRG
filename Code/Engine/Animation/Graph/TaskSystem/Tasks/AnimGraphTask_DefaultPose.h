#pragma once

#include "../AnimGraphTask.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph::Tasks
{
    class DefaultPoseTask : public Task
    {

    public:

        DefaultPoseTask( int16 sourceNodeIdx, Pose::InitialState type );
        virtual void Execute( TaskContext const& context ) override;

        #if KRG_DEVELOPMENT_TOOLS
        virtual String GetDebugText() const override { return "Default Pose Task"; }
        virtual Color GetDebugColor() const { return Colors::LightGray; }
        #endif

    private:

        Pose::InitialState  m_type;
    };
}