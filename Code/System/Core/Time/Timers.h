#pragma once

#include "System/Core/Time/Time.h"

//-------------------------------------------------------------------------

namespace KRG
{
    //-------------------------------------------------------------------------
    // Timers
    //-------------------------------------------------------------------------

    // The system timer uses the system clock for all timing (i.e. the actual platform/OS time)
    class SystemTimer
    {
    public:

        SystemTimer() { Start(); }

        inline void Start() { m_startTime = SystemClock::GetTime(); }
        inline void Reset() { Start(); }

        inline Nanoseconds GetElapsedTimeNanoseconds() const { return SystemClock::GetTime() - m_startTime; }
        inline Seconds GetElapsedTimeSeconds() const { return GetElapsedTimeNanoseconds().ToSeconds(); }
        inline Milliseconds GetElapsedTimeMilliseconds() const { return GetElapsedTimeNanoseconds().ToMilliseconds(); }
        inline Microseconds GetElapsedTimeMicroseconds() const { return GetElapsedTimeNanoseconds().ToMicroseconds(); }

    private:

        Nanoseconds m_startTime;
    };

    //-------------------------------------------------------------------------

    // The engine timer uses the manually updated engine time (i.e. the accumulated delta time)
    class EngineTimer
    {
    public:

        EngineTimer() { Start(); }

        inline void Start() { m_startTime = EngineClock::GetTime(); }
        inline void Reset() { Start(); }

        inline Nanoseconds GetElapsedTimeNanoseconds() const { return EngineClock::GetTime() - m_startTime; }
        inline Seconds GetElapsedTimeSeconds() const { return GetElapsedTimeNanoseconds().ToSeconds(); }
        inline Milliseconds GetElapsedTimeMilliseconds() const { return GetElapsedTimeNanoseconds().ToMilliseconds(); }
        inline Microseconds GetElapsedTimeMicroseconds() const { return GetElapsedTimeNanoseconds().ToMicroseconds(); }

    private:

        Nanoseconds m_startTime;
    };

    //-------------------------------------------------------------------------
    // Scoped Timers
    //-------------------------------------------------------------------------

    class ScopedSystemTimer
    {

    public:

        ScopedSystemTimer( Milliseconds& result ) : m_result( result ) {};
        ~ScopedSystemTimer() { m_result = m_timer.GetElapsedTimeMilliseconds(); }

    private:

        ScopedSystemTimer() = delete;
        void operator=( ScopedSystemTimer const& ) = delete;

    private:

        SystemTimer         m_timer;
        Milliseconds&       m_result;
    };

    //-------------------------------------------------------------------------

    class ScopedEngineTimer
    {

    public:

        ScopedEngineTimer( Milliseconds& result ) : m_result( result ) {};
        ~ScopedEngineTimer() { m_result = m_timer.GetElapsedTimeMilliseconds(); }

    private:

        ScopedEngineTimer() = delete;
        void operator=( ScopedEngineTimer const& ) = delete;

    private:

        EngineTimer         m_timer;
        Milliseconds&       m_result;
    };
}