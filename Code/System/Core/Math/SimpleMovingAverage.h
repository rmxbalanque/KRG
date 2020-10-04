#pragma once

#include "Math.h"

//-------------------------------------------------------------------------

namespace KRG
{
    template<U32 HistorySize>
    class SimpleMovingAverage
    {
    public:

        SimpleMovingAverage( F32 startValue )
            : m_average( startValue )
            , m_nextFreeIdx( 0 )
        {
            static_assert( HistorySize > 1, "History size has to be at least 1" );
            for ( auto i = 0; i < HistorySize; i++ )
            {
                m_values[i] = startValue;
            }
        }

        inline F32 GetAverage() const { return m_average; }

        inline void AddValue( F32 const value )
        {
            m_values[m_nextFreeIdx++] = value;
            if ( m_nextFreeIdx == m_values.Num() )
            {
                m_nextFreeIdx = 0;
            }

            m_average = 0;
            for ( F32 v : m_values )
            {
                m_average += v;
            }
            m_average /= m_values.Num();
        }

    private:

        F32                     m_values[HistorySize];
        F32                     m_average;
        S32                     m_nextFreeIdx;
    };
}