#include "AnimationClip.h"
#include "AnimationPose.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        FrameTime AnimationClip::GetFrameTime( Percentage percentageThrough ) const
        {
            FrameTime frameTime;

            Percentage const clampedTime = percentageThrough.GetClamped( false );
            if ( clampedTime == 1.0f )
            {
                frameTime = FrameTime( (int32) m_numFrames - 1 );
            }
            // Find time range in key list
            else if ( clampedTime != 0.0f )
            {
                frameTime = FrameTime( percentageThrough, m_numFrames - 1 );
            }

            return frameTime;
        }

        //-------------------------------------------------------------------------

        void AnimationClip::GetPose( FrameTime const& frameTime, Pose* pOutPose ) const
        {
            KRG_ASSERT( IsValid() );
            KRG_ASSERT( pOutPose != nullptr && pOutPose->GetSkeleton() == m_pSkeleton.GetPtr() );
            KRG_ASSERT( frameTime.GetFrameIndex() < m_numFrames );

            pOutPose->ClearGlobalTransforms();

            //-------------------------------------------------------------------------

            Transform boneTransform;
            uint16 const* pTrackData = m_compressedPoseData.data();

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

        Transform AnimationClip::GetDisplacementTransform( FrameTime const& frameTime ) const
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

        Transform AnimationClip::GetLocalSpaceTransform( int32 boneIdx, FrameTime const& frameTime ) const
        {
            KRG_ASSERT( IsValid() && m_pSkeleton->IsValidBoneIndex( boneIdx ) );

            uint32 frameIdx = frameTime.GetFrameIndex();
            KRG_ASSERT( frameIdx < m_numFrames );

            //-------------------------------------------------------------------------

            auto const& trackSettings = m_trackCompressionSettings[boneIdx];
            uint16 const* pTrackData = m_compressedPoseData.data() + trackSettings.m_trackStartIndex;

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

        Transform AnimationClip::GetGlobalSpaceTransform( int32 boneIdx, FrameTime const& frameTime ) const
        {
            KRG_ASSERT( IsValid() && m_pSkeleton->IsValidBoneIndex( boneIdx ) );

            uint32 frameIdx = frameTime.GetFrameIndex();
            KRG_ASSERT( frameIdx < m_numFrames );

            // Find all parent bones
            //-------------------------------------------------------------------------

            TInlineVector<int32, 20> boneHierarchy;
            boneHierarchy.emplace_back( boneIdx );

            int32 parentBoneIdx = m_pSkeleton->GetParentIndex( boneIdx );
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
                    uint16 const* pTrackData = m_compressedPoseData.data() + trackSettings.m_trackStartIndex;
                    ReadCompressedTrackKeyFrame( pTrackData, trackSettings, frameIdx, globalTransform );
                }

                // Read and multiply out all the transforms moving down the hierarchy
                Transform localTransform;
                for ( int32 i = (int32) boneHierarchy.size() - 2; i >= 0; i-- )
                {
                    int32 const trackIdx = boneHierarchy[i];
                    auto const& trackSettings = m_trackCompressionSettings[trackIdx];
                    uint16 const* pTrackData = m_compressedPoseData.data() + trackSettings.m_trackStartIndex;
                    ReadCompressedTrackKeyFrame( pTrackData, trackSettings, frameIdx, localTransform );

                    globalTransform = localTransform * globalTransform;
                }
            }
            else // Interpolate key-frames
            {
                // Read root transform
                {
                    auto const& trackSettings = m_trackCompressionSettings[boneHierarchy.back()];
                    uint16 const* pTrackData = m_compressedPoseData.data() + trackSettings.m_trackStartIndex;
                    ReadCompressedTrackTransform( pTrackData, trackSettings, frameTime, globalTransform );
                }

                // Read and multiply out all the transforms moving down the hierarchy
                Transform localTransform;
                for ( int32 i = (int32) boneHierarchy.size() - 2; i >= 0; i-- )
                {
                    int32 const trackIdx = boneHierarchy[i];
                    auto const& trackSettings = m_trackCompressionSettings[trackIdx];
                    uint16 const* pTrackData = m_compressedPoseData.data() + trackSettings.m_trackStartIndex;
                    ReadCompressedTrackTransform( pTrackData, trackSettings, frameTime, localTransform );

                    globalTransform = localTransform * globalTransform;
                }
            }

            return globalTransform;
        }
    }
}