#pragma once

#include "Engine/Animation/TaskSystem/Animation_Task.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Tasks
{
    class CachedPoseWriteTask : public Task
    {
    public:

        CachedPoseWriteTask( TaskSourceID sourceID, TaskIndex sourceTaskIdx, UUID cachedPoseID );
        virtual void Execute( TaskContext const& context ) override;

        #if KRG_DEVELOPMENT_TOOLS
        virtual String GetDebugText() const override { return String( TEXT( "Write Cached Pose" ) ); }
        virtual Color GetDebugColor() const { return Colors::Red; }
        #endif

    private:

        UUID m_cachedPoseID;
    };

    //-------------------------------------------------------------------------

    class CachedPoseReadTask : public Task
    {

    public:

        CachedPoseReadTask( TaskSourceID sourceID, UUID cachedPoseID );
        virtual void Execute( TaskContext const& context ) override;

        #if KRG_DEVELOPMENT_TOOLS
        virtual String GetDebugText() const override { return String( TEXT( "Read Cached Pose" ) ); }
        virtual Color GetDebugColor() const { return Colors::Red; }
        #endif

    private:

        UUID m_cachedPoseID;
    };
}