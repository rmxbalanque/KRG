#pragma once

#include "Math.h"
#include "System/Core/Serialization/Serialization.h"
#include "System/Core/Types/Percentage.h"

//-------------------------------------------------------------------------

namespace KRG
{
    struct FloatRange
    {
        KRG_SERIALIZE_MEMBERS( m_start, m_end );

        FloatRange() = default;

        inline FloatRange( float value )
            : m_start( value )
            , m_end( value )
        {}

        inline FloatRange( float min, float max )
            : m_start( min )
            , m_end( max )
        {
            KRG_ASSERT( IsValid() );
        }

        // Reset the range to an invalid value
        inline void Reset() { *this = FloatRange(); }

        // Is the range initialized
        inline bool IsSet() const { return m_start != FLT_MAX; }

        // Is the range contained valid i.e. is the end greater than the start
        inline bool IsValid() const { KRG_ASSERT( IsSet() ); return m_end >= m_start; }

        // Is the range set and valid
        inline bool IsSetAndValid() const { return IsSet() && IsValid(); }

        // Get the length of the range
        inline const float GetLength() const { return m_end - m_start; }

        // Get the midpoint of the range
        inline const float GetMidpoint() const { return m_start + ( ( m_end - m_start ) / 2 ); }

        // Does this range overlap the specified range
        inline bool Overlaps( FloatRange const& rhs ) const
        {
            KRG_ASSERT( IsSetAndValid() );
            return Math::Max( m_start, rhs.m_start ) <= Math::Min( m_end, rhs.m_end );
        }

        // Shifts the range by the supplied delta
        inline void ShiftRange( float delta )
        {
            m_start += delta;
            m_end += delta;
        }

        // Does the range [min, max] contain the specified range
        inline bool ContainsInclusive( FloatRange const& rhs ) const
        {
            KRG_ASSERT( IsSetAndValid() && rhs.IsSetAndValid() );
            return m_start <= rhs.m_start && m_end >= rhs.m_end;
        }

        // Does the range [min, max] contain the specified value
        inline bool ContainsInclusive( float const& v ) const
        {
            KRG_ASSERT( IsSetAndValid() );
            return v >= m_start && v <= m_end;
        }

        // Does the range (min, max) contain the specified range
        inline bool ContainsExclusive( FloatRange const& rhs ) const
        {
            KRG_ASSERT( IsSetAndValid() && rhs.IsSetAndValid() );
            return m_start < rhs.m_start && m_end > rhs.m_end;
        }

        // Does the range (min, max) contain the specified value
        inline bool ContainsExclusive( float const& v ) const
        {
            KRG_ASSERT( IsSetAndValid() );
            return v > m_start && v < m_end;
        }

        // Get a value clamped to this range i.e. Clamp to [min, max]
        inline float GetClampedValue( float const& v ) const
        {
            KRG_ASSERT( IsSetAndValid() );
            return Math::Clamp( v, m_start, m_end );
        }

        // Get the percentage through this range that specified value lies at. This is not clamped and returns a value between [-FLT_MAX, FLT_MAX]
        inline Percentage GetPercentageThrough( float const& v ) const
        {
            KRG_ASSERT( IsSet() );
            float const length = GetLength();
            Percentage percentageThrough = 0.0f;
            if ( length != 0 )
            {
                percentageThrough = Percentage( ( v - m_start ) / length );
            }
            return percentageThrough;
        }

        // Get the percentage through this range that specified value lies at. This is clamped between [0, 1]
        inline Percentage GetPercentageThroughClamped( float const& v ) const
        {
            return GetPercentageThrough( GetClampedValue( v ) );
        }

        // Get the value in this range at the specified percentage through. Unclamped so returns [-FLT_MAX, FLT_MAX]
        inline float GetValueForPercentageThrough( Percentage const percentageThrough ) const
        {
            KRG_ASSERT( IsSet() );
            return ( GetLength() * percentageThrough ) + m_start;
        }

        // Get the value in this range at the specified percentage through. Clamped to [min, max]
        inline float GetValueForPercentageThroughClamped( Percentage const percentageThrough ) const
        {
            return GetValueForPercentageThrough( percentageThrough.GetClamped( false ) );
        }

        // Ensure the range is valid if the values are set incorrectly
        inline void MakeValid()
        {
            KRG_ASSERT( IsSet() );

            if ( !IsValid() )
            {
                float originalEnd = m_end;
                m_end = m_start;
                m_start = originalEnd;
            }
        }

        // Insert a new value into the range and grow it if necessary
        inline void GrowRange( float newValue )
        {
            if ( IsSet() )
            {
                KRG_ASSERT( IsValid() );
                m_start = Math::Min( m_start, newValue );
                m_end = Math::Max( m_end, newValue );
            }
            else
            {
                m_start = m_end = newValue;
            }
        }

        // Grow this range with the supplied range
        inline void Merge( FloatRange const& rhs )
        {
            KRG_ASSERT( IsSetAndValid() && rhs.IsSetAndValid() );
            m_start = Math::Min( m_start, rhs.m_start );
            m_end = Math::Max( m_end, rhs.m_end );
        }

        // Get the combined range of this and the supplied range
        inline FloatRange GetMerged( FloatRange const& rhs ) const
        {
            FloatRange mergedRange = *this;
            mergedRange.Merge( rhs );
            return mergedRange;
        }

        inline bool operator==( FloatRange const& rhs ) const
        {
            return m_start == rhs.m_start && m_end == rhs.m_end;
        }

        inline bool operator!=( FloatRange const& rhs ) const
        {
            return m_start != rhs.m_start || m_end != rhs.m_end;
        }

    public:

