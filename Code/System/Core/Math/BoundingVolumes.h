#pragma once

#include "Transform.h"

//-------------------------------------------------------------------------

namespace KRG
{
    enum class OverlapResult
    {
        NoOverlap,
        Overlap,
        FullyEnclosed,
    };

    //-------------------------------------------------------------------------

    struct Sphere;
    struct AABB;
    struct OBB;

    //-------------------------------------------------------------------------
    // Bounding Sphere
    //-------------------------------------------------------------------------

    struct KRG_SYSTEM_CORE_API Sphere
    {
        friend AABB;
        friend OBB;

    public:

        Sphere() = default;
        Sphere( Vector position, F32 radius ) : m_center( position ), m_radius( radius ) { KRG_ASSERT( radius >= 0.0f ); }
        Sphere( Vector* points, U32 numPoints );
        explicit Sphere( AABB const& box );
        explicit Sphere( OBB const& box );

        //-------------------------------------------------------------------------

        KRG_FORCE_INLINE Vector GetCenter() const { return m_center; }
        KRG_FORCE_INLINE void SetCenter( Vector const& newCenter ) { m_center = newCenter; }
        KRG_FORCE_INLINE F32 GetRadius() const{ return m_radius.ToFloat(); }
        KRG_FORCE_INLINE void GetRadius( F32 newRadius ) { KRG_ASSERT( newRadius > 0 ); m_radius = Vector( newRadius ); }
        KRG_FORCE_INLINE bool IsPoint() { return m_radius.IsNearZero4(); }

        // Queries
        //-------------------------------------------------------------------------

        KRG_FORCE_INLINE bool ContainsPoint( Vector const& point ) const;
        KRG_FORCE_INLINE OverlapResult OverlapTest( Sphere const& other ) const;
        KRG_FORCE_INLINE bool Contains( Sphere const& other ) const; // Fast containment test
        KRG_FORCE_INLINE bool Overlaps( Sphere const& other ) const; // Fast overlap test

        //-------------------------------------------------------------------------

        inline bool Overlaps( AABB const& box ) const;
        inline bool Contains( AABB const& box ) const;

        inline bool Overlaps( OBB const& box ) const;
        inline bool Contains( OBB const& box ) const;

    private:

        Vector m_center = Vector::UnitW;
        Vector m_radius = Vector::Zero;
    };

    //-------------------------------------------------------------------------
    // Axis Aligned Bounding Box
    //-------------------------------------------------------------------------

    struct KRG_SYSTEM_CORE_API AABB
    {
        KRG_SERIALIZE_MEMBERS( m_center, m_extents );

    public:

        inline static AABB FromMinMax( Vector const& min, Vector const& max )
        {
            KRG_ASSERT( min.IsLessThanEqual3( max ) );
            Vector const extents = ( max - min ) * Vector::Half;
            Vector const center = min + extents;
            return AABB( center, extents );
        }

    public:

        AABB() = default;
        AABB( Vector* points, U32 numPoints );
        AABB( Vector const& center ) : m_center( center ), m_extents( Vector::Zero ) {}
        AABB( Vector const& center, Vector const& extents ) : m_center( center ), m_extents( extents ) { KRG_ASSERT( IsValid() ); }
        AABB( Vector const& center, F32 const& extents ) : m_center( center ), m_extents( extents ) { KRG_ASSERT( IsValid() ); }
        explicit AABB( OBB const& box );

        inline bool IsValid() const { return m_extents.IsGreaterThanEqual3( Vector::Zero ); }
        inline void Reset() { m_center = Vector::Zero; m_extents = Vector::NegativeOne; }
        inline void Reset( Float3 center ) { m_center = center; m_extents = Vector::Zero; }

        //-------------------------------------------------------------------------

        KRG_FORCE_INLINE void AddPoint( Vector const& point )
        {
            Vector const min = Vector::Min( GetMin(), point );
            Vector const max = Vector::Max( GetMax(), point );
            SetFromMinMax( min, max );
        }

