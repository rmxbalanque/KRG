#pragma once

#include "_Module/API.h"
#include "System/Core/Types/Percentage.h"
#include "System/Core/Time/Time.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class KRG_ENGINE_ANIMATION_API FrameTime
    {
    public:

        FrameTime() = default;

        inline FrameTime( uint32 frameIndex, Percentage percentageThrough = Percentage( 0 ) )
            : m_frameIndex( frameIndex )
            , m_percentageThrough( percentageThrough )
        {
            KRG_ASSERT( frameIndex >= 0 );
            KRG_ASSERT( percentageThrough.ToFloat() >= 0.0f && percentageThrough < 1.0f );
        }

        inline FrameTime( Percentage percent, uint32 numFrames )
        {
            KRG_ASSERT( numFrames > 0 && percent.ToFloat() >= 0.0f );
            percent.Clamp( true );

            float integerPortion;
            m_percentageThrough = Percentage( Math::ModF( percent.ToFloat() * numFrames, &integerPortion ) );
            m_frameIndex = (uint32) integerPortion;
        }

        inline FrameTime( Percentage percent, Seconds animationDuration, Seconds frameLength )
            : FrameTime( percent, (uint32) ( animationDuration / frameLength ) )
        {
            KRG_ASSERT( Math::IsNearZero( Math::FModF( animationDuration, frameLength ) ) );
        }

        //-------------------------------------------------------------------------

        inline uint32 GetFrameIndex() const { return m_frameIndex; }
        inline Percentage GetPercentageThrough() const { return m_percentageThrough; }
        inline bool IsExactlyAtKeyFrame() const { return m_percentageThrough == 0.0f; }

        inline float ToFloat() const { return m_percentageThrough.ToFloat() + m_frameIndex; }

        inline Seconds GetTimeInSeconds( Seconds frameLength ) const { KRG_ASSERT( frameLength > 0.0f );  return ToFloat() * frameLength; }

        inline FrameTime operator+( FrameTime const& RHS ) const;
        inline FrameTime& operator+=( FrameTime const& RHS );
        inline FrameTime operator-( FrameTime const& RHS ) const;
        inline FrameTime& operator-=( FrameTime const& RHS );

        inline FrameTime operator+( uint32 const& RHS ) const;
        inline FrameTime& operator+=( uint32 const& RHS );
        inline FrameTime operator-( uint32 const& RHS ) const;
        inline FrameTime& operator-=( uint32 const& RHS );

        FrameTime operator+( Percentage const& RHS ) const;
        FrameTime& operator+=( Percentage const& RHS );
        FrameTime operator-( Percentage const& RHS ) const;
        FrameTime& operator-=( Percentage const& RHS );

    private:

        uint32             m_frameIndex = 0;
        Percentage      m_percentageThrough = Percentage( 0.0f );
    };

    //-------------------------------------------------------------------------

    inline FrameTime FrameTime::operator+( FrameTime const& RHS ) const
    {
        FrameTime newInterval = *this;
        newInterval.m_frameIndex += RHS.m_frameIndex;
        newInterval.m_percentageThrough += RHS.m_percentageThrough;
        return newInterval;
    }

    inline FrameTime& FrameTime::operator+=( FrameTime const& RHS )
    {
        m_frameIndex += RHS.m_frameIndex;
        m_percentageThrough += RHS.m_percentageThrough;
        return *this;
    }

    inline FrameTime FrameTime::operator-( FrameTime const& RHS ) const
    {
        FrameTime newInterval = *this;
        newInterval.m_frameIndex -= RHS.m_frameIndex;
        newInterval.m_percentageThrough -= RHS.m_percentageThrough;
        return newInterval;
    }

    inline FrameTime& FrameTime::operator-=( FrameTime const& RHS )
    {
        m_frameIndex -= RHS.m_frameIndex;
        m_percentageThrough -= RHS.m_percentageThrough;
        return *this;
    }

    inline FrameTime FrameTime::operator+( uint32 const& RHS ) const
    {
        FrameTime newInterval = *this;
        newInterval.m_frameIndex += RHS;
        return newInterval;
    }

    inline FrameTime& FrameTime::operator+=( uint32 const& RHS )
    {
        m_frameIndex += RHS;
        return *this;
    }

    inline FrameTime FrameTime::operator-( uint32 const& RHS ) const
    {
        FrameTime newInterval = *this;
        newInterval.m_frameIndex -= RHS;
        return newInterval;
    }

    inline FrameTime& FrameTime::operator-=( uint32 const& RHS )
    {
        m_frameIndex -= RHS;
        return *this;
    }
}