#pragma once

#include "System/Core/_Module/API.h"
#include "System/Core/Math/Matrix.h"
#include "Line.h"

//-------------------------------------------------------------------------

namespace KRG
{
    // Plane equation: a + b + c + d = 0
    //-------------------------------------------------------------------------

    class KRG_SYSTEM_CORE_API alignas( 16 ) Plane
    {
        KRG_SERIALIZE_MEMBERS( a, b, c, d );

    public:

        Plane() {}

        Plane( F32 a, F32 b, F32 c, F32 d )
        {
            m_data = _mm_set_ps( d, c, b, a );
            Normalize();
        }

        explicit Plane( Float3 const& point, Float3 const& normal ) : Plane( Vector( point ), Vector( normal ) ) {}
        explicit Plane( F32 distance, Float3 const& normal ) : Plane( distance, Vector( normal ) ) {}
        explicit Plane( Float3 const& point0, Float3 const& point1, Float3 const& point2 ) : Plane( Vector( point0 ), Vector( point1 ) , Vector( point2 ) ) {}
        explicit Plane( Float4 const& planeEquation ) { m_data = _mm_set_ps( planeEquation.w, planeEquation.z, planeEquation.y, planeEquation.x ); }

        explicit Plane( Vector const planeEquation ) : m_data( planeEquation.m_data ) {}

        explicit Plane( F32 const& distance, Vector const normal )
        {
            KRG_ASSERT( normal.IsNormalized3() );
            m_data = normal.m_data;
            d = distance;
        }

        explicit Plane( Vector const point, Vector const normal )
        {
            KRG_ASSERT( normal.IsNormalized3() );
            Vector const D = Vector::Dot3( point, normal ).GetNegated();
            AsVector() = Vector::Select( D, normal, Vector( 1, 1, 1, 0 ) );
        }

        explicit Plane( Vector const point0, Vector const point1, Vector const point2 )
        {
            Vector const V10 = point0 - point1;
            Vector const V20 = point0 - point2;
            Vector const normal = Vector::Cross3( V10, V20 ).GetNormalized3();
            Vector const D = Vector::Dot3( point0, normal ).GetNegated();
            AsVector() = Vector::Select( D, normal, Vector( 1, 1, 1, 0 ) );
        }

        inline Vector ToVector() const { return AsVector(); }
        inline Float4 ToFloat4() const { return AsVector().ToFloat4(); }
        inline Vector GetNormal() const { return Vector( a, b, c, 0.0f ); }

        inline bool IsNormalized() const { return AsVector().IsNormalized3(); }

        Plane& Normalize()
        {
            auto vLengthSq = _mm_mul_ps( *this, *this );
            auto vTemp = _mm_shuffle_ps( vLengthSq, vLengthSq, _MM_SHUFFLE( 2, 1, 2, 1 ) );
            vLengthSq = _mm_add_ss( vLengthSq, vTemp );
            vTemp = _mm_shuffle_ps( vTemp, vTemp, _MM_SHUFFLE( 1, 1, 1, 1 ) );
            vLengthSq = _mm_add_ss( vLengthSq, vTemp );
            vLengthSq = _mm_shuffle_ps( vLengthSq, vLengthSq, _MM_SHUFFLE( 0, 0, 0, 0 ) );

            // Prepare for the division
            auto vResult = _mm_sqrt_ps( vLengthSq );
            // Failsafe on zero (Or epsilon) length planes
            // If the length is infinity, set the elements to zero
            vLengthSq = _mm_cmpneq_ps( vLengthSq, Vector::Infinity );
            // Reciprocal mul to perform the normalization
            vResult = _mm_div_ps( *this, vResult );
            // Any that are infinity, set to zero
            *this = _mm_and_ps( vResult, vLengthSq );
            return *this;
        }

        inline Plane GetNormalized() const
        {
            Plane p = *this;
            p.Normalize();
            return p;
        }

        inline Plane& operator*=( Matrix const& transform )
        {
            auto W = AsVector().GetSplatW();
            auto Z = AsVector().GetSplatZ();
            auto Y = AsVector().GetSplatY();
            auto X = AsVector().GetSplatX();

            Vector result = W * transform[3];
            Vector::MultiplyAdd( Z, transform[2], result );
            Vector::MultiplyAdd( Y, transform[1], result );
            Vector::MultiplyAdd( X, transform[0], result );
            AsVector() = result;
            return *this;
        }

        inline Plane operator*( Matrix const& transform ) const
        {
            Plane p = *this;
            p *= transform;
            return p;
        }

        //-------------------------------------------------------------------------

        inline Vector ProjectPoint( Vector const point ) const
        {
            KRG_ASSERT( point.w == 1.0f && IsNormalized() );
            auto const planeVector = ToVector();
            auto const planeNormal = GetNormal();
            auto const distanceToPlane = Vector::Dot4( point, planeVector );
            auto const projectedPoint = Vector::MultiplyAdd( planeNormal.GetNegated(), distanceToPlane, point );
            return projectedPoint;
        }

        //-------------------------------------------------------------------------

        inline Vector SignedDistanceToPoint( Vector const point ) const
        {
            KRG_ASSERT( point.w == 1.0f && IsNormalized() );
            return Vector::Dot4( point, ToVector() );
        }

        inline float GetSignedDistanceToPoint( Vector const point ) const 
        { 
            return SignedDistanceToPoint( point ).ToFloat();
        }

        inline Vector AbsoluteDistanceToPoint( Vector const point ) const 
        { 
            return SignedDistanceToPoint( point ).Abs();
        }

        inline float GetAbsoluteDistanceToPoint( Vector const point ) const 
        {
            return AbsoluteDistanceToPoint( point ).ToFloat();
        }

        inline bool ArePointsOnSameSide( Vector const point0, Vector const point1 ) const 
        { 
            Vector const distanceProduct = ( SignedDistanceToPoint( point0 ) * SignedDistanceToPoint( point1 ) );
            return distanceProduct.IsGreaterThanEqual4( Vector::Zero );
        }

        inline bool IsPointInFront( Vector const point ) const 
        {
            return SignedDistanceToPoint( point ).IsGreaterThanEqual4( Vector::Zero );
        }

        inline bool IsPointBehind( Vector const point ) const 
        { 
            return SignedDistanceToPoint( point ).IsLessThan4( Vector::Zero );
        }

        //-------------------------------------------------------------------------

        bool IntersectLine( Vector const& point0, Vector const& point1, Vector& intersectionPoint ) const;
        bool IntersectLine( LineSegment const& line, Vector& intersectionPoint ) const { return IntersectLine( line.GetStartPoint(), line.GetEndPoint(), intersectionPoint ); }
        bool IntersectRay( Vector const& rayOrigin, Vector const& rayDirection, Vector& intersectionPoint ) const;
        bool IntersectPlane( Plane const& otherPlane, Vector& outLineStart, Vector& outLineEnd ) const;

    private:

        inline Vector& AsVector() { return reinterpret_cast<Vector&>( *this ); }
        inline Vector const& AsVector() const { return reinterpret_cast<Vector const&>( *this ); }

        KRG_FORCE_INLINE operator __m128&() { return m_data; }
        KRG_FORCE_INLINE operator __m128 const&() const { return m_data; }
        inline Plane& operator=( __m128 v ) { m_data = v; return *this; }

    public:

        union
        {
            struct { F32 a,b,c,d; };
            __m128 m_data;
        };
    };

    //-------------------------------------------------------------------------

    static_assert( sizeof( Vector ) == 16, "Plane size must be 16 bytes!" );
}