#include "AnimationFrameTime.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        FrameTime FrameTime::operator+( Percentage const& RHS ) const
        {
            FrameTime newTime = *this;
            S32 loopCount;
            Percentage newPercent = m_percentageThrough + RHS;
            newPercent.GetLoopCountAndNormalizedTime( loopCount, newTime.m_percentageThrough );
            newTime.m_frameIndex += loopCount;
            return newTime;
        }

        FrameTime& FrameTime::operator+=( Percentage const& RHS )
        {
            S32 loopCount;
            Percentage newPercent = m_percentageThrough + RHS;
            newPercent.GetLoopCountAndNormalizedTime( loopCount, m_percentageThrough );
            m_frameIndex += loopCount;
            return *this;
        }

        FrameTime FrameTime::operator-( Percentage const& RHS ) const
        {
            FrameTime newTime = *this;

            S32 loopCount;
            Percentage newPercent = m_percentageThrough - RHS;
            newPercent.GetLoopCountAndNormalizedTime( loopCount, newTime.m_percentageThrough );

            if ( loopCount < 0 )
            {
                newTime.m_frameIndex -= loopCount;
                newTime.m_percentageThrough.Invert();
            }

            return newTime;
        }

        FrameTime& FrameTime::operator-=( Percentage const& RHS )
        {
            S32 loopCount;
            Percentage newPercent = m_percentageThrough - RHS;
            newPercent.GetLoopCountAndNormalizedTime( loopCount, m_percentageThrough );

            if ( loopCount < 0 )
            {
                m_frameIndex -= loopCount;
                m_percentageThrough.Invert();
            }

            return *this;
        }
    }
}