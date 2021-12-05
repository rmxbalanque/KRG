#include "Random.h"
#include <random>
#include "System/Core/Threading/Threading.h"

//-------------------------------------------------------------------------

namespace KRG::Math
{
    RNG::RNG()
    {
        m_rng.seed( pcg_extras::seed_seq_from<std::random_device>() );
    }

    RNG::RNG( uint32 seed )
        : m_rng( seed )
    {
        KRG_ASSERT( seed != 0 );
    }

    //-------------------------------------------------------------------------

    namespace
    {
        RNG                 g_rng;
        Threading::Mutex    g_globalRandomMutex;
    }

    bool GetRandomBool()
    {
        Threading::ScopeLock lock( g_globalRandomMutex );
        return g_rng.GetInt( 0, 1 ) == 1;
    }

    uint32 GetRandomInt( uint32 min, uint32 max )
    {
        Threading::ScopeLock lock( g_globalRandomMutex );
        return g_rng.GetInt( min, max );
    }

    float GetRandomFloat( float min, float max )
    {
        Threading::ScopeLock lock( g_globalRandomMutex );
        return g_rng.GetFloat( min, max );
    }
}