        float m_start = FLT_MAX;
        float m_end = -FLT_MAX;
    };

    //-------------------------------------------------------------------------

    struct IntRange
    {
        KRG_SERIALIZE_MEMBERS( m_start, m_end );

        IntRange() = default;

        inline IntRange( int32 value )
            : m_start( value )
            , m_end( value )
        {}

        inline IntRange( int32 min, int32 max )
            : m_start( min )
            , m_end( max )
        {
            KRG_ASSERT( IsValid() );
        }

        // Reset the range to an invalid value
        inline void Reset() { *this = IntRange(); }

        // Is the range initialized
        inline bool IsSet() const { return m_start != FLT_MAX; }

        // Is the range contained valid i.e. is the end greater than the start
        inline bool IsValid() const { KRG_ASSERT( IsSet() ); return m_end >= m_start; }

        // Is the range set and valid
        inline bool IsSetAndValid() const { return IsSet() && IsValid(); }

        // Get the length of the range
        inline const int32 GetLength() const { return m_end - m_start; }

        // Get the midpoint of the range
        inline const int32 GetMidpoint() const { return m_start + ( ( m_end - m_start ) / 2 ); }

        // Does this range overlap the specified range
        inline bool Overlaps( IntRange const& rhs ) const
        {
            KRG_ASSERT( IsSetAndValid() );
            return Math::Max( m_start, rhs.m_start ) <= Math::Min( m_end, rhs.m_end );
        }

        // Shifts the range by the supplied delta
        inline void ShiftRange( int32 delta )
        {
            m_start += delta;
            m_end += delta;
        }

        // Does the range [min, max] contain the specified range
        inline bool ContainsInclusive( IntRange const& rhs ) const
        {
            KRG_ASSERT( IsSetAndValid() && rhs.IsSetAndValid() );
            return m_start <= rhs.m_start && m_end >= rhs.m_end;
        }

        // Does the range [min, max] contain the specified value
        inline bool ContainsInclusive( int32 const& v ) const
        {
            KRG_ASSERT( IsSetAndValid() );
            return v >= m_start && v <= m_end;
        }

        // Does the range (min, max) contain the specified range
        inline bool ContainsExclusive( IntRange const& rhs ) const
        {
            KRG_ASSERT( IsSetAndValid() && rhs.IsSetAndValid() );
            return m_start < rhs.m_start && m_end > rhs.m_end;
        }

        // Does the range (min, max) contain the specified value
        inline bool ContainsExclusive( int32 const& v ) const
        {
            KRG_ASSERT( IsSetAndValid() );
            return v > m_start && v < m_end;
        }

        // Get a value clamped to this range i.e. Clamp to [min, max]
        inline int32 GetClampedValue( int32 const& v ) const
        {
            KRG_ASSERT( IsSetAndValid() );
            return Math::Clamp( v, m_start, m_end );
        }

        // Get the percentage through this range that specified value lies at. This is not clamped and returns a value between [-FLT_MAX, FLT_MAX]
        inline Percentage GetPercentageThrough( int32 const& v ) const
        {
            KRG_ASSERT( IsSet() );
            int32 const length = GetLength();
            Percentage percentageThrough = 0.0f;
            if ( length != 0 )
            {
                percentageThrough = Percentage( float( v - m_start ) / length );
            }
            return percentageThrough;
        }

        // Get the percentage through this range that specified value lies at. This is clamped between [0, 1]
        inline Percentage GetPercentageThroughClamped( int32 const& v ) const
        {
            return GetPercentageThrough( GetClampedValue( v ) );
        }

        // Get the value in this range at the specified percentage through. Unclamped so returns [-FLT_MAX, FLT_MAX]
        inline int32 GetValueForPercentageThrough( Percentage const percentageThrough ) const
        {
            KRG_ASSERT( IsSet() );
            return Math::RoundToInt( ( GetLength() * percentageThrough ) + m_start );
        }

        // Get the value in this range at the specified percentage through. Clamped to [min, max]
        inline int32 GetValueForPercentageThroughClamped( Percentage const percentageThrough ) const
        {
            return GetValueForPercentageThrough( percentageThrough.GetClamped( false ) );
        }

        // Ensure the range is valid if the values are set incorrectly
        inline void MakeValid()
        {
            KRG_ASSERT( IsSet() );

            if ( !IsValid() )
            {
                int32 originalEnd = m_end;
                m_end = m_start;
                m_start = originalEnd;
            }
        }

        // Insert a new value into the range and grow it if necessary
        inline void GrowRange( int32 newValue )
        {
            if ( IsSet() )
            {
                KRG_ASSERT( IsValid() );
                m_start = Math::Min( m_start, newValue );
                m_end = Math::Max( m_end, newValue );
            }
            else
            {
                m_start = m_end = newValue;
            }
        }

        // Grow this range with the supplied range
        inline void Merge( IntRange const& rhs )
        {
            KRG_ASSERT( IsSetAndValid() && rhs.IsSetAndValid() );
            m_start = Math::Min( m_start, rhs.m_start );
            m_end = Math::Max( m_end, rhs.m_end );
        }

        // Get the combined range of this and the supplied range
        inline IntRange GetMerged( IntRange const& rhs ) const
        {
            IntRange mergedRange = *this;
            mergedRange.Merge( rhs );
            return mergedRange;
        }

        inline bool operator==( IntRange const& rhs ) const
        {
            return m_start == rhs.m_start && m_end == rhs.m_end;
        }

        inline bool operator!=( IntRange const& rhs ) const
        {
            return m_start != rhs.m_start || m_end != rhs.m_end;
        }

    public:

        int32 m_start = INT_MAX;
        int32 m_end = INT_MIN;
    };
}