        KRG_FORCE_INLINE void SetFromMinMax( Vector const& min, Vector const& max )
        {
            KRG_ASSERT( min.IsLessThanEqual3( max ) );
            m_extents = ( max - min ) * Vector::Half;
            m_center = min + m_extents;
        }

        //-------------------------------------------------------------------------

        // Positioning
        //-------------------------------------------------------------------------

        KRG_FORCE_INLINE void SetCenter( Vector const& newCenter ) { m_center = newCenter; }

        // Move the AABB by the amount in deltaVector
        KRG_FORCE_INLINE void Translate( Vector const& deltaVector ) { m_center += deltaVector; }

        // Applies the given transform to the current AABB corners and calculates the new AABB for the transformed box
        void ApplyTransform( Transform const& transform );

        KRG_FORCE_INLINE AABB GetTransformed( Transform const& transform ) const
        {
            AABB result = *this;
            result.ApplyTransform( transform );
            return result;
        }

        //-------------------------------------------------------------------------

        KRG_FORCE_INLINE Vector const& GetCenter() const { return m_center; }
        KRG_FORCE_INLINE Vector const& GetExtents() const { return m_extents; }
        KRG_FORCE_INLINE Vector GetMin() const { return m_center - m_extents; }
        KRG_FORCE_INLINE Vector GetMax() const { return m_center + m_extents; }

        KRG_FORCE_INLINE void GetCorners( Vector corners[8] ) const;
        KRG_FORCE_INLINE F32 GetVolume() const;

        // Queries
        //-------------------------------------------------------------------------

        KRG_FORCE_INLINE bool ContainsPoint( Vector const& point ) const { return ( point - m_center ).Abs().IsLessThanEqual3( m_extents ); }

        //-------------------------------------------------------------------------

        KRG_FORCE_INLINE OverlapResult OverlapTest( AABB const& other ) const;
        KRG_FORCE_INLINE bool Overlaps( AABB const& other ) const; // Fast overlap test

        //-------------------------------------------------------------------------

        OverlapResult OverlapTest( Sphere const& sphere ) const;
        KRG_FORCE_INLINE bool Overlaps( Sphere const& sphere ) const; // Fast overlap test

        //-------------------------------------------------------------------------

        OverlapResult OverlapTest( OBB const& box ) const;
        KRG_FORCE_INLINE bool Overlaps( OBB const& box ) const; // Fast overlap test

        //-------------------------------------------------------------------------

        KRG_FORCE_INLINE AABB GetMergedBox( AABB const& other ) const
        {
            Vector const newMin = Vector::Min( GetMin(), other.GetMin() );
            Vector const newMax = Vector::Max( GetMax(), other.GetMax() );
            return AABB::FromMinMax( newMin, newMax );
        }

        KRG_FORCE_INLINE AABB GetIntersectionBox( AABB const& other ) const
        {
            Vector const newMin = Vector::Max( GetMin(), other.GetMin() );
            Vector const newMax = Vector::Min( GetMax(), other.GetMax() );
            return AABB::FromMinMax( newMin, newMax );
        }

    public:

        Vector          m_center = Vector::UnitW;
        Vector          m_extents = Vector::NegativeOne;
    };

    //-------------------------------------------------------------------------
    // Oriented Bounding Box
    //-------------------------------------------------------------------------

    struct KRG_SYSTEM_CORE_API OBB
    {
        KRG_SERIALIZE_MEMBERS( m_center, m_extents, m_orientation );

        OBB() = default;
        OBB( Vector center, Vector extents, Quaternion orientation = Quaternion::Identity );
        explicit OBB( AABB const& aabb );
        explicit OBB( AABB const& aabb, Transform const& transform );

        //-------------------------------------------------------------------------

        KRG_FORCE_INLINE void Reset() { m_orientation = Quaternion::Identity; m_center = m_extents = Vector::Zero; }

        KRG_FORCE_INLINE void GetCorners( Vector corners[8] ) const
        {
            for ( S32 i = 0; i < 8; ++i )
            {
                corners[i] = m_center + m_orientation.RotateVector( m_extents * Vector::BoxCorners[i] );
            }
        }

