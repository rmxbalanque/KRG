#pragma once

#include "Math.h"
#include "System/Core/Serialization/Serialization.h"
#include "System/Core/Types/Percentage.h"

//-------------------------------------------------------------------------

namespace KRG
{
    template<typename T>
    struct TRange
    {
        KRG_SERIALIZE_MEMBERS( m_min, m_max );

        TRange() = default;

        inline TRange( T value )
            : m_min( value )
            , m_max( value )
        {}

        inline TRange( T v0, T v1 )
            : m_min( v0 )
            , m_max( v1 )
        {
            if ( v0 > v1 )
            {
                m_min = v1;
                m_max = v0;
            }
        }

        // Reset the range to an invalid value
        inline void Reset() { *this = TRange<T>(); }

        // Is the range initialized
        inline bool IsSet() const { return m_min != FLT_MAX; }

        // Is the range contained valid i.e. is the max greater than the min
        inline bool IsValid() const { KRG_ASSERT( IsSet() ); return m_max >= m_min; }

        // Is the range set and valid
        inline bool IsSetAndValid() const { return IsSet() && IsValid(); }

        // Get the length of the range
        inline const T GetLength() const { return m_max - m_min; }

        // Get the midpoint of the range
        inline const T GetMidpoint() const { return m_min + ( ( m_max - m_min ) / 2 ); }

        // Does this range overlap the specified range
        inline bool Overlaps( TRange<T> const& rhs ) const
        {
            KRG_ASSERT( IsSetAndValid() );
            return Math::Max( m_min, rhs.m_min ) <= Math::Min( m_max, rhs.m_max );
        }

        // Does the range [min, max] contain the specified range
        inline bool ContainsInclusive( TRange<T> const& rhs ) const
        {
            KRG_ASSERT( IsSetAndValid() && rhs.IsSetAndValid() );
            return m_min <= rhs.m_min && m_max >= rhs.m_max;
        }

        // Does the range [min, max] contain the specified value
        inline bool ContainsInclusive( T const& v ) const
        {
            KRG_ASSERT( IsSetAndValid() );
            return v >= m_min && v <= m_max;
        }

        // Does the range (min, max) contain the specified range
        inline bool ContainsExclusive( TRange<T> const& rhs ) const
        {
            KRG_ASSERT( IsSetAndValid() && rhs.IsSetAndValid() );
            return m_min < rhs.m_min && m_max > rhs.m_max;
        }

        // Does the range (min, max) contain the specified value
        inline bool ContainsExclusive( T const& v ) const
        {
            KRG_ASSERT( IsSetAndValid() );
            return v > m_min && v < m_max;
        }

        // Get a value clamped to this range i.e. Clamp to [min, max]
        inline T GetClampedValue( T const& v ) const
        {
            KRG_ASSERT( IsSetAndValid() );
            return Math::Clamp( v, m_min, m_max );
        }

        // Get the percentage through this range that specified value lies at. This is not clamped and returns a value between [-FLT_MAX, FLT_MAX]
        inline Percentage GetPercentageThrough( T const& v ) const
        {
            KRG_ASSERT( IsSetAndValid() );
            T const length = GetLength();
            Percentage percentageThrough = 0.0f;
            if ( length != 0 )
            {
                percentageThrough = Percentage( ( v - m_min ) / length );
            }
            return percentageThrough;
        }

        // Get the percentage through this range that specified value lies at. This is clamped between [0, 1]
        inline Percentage GetPercentageThroughClamped( T const& v ) const
        {
            KRG_ASSERT( IsSetAndValid() );
            T const length = GetLength();
            Percentage const percentageThrough( ( length != 0 ) ? ( Clamp( v ) - m_min ) / length : 0 );
            return percentageThrough;
        }

        // Get the value in this range at the specified percentage through. Unclamped so returns [-FLT_MAX, FLT_MAX]
        inline T GetValueForPercentageThrough( Percentage const percentageThrough ) const
        {
            KRG_ASSERT( percentageThrough >= 0.0f && percentageThrough <= 1.0f );
            return ( GetLength() * percentageThrough ) + m_min;
        }

        // Get the value in this range at the specified percentage through. Clamped to [min, max]
        inline T GetValueForPercentageThroughClamped( Percentage const percentageThrough ) const
        {
            KRG_ASSERT( IsSetAndValid() );
            auto const clampedPercentage = Math::Clamp( percentageThrough, 0.0f, 1.0f );
            return GetValueForPercentageThrough( clampedPercentage );
        }

        // Ensure the range is valid if the values are set incorrectly
        inline void MakeValid()
        {
            KRG_ASSERT( IsSet() );

            if ( !IsValid() )
            {
                T originalMax = m_max;
                m_max = m_min;
                m_min = originalMax;
            }
        }

        // Insert a new value into the range and grow it if necessary
        inline void GrowRange( T newValue )
        {
            if ( IsSet() )
            {
                m_min = Math::Min( m_min, newValue );
                m_max = Math::Max( m_max, newValue );
            }
            else
            {
                m_min = m_max = newValue;
            }
        }

        // Grow this range with the supplied range
        inline void Merge( TRange<T> const& rhs )
        {
            KRG_ASSERT( IsSetAndValid() && rhs.IsSetAndValid() );
            m_min = Math::Min( m_min, rhs.m_min );
            m_max = Math::Max( m_max, rhs.m_max );
        }

        // Get the combined range of this and the supplied range
        inline TRange<T> GetMerged( TRange<T> const& rhs ) const
        {
            TRange<T> mergedRange = *this;
            mergedRange.Merge( rhs );
            return mergedRange;
        }

    public:

        T m_min = FLT_MAX;
        T m_max = -FLT_MAX;
    };
}