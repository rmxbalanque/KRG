#pragma once

#include "Engine/Animation/Graph/Animation_RuntimeGraph_Task.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class CachedPoseWriteTask : public Task
    {
    public:

        CachedPoseWriteTask( NodeIndex sourceNodeIdx, TaskIndex sourceTaskIdx, UUID cachedPoseID );
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

        CachedPoseReadTask( NodeIndex sourceNodeIdx, UUID cachedPoseID );
        virtual void Execute( TaskContext const& context ) override;

        #if KRG_DEVELOPMENT_TOOLS
        virtual String GetDebugText() const override { return String( TEXT( "Read Cached Pose" ) ); }
        virtual Color GetDebugColor() const { return Colors::Red; }
        #endif

    private:

        UUID m_cachedPoseID;
    };
}