#pragma once

#include "System/Core/Time/Time.h"
#include "System/Core/Types/Percentage.h"

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

    // The system cool down timer uses the system clock for all timing (i.e. the actual platform/OS time)
    class SystemCooldownTimer
    {
    public:

        SystemCooldownTimer() {}

        inline void Start( Seconds cooldownTime )
        {
            m_startTime = SystemClock::GetTime();
            m_requiredTime = cooldownTime;
            m_isRunning = true;
        }

        inline bool Update()
        {
            KRG_ASSERT( m_isRunning );

            if ( GetElapsedTimeSeconds() > m_requiredTime )
            {
                m_isRunning = false;
                return true;
            }

            return false;
        }

        inline void Stop()
        {
            m_requiredTime = 0.0f;
            m_isRunning = false;
        }

        inline bool IsRunning() const { return m_isRunning; }
        inline bool IsComplete() const { return !m_isRunning; }

        inline Seconds GetRemainingTime() const
        {
            KRG_ASSERT( m_isRunning );
            return Math::Max( 0.0f, (float) ( m_requiredTime - GetElapsedTimeSeconds() ) );
        }

        inline Percentage GetPercentageThrough() const
        {
            return Percentage( GetElapsedTimeSeconds() / m_requiredTime ).GetClamped( false );
        }

    private:

        inline Seconds GetElapsedTimeSeconds() const { return ( SystemClock::GetTime() - m_startTime ).ToSeconds(); }

    private:

        Nanoseconds     m_startTime;
        Seconds         m_requiredTime;
        bool            m_isRunning = false;
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

    // The engine cool down timer uses the manually updated engine time (i.e. the accumulated delta time)
    class EngineCooldownTimer
    {
    public:

        EngineCooldownTimer() {}

        inline void Start( Seconds cooldownTime )
        {
            m_startTime = EngineClock::GetTime();
            m_requiredTime = cooldownTime;
            m_isRunning = true;
        }

        inline bool Update()
        {
            KRG_ASSERT( m_isRunning );

            if ( GetElapsedTimeSeconds() > m_requiredTime )
            {
                m_isRunning = false;
                return true;
            }

            return false;
        }

        inline void Stop()
        {
            m_requiredTime = 0.0f;
            m_isRunning = false;
        }

        inline bool IsRunning() const { return m_isRunning; }
        inline bool IsComplete() const { return !m_isRunning; }

        inline Seconds GetRemainingTime() const
        {
            KRG_ASSERT( m_isRunning );
            return Math::Max( 0.0f, (float) ( m_requiredTime - GetElapsedTimeSeconds() ) );
        }

        inline Percentage GetPercentageThrough() const
        {
            return Percentage( GetElapsedTimeSeconds() / m_requiredTime ).GetClamped( false );
        }

    private:

        inline Seconds GetElapsedTimeSeconds() const { return ( EngineClock::GetTime() - m_startTime ).ToSeconds(); }

    private:

        Nanoseconds     m_startTime;
        Seconds         m_requiredTime;
        bool            m_isRunning = false;
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