#pragma once
#include "Quaternion.h"

//-------------------------------------------------------------------------

namespace KRG::Math
{
    inline Vector CalculateAngularVelocity( Quaternion const& from, Quaternion const& to, float deltaTime )
    {
        KRG_ASSERT( deltaTime > 0 );

        Quaternion const deltaQuat( to.ToVector() - from.ToVector() );
        Quaternion orientationDelta = deltaQuat * from.GetConjugate();

        // Make sure we move along the shortest arc.
        Vector const dotToFrom = Quaternion::Dot( to, from );
        if ( dotToFrom.IsLessThan4( Vector::Zero ) )
        {
            orientationDelta.Negate();
        }

        Vector angularVelocity = Vector::Zero;
        angularVelocity = orientationDelta.ToVector() * Vector( deltaTime / 2 );
        angularVelocity.SetW0();
        return angularVelocity;
    }

    inline Vector CalculateLinearVelocity( Vector const& from, Vector const& to, float deltaTime )
    {
        KRG_ASSERT( deltaTime > 0 );
        return ( to - from ) / Vector( deltaTime );
    }
}