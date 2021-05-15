#pragma once

#include "../AnimGraphTask.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph::Tasks
{
    class CachedPoseWriteTask : public Task
    {
    public:

        CachedPoseWriteTask( int16 sourceNodeIdx, int8 sourceTaskIdx, UUID cachedPoseID );
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

        CachedPoseReadTask( int16 sourceNodeIdx, UUID cachedPoseID );
        virtual void Execute( TaskContext const& context ) override;

        #if KRG_DEVELOPMENT_TOOLS
        virtual String GetDebugText() const override { return String( TEXT( "Read Cached Pose" ) ); }
        virtual Color GetDebugColor() const { return Colors::Red; }
        #endif

    private:

        UUID m_cachedPoseID;
    };
}