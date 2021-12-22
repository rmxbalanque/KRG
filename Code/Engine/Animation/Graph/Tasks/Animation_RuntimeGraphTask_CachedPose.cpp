#include "Animation_RuntimeGraphTask_CachedPose.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    CachedPoseWriteTask::CachedPoseWriteTask( NodeIndex sourceNodeIdx, TaskIndex sourceTaskIdx, UUID cachedPoseID )
        : Task( sourceNodeIdx, UpdateStage::Any, { sourceTaskIdx } )
        , m_cachedPoseID( cachedPoseID )
    {
        KRG_ASSERT( sourceTaskIdx != InvalidIndex );
        KRG_ASSERT( m_cachedPoseID.IsValid() );
    }

    void CachedPoseWriteTask::Execute( TaskContext const& context )
    {
        auto pCachedPoseBuffer = context.m_posePool.GetCachedPoseBuffer( m_cachedPoseID );
        PoseBuffer const* pPoseBuffer = TransferDependencyPoseBuffer( context, 0 );
        KRG_ASSERT( pPoseBuffer->m_pose.IsPoseSet() );
        pCachedPoseBuffer->CopyFrom( pPoseBuffer );
        KRG_ASSERT( pCachedPoseBuffer->m_pose.IsPoseSet() );
        MarkTaskComplete( context );
    }

    //-------------------------------------------------------------------------

    CachedPoseReadTask::CachedPoseReadTask( NodeIndex sourceNodeIdx, UUID cachedPoseID )
        : Task( sourceNodeIdx )
        , m_cachedPoseID( cachedPoseID )
    {
        KRG_ASSERT( m_cachedPoseID.IsValid() );
    }

    void CachedPoseReadTask::Execute( TaskContext const& context )
    {
        auto pCachedPoseBuffer = context.m_posePool.GetCachedPoseBuffer( m_cachedPoseID );
        auto pPoseBuffer = GetNewPoseBuffer( context );

        // If the cached buffer contains a valid pose copy it
        if ( pCachedPoseBuffer->m_pose.IsPoseSet() )
        {
            pPoseBuffer->CopyFrom( pCachedPoseBuffer );
        }
        else // Set the result buffer to reference
        {
            pPoseBuffer->m_pose.Reset( Pose::InitialState::ReferencePose );
        }

        MarkTaskComplete( context );
    }
}