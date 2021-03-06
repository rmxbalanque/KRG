#pragma once

#include "System/Core/Math/Matrix.h"

//-------------------------------------------------------------------------

namespace KRG
{
    //-------------------------------------------------------------------------
    // Line - start point and infinite length in both directions
    //-------------------------------------------------------------------------

    class Line
    {
        friend class Ray;

    public:

        static inline Line FromTwoPoints( Vector const& startPoint, Vector const& endPoint )
        {
            return Line( startPoint, endPoint );
        }
        
        static inline Line FromPointAndDirection( Vector const& startPoint, Vector const& direction )
        {
            KRG_ASSERT( !direction.IsNearZero3() );
            Line line;
            line.m_startPoint = startPoint;
            line.m_direction = direction.GetAsVector();
            return line;
        }

    public:

        Line( Vector const& startPoint, Vector const& endPoint )
            : m_startPoint( startPoint.GetAsPoint() )
        {
            KRG_ASSERT( !startPoint.IsNearEqual3( endPoint ) );
            m_direction = ( endPoint - startPoint ).GetNormalized3().MakeVector();
        }

        inline Vector GetStartPoint() const { return m_startPoint; }
        inline Vector GetDirection() const { return m_direction; }

        //-------------------------------------------------------------------------

        inline float ScalarProjectionOnLine( Vector const point ) const
        {
            auto const dot = Vector::Dot3( ( point - m_startPoint ), m_direction );
            return dot.ToFloat();
        }

        inline Vector GetPointAlongLine( float distanceFromStartPoint ) const 
        { 
            return Vector::MultiplyAdd( m_direction, Vector( distanceFromStartPoint ), m_startPoint );
        }

        inline Vector VectorProjectionOnLine( Vector const point, float& outScalarResolute ) const
        {
            outScalarResolute = ScalarProjectionOnLine( point );
            return GetPointAlongLine( outScalarResolute );
        }

        inline Vector GetClosestPointOnLine( Vector const& point ) const
        {
            float scalarResolute;
            return VectorProjectionOnLine( point, scalarResolute );
        }

        inline Vector GetDistanceAlongLine( Vector const& point ) const
        {
            return Vector( ScalarProjectionOnLine( point ) );
        }

        inline float GetDistanceBetweenLineAndPoint( Vector const& point ) const
        {
            Vector const closestPointOnLine = GetClosestPointOnLine( point );
            return closestPointOnLine.GetDistance3( point );
        }

    protected:

        Line() = default;

    protected:

        Vector m_startPoint;
        Vector m_direction;
    };

    //-------------------------------------------------------------------------
    // A directed line segment
    //-------------------------------------------------------------------------

    class LineSegment : public Line
    {
        friend class Ray;

    public:

        LineSegment( Vector startPoint, Vector endPoint )
            : Line( startPoint, endPoint )
            , m_endPoint( endPoint.GetAsPoint() )
            , m_length( startPoint.GetDistance3( endPoint ) )
        {}

        inline Vector GetEndPoint() const { return m_endPoint; }
        inline float GetLength() const { return m_length.ToFloat(); }

        //-------------------------------------------------------------------------

        inline float ScalarProjectionOnSegment( Vector const point ) const
        {
            auto const dot = Vector::Dot3( ( point - m_startPoint ), m_direction );
            float distance = Math::Clamp( dot.ToFloat(), 0.0f, GetLength() );
            return distance;
        }

        // Returns a point on the segment at the desired percentage between start and end points
        inline Vector GetPointOnSegment( float percentageAlongSegment ) const
        {
            KRG_ASSERT( percentageAlongSegment >= 0 && percentageAlongSegment <= 1.0f );
            Vector const distance = m_length * percentageAlongSegment;
            return Vector::MultiplyAdd( m_direction, distance, m_startPoint );
        }

        inline Vector VectorProjectionOnSegment( Vector const point, float& outScalarResolute ) const
        {
            outScalarResolute = ScalarProjectionOnSegment( point );
            return GetPointAlongLine( outScalarResolute );
        }

        inline Vector GetClosestPointOnSegment( Vector point ) const
        {
            float scalarResolute;
            return VectorProjectionOnSegment( point, scalarResolute );
        }

        inline Vector GetDistanceAlongLine( Vector const& point ) const
        {
            return Vector( ScalarProjectionOnSegment( point ) );
        }

        inline float GetDistanceBetweenSegmentAndPoint( Vector point ) const
        {
            Vector const closestPointOnSegment = GetClosestPointOnSegment( point );
            return closestPointOnSegment.GetDistance3( point );
        }

        //-------------------------------------------------------------------------

        inline LineSegment& Transform( Matrix const& transform )
        {
            m_startPoint = transform.TransformPoint( m_startPoint );
            m_endPoint = transform.TransformPoint( m_endPoint );
            m_direction = ( m_endPoint - m_startPoint ).GetNormalized3();
            m_length = Vector( m_startPoint.GetDistance3( m_endPoint ) );
            return *this;
        }

        inline LineSegment GetTransformed( Matrix const& transform ) const
        {
            LineSegment line = *this;
            line.Transform( transform );
            return line;
        }

    private:

        LineSegment() = default;

    private:

        Vector m_endPoint;
        Vector m_length;
    };


    //-------------------------------------------------------------------------
    // Ray - a line that is infinite in one direction
    //-------------------------------------------------------------------------

    class Ray : public Line
    {

    public:

        Ray( Vector const& startPoint, Vector const& direction )
        { 
            KRG_ASSERT( direction.IsNormalized3() );
            m_startPoint = startPoint;
            m_direction = direction;
        }

        Ray( Line const& line )
        {
            m_startPoint = line.m_startPoint;
            m_direction = line.m_direction;
        }

        Ray( LineSegment const& lineSegment )
        {
            m_startPoint = lineSegment.m_startPoint;
            m_direction = lineSegment.m_direction;
        }

        //-------------------------------------------------------------------------

        inline float ScalarProjectionOnRay( Vector const point ) const
        {
            auto const dot = Vector::Dot3( ( point - m_startPoint ), m_direction );
            float const distance = Math::Max( 0.f, dot.ToFloat() );
            return distance;
        }

        inline Vector GetPointAlongRay( float distanceFromStartPoint ) const 
        { 
            KRG_ASSERT( distanceFromStartPoint >= 0.0f );
            return Vector::MultiplyAdd( m_direction, Vector( distanceFromStartPoint ), m_startPoint );
        }

        inline Vector VectorProjectionOnRay( Vector const point, float& outScalarResolute ) const
        {
            outScalarResolute = ScalarProjectionOnRay( point );
            return GetPointAlongRay( outScalarResolute );
        }

        inline float GetDistanceAlongRay( Vector const point ) const { return ScalarProjectionOnRay( point ); }

        inline Vector GetClosestPointOnRay( Vector const point ) const 
        { 
            float scalarResolute;
            return VectorProjectionOnRay( point, scalarResolute );
        }

        //-------------------------------------------------------------------------

        inline Ray& Transform( Matrix const& transform )
        {
            m_startPoint = transform.TransformPoint( m_startPoint );
            m_direction = transform.RotateVector( m_direction );
            return *this;
        }

        inline Ray GetTransformed( Matrix const& transform ) const
        {
            Ray ray = *this;
            ray.Transform( transform );
            return ray;
        }

        //-------------------------------------------------------------------------

        inline LineSegment ToLineSegment( float length ) const
        { 
            LineSegment segment;
            segment.m_startPoint = m_startPoint;
            segment.m_direction = m_direction;
            segment.m_endPoint = GetPointAlongRay( length );
            return segment;
        }
    };
}