        KRG_FORCE_INLINE AABB GetAABB() const;

        // Positioning
        //-------------------------------------------------------------------------

        KRG_FORCE_INLINE void SetCenter( Vector const& newCenter ) { m_center = newCenter; }

        KRG_FORCE_INLINE void SetOrientation( Quaternion const& newOrientation ) { m_orientation = newOrientation; }

        KRG_FORCE_INLINE void Translate( Vector const& deltaVector ) { m_center += deltaVector; }

        KRG_FORCE_INLINE void Rotate( Quaternion const& deltaRotation ) { m_orientation = deltaRotation * m_orientation; }

        void ApplyTransform( Transform const& transform );

        KRG_FORCE_INLINE OBB GetTransformed( Transform const& transform ) const
        {
            OBB result = *this;
            result.ApplyTransform( transform );
            return result;
        }

        // Queries
        //-------------------------------------------------------------------------

        KRG_FORCE_INLINE bool ContainsPoint( Vector const& point ) const;

        //-------------------------------------------------------------------------

        KRG_FORCE_INLINE OverlapResult OverlapTest( OBB const& other ) const;
        bool Overlaps( OBB const& box ) const; // Slightly faster than the full overlap test

        //-------------------------------------------------------------------------

        OverlapResult OverlapTest( Sphere const& sphere ) const;
        bool Overlaps( Sphere const& sphere ) const;

        //-------------------------------------------------------------------------

        KRG_FORCE_INLINE OverlapResult OverlapTest( AABB const& aabb ) const { return OverlapTest( OBB( aabb ) ); }
        KRG_FORCE_INLINE bool Overlaps( AABB const& aabb ) const { return OverlapTest( OBB( aabb ) ) != OverlapResult::NoOverlap; }

    public:

        Quaternion  m_orientation = Quaternion::Identity;
        Vector      m_center = Vector::UnitW;
        Vector      m_extents = Vector::Zero;
    };

    //-------------------------------------------------------------------------
    // INLINE IMPLEMENTATIONS
    //-------------------------------------------------------------------------

    KRG_FORCE_INLINE bool Sphere::ContainsPoint( Vector const& point ) const
    {
        Vector const distanceSq = ( point - m_center ).LengthSquared3();
        return distanceSq.IsLessThanEqual3( m_radius * m_radius );
    }

    // Full overlap test
    KRG_FORCE_INLINE OverlapResult Sphere::OverlapTest( Sphere const& other ) const
    {
        F32 const r1 = m_radius.x;
        F32 const r2 = other.m_radius.x;
        F32 const distance = ( other.m_center - m_center ).Length3().ToFloat();

        if ( r1 + r2 >= distance )
        {
            return OverlapResult::FullyEnclosed;
        }
        else if ( r1 - r2 >= distance )
        {
            return OverlapResult::Overlap;
        }

        return OverlapResult::NoOverlap;
    }

    // Fast containment test
    KRG_FORCE_INLINE bool Sphere::Contains( Sphere const& other ) const
    {
        Vector const totalRadii = ( m_radius + other.m_radius );
        Vector const distance = ( other.m_center - m_center ).Length3();
        return totalRadii.IsGreaterThan3( distance );
    }

    // Fast overlap test
    KRG_FORCE_INLINE bool Sphere::Overlaps( Sphere const& other ) const
    {
        Vector const deltaCenters = ( m_center - other.m_center ).Length3();
        Vector const sumExtents = m_radius + other.m_radius;
        return deltaCenters.IsLessThanEqual3( sumExtents );
    }

    //-------------------------------------------------------------------------

    KRG_FORCE_INLINE void AABB::GetCorners( Vector corners[8] ) const
    {
        for ( S32 i = 0; i < 8; ++i )
        {
            corners[i] = Vector::MultiplyAdd( m_extents, Vector::BoxCorners[i], m_center );
        }
    }

