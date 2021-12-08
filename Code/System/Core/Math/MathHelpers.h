#pragma once
#include "Quaternion.h"
#include "Plane.h"

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

    inline bool IsVectorToTheLeft( Vector const& reference, Vector const& v, Vector const& upAxis = Vector::UnitZ )
    {
        Vector const nr = reference.GetNormalized3();
        Vector const nv = v.GetNormalized3();
        Vector const cross = Vector::Cross3( nr, nv );
        Vector const dot = Vector::Dot3( cross, upAxis );
        return dot.ToFloat() < 0.0f;
    }

    inline bool IsVectorToTheRight( Vector const& reference, Vector const& v, Vector const& upAxis = Vector::UnitZ )
    {
        Vector const nr = reference.GetNormalized3();
        Vector const nv = v.GetNormalized3();
        Vector const cross = Vector::Cross3( nr, nv );
        Vector const dot = Vector::Dot3( cross, upAxis );
        return dot.ToFloat() > 0.0f;
    }

    inline bool IsVectorToTheLeft2D( Vector const& reference, Vector const& v )
    {
        Vector const nr = reference.GetNormalized2();
        Vector const nv = v.GetNormalized2();
        Vector const cross = Vector::Cross3( nr, nv );
        Vector const dot = Vector::Dot3( cross, Vector::UnitZ );
        return dot.ToFloat() < 0.0f;
    }

    inline bool IsVectorToTheRight2D( Vector const& reference, Vector const& v )
    {
        Vector const nr = reference.GetNormalized2();
        Vector const nv = v.GetNormalized2();
        Vector const cross = Vector::Cross3( nr, nv );
        Vector const dot = Vector::Dot3( cross, Vector::UnitZ );
        return dot.ToFloat() > 0.0f;
    }

    inline bool AreVectorsParallel2D( Vector const& a, Vector const& b )
    {
        Vector const na = a.GetNormalized2();
        Vector const nb = b.GetNormalized2();
        Vector const dot = Vector::Dot3( na, nb );
        return dot.ToFloat() > 0.0f;
    }

    inline bool IsVectorInTheOppositeDirection2D( Vector const& reference, Vector const& v )
    {
        Vector const nr = reference.GetNormalized2();
        Vector const nv = v.GetNormalized2();
        Vector const dot = Vector::Dot3( nr, nv );
        return dot.ToFloat() < 0.0f;
    }

    inline bool AreVectorsOrthogonal2D( Vector const& reference, Vector const& v )
    {
        Vector const nr = reference.GetNormalized2();
        Vector const nv = v.GetNormalized2();
        Vector const dot = Vector::Dot3( nr, nv );
        return dot.IsNearZero4( 0.0f );
    }

    // Returns the shortest angle between two vectors (always positive)
    inline Radians GetAngleBetweenNormalizedVectors( Vector const& a, Vector const& b )
    {
        KRG_ASSERT( !a.IsNearZero3() && !b.IsNearZero3() );

        if ( a.IsNearEqual3( b ) )
        {
            return Radians( 0.0f );
        }

        Vector const dot = Vector::Dot3( a, b );
        Vector const reciprocal = a.InverseLength3() * b.InverseLength3();
        Vector const CosAngle = Vector::Clamp( dot * reciprocal, Vector::NegativeOne, Vector::One );
        return Radians( Vector::ACos( CosAngle ).ToFloat() );
    }

    // Returns the shortest angle between two vectors (always positive)
    KRG_FORCE_INLINE Radians GetAngleBetweenVectors( Vector const& a, Vector const& b )
    {
        return GetAngleBetweenNormalizedVectors( a.GetNormalized3(), b.GetNormalized3() );
    }

    // Returns the angle between two 2D vectors, relative to the reference vector (i.e. negative values means to the left of reference, positive means to the right)
    inline Radians GetYawAngleBetweenVectors( Vector const& reference, Vector const& v )
    {
        Vector const nr = reference.GetNormalized2();
        Vector const nv = v.GetNormalized2();
        Radians horizontalAngle = GetAngleBetweenVectors( nr, nv );

        Vector const cross = Vector::Cross3( nr, nv );
        Vector const dot = Vector::Dot3( cross, Vector::UnitZ );
        if ( dot.ToFloat() < 0.0f )
        {
            horizontalAngle = -horizontalAngle;
        }

        return horizontalAngle;
    }

    // Returns the angle between two 2D vectors, relative to the reference vector (i.e. negative values mean below the reference, positive means above )
    inline Radians GetPitchAngleBetweenNormalizedVectors( Vector const& reference, Vector const& v )
    {
        return Radians( Math::ASin( v.m_z ) - Math::ASin( reference.m_z ) );
    }

    // Returns the angle between two 2D vectors, relative to the reference vector (i.e. negative values mean below the reference, positive means above )
    inline Radians GetPitchAngleBetweenVectors( Vector const& reference, Vector const& v )
    {
        Vector const nr = reference.GetNormalized3();
        Vector const nv = v.GetNormalized3();
        return GetPitchAngleBetweenNormalizedVectors( nr, nv );
    }
}