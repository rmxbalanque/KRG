#include "RawAnimation.h"

//-------------------------------------------------------------------------

namespace KRG::RawAssets
{
    void RawAnimation::Finalize()
    {
        KRG_ASSERT( m_numFrames > 0 );

        // Get all immediate children of the root
        //-------------------------------------------------------------------------

        int32 const numBones = GetNumBones();
        TVector<int32> immediateChildrenOfRoot;
        for ( auto i = 0; i < numBones; i++ )
        {
            if ( m_skeleton.GetParentBoneIndex( i ) == 0 )
            {
                immediateChildrenOfRoot.emplace_back( i );
            }
        }

        // Root Motion
        //-------------------------------------------------------------------------

        TrackData& rootTrackData = m_tracks[0];
        Vector rootMotionOriginOffset = rootTrackData.m_transforms[0].GetTranslation(); // Ensure that the root motion always starts at the origin

        for ( uint32 i = 0; i < m_numFrames; i++ )
        {
            // If we detect scaling on the root, log an error and exit
            if ( rootTrackData.m_transforms[i].HasScale() )
            {
                LogError( "Root scaling detected! This is not allowed, please remove all scaling from the root bone!" );
                return;
            }

            // Extract root position and remove the origin offset from it
            m_rootPositions.emplace_back( rootTrackData.m_transforms[i] );
            m_rootPositions.back().SetTranslation( m_rootPositions.back().GetTranslation() - rootMotionOriginOffset );
            rootTrackData.m_transforms[i] = Transform::Identity;
        }

        // Calculate component ranges
        //-------------------------------------------------------------------------

        for ( auto& track : m_tracks )
        {
            for ( auto const& transform : track.m_transforms )
            {
                Vector const& translation = transform.GetTranslation();
                track.m_translationValueRangeX.GrowRange( translation.m_x );
                track.m_translationValueRangeY.GrowRange( translation.m_y );
                track.m_translationValueRangeZ.GrowRange( translation.m_z );

                Vector const& scale = transform.GetScale();
                track.m_scaleValueRangeX.GrowRange( scale.m_x );
                track.m_scaleValueRangeY.GrowRange( scale.m_y );
                track.m_scaleValueRangeZ.GrowRange( scale.m_z );
            }
        }
    }
}