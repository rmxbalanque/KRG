#ifdef _WIN32
#include "RawAnimation.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace RawAssets
    {
        void RawAnimation::Finalize()
        {
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
}
#endif