    KRG_FORCE_INLINE F32 AABB::GetVolume() const
    {
        Vector const fullExtents = m_extents * 2;
        return m_extents.x * m_extents.y * m_extents.z;
    }

    // Full overlap test
    KRG_FORCE_INLINE OverlapResult AABB::OverlapTest( AABB const& other ) const
    {
        Vector const MinA = GetMin();
        Vector const MaxA = GetMax();
        Vector const MinB = other.GetMin();
        Vector const MaxB = other.GetMax();

        if ( MinA.IsGreaterThan3( MaxB ) || MinB.IsGreaterThan3( MaxA ) )
        {
            return OverlapResult::NoOverlap;
        }

        if ( MinA.IsLessThanEqual3( MinB ) && MaxB.IsLessThanEqual3( MaxA ) )
        {
            return OverlapResult::FullyEnclosed;
        }

        return OverlapResult::Overlap;
    }

    // Fast overlap test
    KRG_FORCE_INLINE bool AABB::Overlaps( AABB const& other ) const
    {
        Vector const deltaCenters = ( m_center - other.m_center ).GetAbs();
        Vector const sumExtents = m_extents + other.m_extents;
        return deltaCenters.IsLessThanEqual3( sumExtents );
    }

    KRG_FORCE_INLINE bool AABB::Overlaps( Sphere const& sphere ) const
    {
        Vector boxMin = m_center - m_extents;
        Vector boxMax = m_center + m_extents;

        // Find the distance to the nearest point on the box.
        // for each i in (x, y, z)
        // if (SphereCenter(i) < BoxMin(i)) d2 += (SphereCenter(i) - BoxMin(i)) ^ 2
        // else if (SphereCenter(i) > BoxMax(i)) d2 += (SphereCenter(i) - BoxMax(i)) ^ 2

        // Compute d for each dimension.
        Vector const lessThanMin = sphere.m_center.LessThan( boxMin );
        Vector const greaterThanMax = sphere.m_center.GreaterThan( boxMax );
        Vector const minDelta = sphere.m_center - boxMin;
        Vector const maxDelta = sphere.m_center - boxMax;

        // Choose value for each dimension based on the comparison.
        Vector d = Vector::Zero;
        d = Vector::Select( d, minDelta, lessThanMin );
        d = Vector::Select( d, maxDelta, greaterThanMax );

        // Use a dot-product to square them and sum them together.
        Vector d2 = d.Dot3( d );
        return d2.IsLessThanEqual3( sphere.m_radius * sphere.m_radius );
    }

    //-------------------------------------------------------------------------

    // Optimization from: https://zeux.io/2010/10/17/aabb-from-obb-with-component-wise-abs/
    KRG_FORCE_INLINE AABB OBB::GetAABB() const
    {
        Matrix rotationTransform( m_orientation );
        for ( auto i = 0; i < 4; i++ )
        {
            rotationTransform[i] = rotationTransform[i].Abs();
        }

        Vector const newExtent = rotationTransform.RotateVector( m_extents );
        return AABB::FromMinMax( m_center - newExtent, m_center + newExtent );
    }

    KRG_FORCE_INLINE bool OBB::ContainsPoint( Vector const& point ) const
    {
        Vector const untransformedPoint = m_orientation.RotateVectorInverse( point - m_center );
        return untransformedPoint.IsInBounds3( m_extents );
    }

    KRG_FORCE_INLINE OverlapResult OBB::OverlapTest( OBB const& other ) const
    {
        if ( !Overlaps( other ) )
        {
            return OverlapResult::NoOverlap;
        }

        Vector const offset = other.m_center - m_center;
        for ( S32 i = 0; i < 8; ++i )
        {
            Vector C = other.m_orientation.RotateVector( other.m_extents * Vector::BoxCorners[i] ) + offset;
            C = m_orientation.RotateVectorInverse( C );
            if ( !C.IsInBounds3( m_extents ) )
            {
                return OverlapResult::Overlap;
            }
        }

        return OverlapResult::FullyEnclosed;
    }
}