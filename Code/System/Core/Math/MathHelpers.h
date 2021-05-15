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

    //-------------------------------------------------------------------------

    // Return the angle between the two vectors in the XY plane
    KRG_FORCE_INLINE Radians CalculateHorizontalAngleBetweenVectors( Vector const& a, Vector const& b )
    {
        return Vector::AngleBetweenVectors( a.GetNormalized2(), b.GetNormalized2() ).ToFloat();
    }

    // Return the angle between the two vectors in the YZ plane
    KRG_FORCE_INLINE Radians CalculateVerticalAngleBetweenVectors( Vector const& a, Vector const& b )
    {
        Vector const aa = Vector::Select( a, Vector::Zero, Vector::Select0110 ).GetNormalized3();
        Vector const bb = Vector::Select( b, Vector::Zero, Vector::Select0110 ).GetNormalized3();
        return Vector::AngleBetweenVectors( aa, bb ).ToFloat();
    }
}