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
                    track.m_translationValueRangeX.GrowRange( translation.x );
                    track.m_translationValueRangeY.GrowRange( translation.y );
                    track.m_translationValueRangeZ.GrowRange( translation.z );

                    Vector const& scale = transform.GetScale();
                    track.m_scaleValueRange.GrowRange( scale.x );
                }
            }
        }
    }
}
#endif