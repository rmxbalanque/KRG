#include "Time.h"
#include <chrono>

//-------------------------------------------------------------------------

namespace KRG
{
    Ticks Ticks::Now()
    {
        auto const time = std::chrono::high_resolution_clock::now();
        U64 const numTicks = time.time_since_epoch().count();
        return Ticks( numTicks );
    }

    Ticks::operator Microseconds() const
    {
        auto const duration = std::chrono::duration<U64, std::chrono::steady_clock::period>( m_value );
        U64 const numMicroseconds = std::chrono::duration_cast<std::chrono::microseconds>( duration ).count();
        return F32( numMicroseconds );
    }
}