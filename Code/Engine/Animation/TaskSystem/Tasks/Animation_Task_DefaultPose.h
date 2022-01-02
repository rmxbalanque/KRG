#pragma once

#include "Engine/Animation/TaskSystem/Animation_Task.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Tasks
{
    class DefaultPoseTask : public Task
    {

    public:

        DefaultPoseTask( TaskSourceID sourceID, Pose::InitialState type );
        virtual void Execute( TaskContext const& context ) override;

        #if KRG_DEVELOPMENT_TOOLS
        virtual String GetDebugText() const override { return "Default Pose Task"; }
        virtual Color GetDebugColor() const { return Colors::LightGray; }
        #endif

    private:

        Pose::InitialState  m_type;
    };
}