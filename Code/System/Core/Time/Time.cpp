#include "Time.h"
#include <chrono>

//-------------------------------------------------------------------------

namespace KRG
{
    KRG::Nanoseconds EngineClock::CurrentTime = 0;

    //-------------------------------------------------------------------------

    Nanoseconds::operator Microseconds() const
    {
        auto const duration = std::chrono::duration<uint64, std::chrono::steady_clock::period>( m_value );
        uint64 const numMicroseconds = std::chrono::duration_cast<std::chrono::microseconds>( duration ).count();
        return float( numMicroseconds );
    }

    //-------------------------------------------------------------------------

    Nanoseconds SystemClock::GetTime()
    {
        auto const time = std::chrono::high_resolution_clock::now();
        uint64 const numNanosecondsSinceEpoch = time.time_since_epoch().count();
        return Nanoseconds( numNanosecondsSinceEpoch );
    }

    //-------------------------------------------------------------------------

    void EngineClock::Update( Milliseconds deltaTime )
    {
        KRG_ASSERT( deltaTime >= 0 );
        CurrentTime += deltaTime.ToNanoseconds();
    }
}