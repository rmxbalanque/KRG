#include "AnimationGraphTask_Blend.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph::Tasks
{
    BlendTask::BlendTask( NodeIndex sourceNodeIdx, TaskIndex sourceTaskIdx, TaskIndex targetTaskIdx, float const blendWeight, TBitFlags<PoseBlendOptions> blendOptions, BoneMask const* pBoneMask )
        : Task( sourceNodeIdx, UpdateStage::Any, { sourceTaskIdx, targetTaskIdx } )
        , m_blendWeight( blendWeight )
        , m_pBoneMask( pBoneMask )
        , m_blendOptions( blendOptions )
    {
        KRG_ASSERT( m_blendWeight >= 0.0f && m_blendWeight <= 1.0f );

        // Ensure that blend weights very close to 1.0f are set to 1.0f since the blend code will optimize away unnecessary blend operations
        if ( Math::IsNearEqual( m_blendWeight, 1.0f ) )
        {
            m_blendWeight = 1.0f;
        }
    }

    void BlendTask::Execute( TaskContext const& context )
    {
        auto pSourceBuffer = TransferDependencyPoseBuffer( context, 0 );
        auto pTargetBuffer = AccessDependencyPoseBuffer( context, 1 );
        auto pFinalBuffer = pSourceBuffer;

        // Ensure the global pose is created as it will be needed during the blend
        if ( m_blendOptions.IsFlagSet( PoseBlendOptions::LockUnmaskedBones ) )
        {
            PoseBuffer* pTemporaryBuffer = nullptr;
            int8 tempBufferIdx = GetTemporaryPoseBuffer( context, pTemporaryBuffer );
            KRG_ASSERT( pTemporaryBuffer != nullptr );

            // We need to make a copy of the pose since the lock-unmasked bones requires that the source and the target are not the same pose
            pTemporaryBuffer->m_pose.CopyFrom( pSourceBuffer->m_pose );
            pTemporaryBuffer->m_pose.CalculateGlobalTransforms();
            Blender::Blend( &pTemporaryBuffer->m_pose, &pTargetBuffer->m_pose, m_blendWeight, m_blendOptions, m_pBoneMask, &pFinalBuffer->m_pose );

            ReleaseTemporaryPoseBuffer( context, tempBufferIdx );
        }
        else
        {
            Blender::Blend( &pSourceBuffer->m_pose, &pTargetBuffer->m_pose, m_blendWeight, m_blendOptions, m_pBoneMask, &pFinalBuffer->m_pose );
        }

        // Release source result
        ConsumePoseBuffer( context, 1 );
        MarkTaskComplete( context );
    }
}