#include "Percentage.h"

//-------------------------------------------------------------------------

namespace KRG
{
    Percentage Percentage::Clamp( Percentage value, bool allowLooping )
    {
        Percentage clampedValue = value;
        if ( allowLooping )
        {
            F32 integerPortion;
            clampedValue = Percentage( Math::ModF( F32( value ), &integerPortion ) );
            if ( clampedValue < 0.0f ) // wrapping backwards
            {
                clampedValue = Percentage( 1.0f - clampedValue );
            }
        }
        else // Clamp
        {
            if ( value < 0.0f )
            {
                clampedValue = Percentage( 0.0f );
            }
            else if ( value > 1.0f )
            {
                clampedValue = Percentage( 1.0f );
            }
        }

        return clampedValue;
    }
}