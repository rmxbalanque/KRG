#include "AnimTask_Blend.h"
#include "Engine/Animation/Operations/BlendOps.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        namespace Tasks
        {
            BlendTask::BlendTask( int32 sourceNodeIdx, float const blendWeight, uint32 const blendOptions, BoneMask const* pBoneMask )
                : Task( sourceNodeIdx )
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

            void BlendTask::Execute( TaskPoseStack& poseStack ) const
            {
                // Create a temp source pose
                poseStack.PushPose();
                auto pSourcePose = poseStack.PopPose();

                // The blended poses - pose0 will be the result pose
                auto pTargetPose = poseStack.PopPose();
                auto pResultPose = poseStack.GetPose();
                pSourcePose->CopyFrom( pResultPose );
                pResultPose->ClearGlobalTransforms();

                // Ensure the global pose is cached as it will be needed during the blend
                if ( m_blendOptions & Blender::Options::LockUnmaskedBones )
                {
                    pSourcePose->CalculateGlobalTransforms();

                    // We need to ensure that pose 0 is not the same as the result without paying the cost of a copy
                    // So we switch the arguments around and invert the blend weight
                    Blender::Blend( pSourcePose, pTargetPose, m_blendWeight, m_blendOptions, m_pBoneMask, pResultPose );
                }
                else
                {
                    Blender::Blend( pSourcePose, pTargetPose, m_blendWeight, m_blendOptions, m_pBoneMask, pResultPose );
                }

                #if KRG_DEVELOPMENT_TOOLS
                poseStack.RecordDebugPose( pResultPose );
                #endif
            }
        }
    }

}