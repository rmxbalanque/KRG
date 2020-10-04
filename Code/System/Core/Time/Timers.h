#pragma once

#include "System/Core/Types/Time.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class Timer
    {
    public:

        Timer() { Start(); }

        inline void Start() { m_startTime = Ticks::Now(); }
        inline void Reset() { Start(); }

        inline Ticks GetElapsedTimeTicks() const { return Ticks::Now() - m_startTime; }
        inline Seconds GetElapsedTimeSeconds() const { return GetElapsedTimeTicks().ToSeconds(); }
        inline Milliseconds GetElapsedTimeMilliseconds() const { return GetElapsedTimeTicks().ToMilliseconds(); }
        inline Microseconds GetElapsedTimeMicroseconds() const { return GetElapsedTimeTicks().ToMicroseconds(); }

    private:

        Ticks m_startTime;
    };

    //-------------------------------------------------------------------------

    class ScopedTimer
    {

    public:

        ScopedTimer( Milliseconds& result ) : m_result( result ) {};
        ~ScopedTimer() { m_result = m_timer.GetElapsedTimeMilliseconds(); }

    private:

        ScopedTimer() = delete;
        void operator=( ScopedTimer const& ) = delete;

    private:

        Timer               m_timer;
        Milliseconds&       m_result;
    };
}