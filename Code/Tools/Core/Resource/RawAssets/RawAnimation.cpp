#include "RawAnimation.h"

//-------------------------------------------------------------------------

namespace KRG::RawAssets
{
    void RawAnimation::Finalize()
    {
        KRG_ASSERT( m_numFrames > 0 );

        // Root Motion
        //-------------------------------------------------------------------------

        TrackData& rootTrackData = m_tracks[0];
        Vector rootMotionStartPosition = rootTrackData.m_transforms[0].GetTranslation();

        for ( uint32 i = 0; i < m_numFrames; i++ )
        {
            if ( !rootTrackData.m_transforms[i].GetScale().IsEqual3( Vector::One ) )
            {
                LogError( "Root bone has scaling. This is not supported!" );
                return;
            }

            m_rootMotionDeltas.emplace_back( rootTrackData.m_transforms[i] );
            m_rootMotionDeltas.back().SetTranslation( m_rootMotionDeltas.back().GetTranslation() - rootMotionStartPosition );
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