#include "AnimationData.h"
#include "AnimationPose.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        FrameTime AnimationData::GetFrameTime( Percentage percentageThrough ) const
        {
            FrameTime frameTime;

            Percentage const clampedTime = percentageThrough.GetClamped( false );
            if ( clampedTime == 1.0f )
            {
                frameTime = FrameTime( (S32) m_numFrames - 1 );
            }
            // Find time range in key list
            else if ( clampedTime != 0.0f )
            {
                frameTime = FrameTime( percentageThrough, m_numFrames - 1 );
            }

            return frameTime;
        }

        //-------------------------------------------------------------------------

        void AnimationData::GetPose( FrameTime const& frameTime, Pose* pOutPose ) const
        {
            KRG_ASSERT( IsValid() );
            KRG_ASSERT( pOutPose != nullptr && pOutPose->GetSkeleton() == m_pSkeleton.GetPtr() );
            KRG_ASSERT( frameTime.GetFrameIndex() < m_numFrames );

            pOutPose->ClearGlobalTransforms();

            //-------------------------------------------------------------------------

            Transform boneTransform;
            U16 const* pTrackData = m_compressedPoseData.data();

            // Read exact key frame
            if ( frameTime.IsExactlyAtKeyFrame() )
            {
                auto const numBones = m_pSkeleton->GetNumBones();
                for ( auto boneIdx = 0; boneIdx < numBones; boneIdx++ )
                {
                    pTrackData = ReadCompressedTrackKeyFrame( pTrackData, m_trackCompressionSettings[boneIdx], frameTime.GetFrameIndex(), boneTransform );
                    pOutPose->SetTransform( boneIdx, boneTransform );
                }
            }
            else // Read interpolated anim pose
            {
                auto const numBones = m_pSkeleton->GetNumBones();
                for ( auto boneIdx = 0; boneIdx < numBones; boneIdx++ )
                {
                    pTrackData = ReadCompressedTrackTransform( pTrackData, m_trackCompressionSettings[boneIdx], frameTime, boneTransform );
                    pOutPose->SetTransform( boneIdx, boneTransform );
                }
            }
        }

        Transform AnimationData::GetDisplacementTransform( FrameTime const& frameTime ) const
        {
            KRG_ASSERT( IsValid() );
            KRG_ASSERT( frameTime.GetFrameIndex() < m_numFrames );

            Transform displacementTransform;

            if ( m_displacementTrack.empty() )
            {
                displacementTransform = Transform::Identity;
            }
            else 
            {
                if ( frameTime.IsExactlyAtKeyFrame() )
                {
                    displacementTransform = m_displacementTrack[frameTime.GetFrameIndex()];
                }
                else // Read interpolated transform
                {
                    Transform const& frameStartTransform = m_displacementTrack[frameTime.GetFrameIndex()];
                    Transform const& frameEndTransform = m_displacementTrack[frameTime.GetFrameIndex() + 1];
                    displacementTransform = Transform::Slerp( frameStartTransform, frameEndTransform, frameTime.GetPercentageThrough() );
                }
            }

            return displacementTransform;
        }

        Transform AnimationData::GetLocalSpaceTransform( S32 boneIdx, FrameTime const& frameTime ) const
        {
            KRG_ASSERT( IsValid() && m_pSkeleton->IsValidBoneIndex( boneIdx ) );

            U32 frameIdx = frameTime.GetFrameIndex();
            KRG_ASSERT( frameIdx < m_numFrames );

            //-------------------------------------------------------------------------

            auto const& trackSettings = m_trackCompressionSettings[boneIdx];
            U16 const* pTrackData = m_compressedPoseData.data() + trackSettings.m_trackStartIndex;

            //-------------------------------------------------------------------------

            Transform boneLocalTransform;

            if ( frameTime.IsExactlyAtKeyFrame() )
            {
                ReadCompressedTrackKeyFrame( pTrackData, trackSettings, frameIdx, boneLocalTransform );
            }
            else
            {
                ReadCompressedTrackTransform( pTrackData, trackSettings, frameTime, boneLocalTransform );
            }
            return boneLocalTransform;
        }

        Transform AnimationData::GetGlobalSpaceTransform( S32 boneIdx, FrameTime const& frameTime ) const
        {
            KRG_ASSERT( IsValid() && m_pSkeleton->IsValidBoneIndex( boneIdx ) );

            U32 frameIdx = frameTime.GetFrameIndex();
            KRG_ASSERT( frameIdx < m_numFrames );

            // Find all parent bones
            //-------------------------------------------------------------------------

            TInlineVector<S32, 20> boneHierarchy;
            boneHierarchy.emplace_back( boneIdx );

            S32 parentBoneIdx = m_pSkeleton->GetParentIndex( boneIdx );
            while ( parentBoneIdx != InvalidIndex )
            {
                boneHierarchy.emplace_back( parentBoneIdx );
                parentBoneIdx = m_pSkeleton->GetParentIndex( parentBoneIdx );
            }

            // Calculate the global transform
            //-------------------------------------------------------------------------

            Transform globalTransform;

            if ( frameTime.IsExactlyAtKeyFrame() )
            {
                // Read root transform
                {
                    auto const& trackSettings = m_trackCompressionSettings[boneHierarchy.back()];
                    U16 const* pTrackData = m_compressedPoseData.data() + trackSettings.m_trackStartIndex;
                    ReadCompressedTrackKeyFrame( pTrackData, trackSettings, frameIdx, globalTransform );
                }

                // Read and multiply out all the transforms moving down the hierarchy
                Transform localTransform;
                for ( S32 i = (S32) boneHierarchy.size() - 2; i >= 0; i-- )
                {
                    S32 const trackIdx = boneHierarchy[i];
                    auto const& trackSettings = m_trackCompressionSettings[trackIdx];
                    U16 const* pTrackData = m_compressedPoseData.data() + trackSettings.m_trackStartIndex;
                    ReadCompressedTrackKeyFrame( pTrackData, trackSettings, frameIdx, localTransform );

                    globalTransform = localTransform * globalTransform;
                }
            }
            else // Interpolate key-frames
            {
                // Read root transform
                {
                    auto const& trackSettings = m_trackCompressionSettings[boneHierarchy.back()];
                    U16 const* pTrackData = m_compressedPoseData.data() + trackSettings.m_trackStartIndex;
                    ReadCompressedTrackTransform( pTrackData, trackSettings, frameTime, globalTransform );
                }

                // Read and multiply out all the transforms moving down the hierarchy
                Transform localTransform;
                for ( S32 i = (S32) boneHierarchy.size() - 2; i >= 0; i-- )
                {
                    S32 const trackIdx = boneHierarchy[i];
                    auto const& trackSettings = m_trackCompressionSettings[trackIdx];
                    U16 const* pTrackData = m_compressedPoseData.data() + trackSettings.m_trackStartIndex;
                    ReadCompressedTrackTransform( pTrackData, trackSettings, frameTime, localTransform );

                    globalTransform = localTransform * globalTransform;
                }
            }

            return globalTransform;
        }
    }
}