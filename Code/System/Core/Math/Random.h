#pragma once

#include "System/Core/_Module/API.h"
#include "System/Core/ThirdParty/pcg/include/pcg_random.hpp"
#include "System/Core/Core/IntegralTypes.h"
#include "System/Core/Core/Defines.h"
#include "Math.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Math
    {
        // Non-threadsafe random number generator based on PCG
        //-------------------------------------------------------------------------

        class KRG_SYSTEM_CORE_API RNG
        {

        public:

            RNG(); // Non-deterministic RNG
            RNG( uint32 seed ); // Deterministic RNG

            inline uint32 GetInt( uint32 min = 0, uint32 max = 0xFFFFFFFF ) const
            {
                KRG_ASSERT( max > min );
                uint32 range = Min( 0xFFFFFFFE, max - min );
                return min + m_rng( range + 1 );
            }

            inline float GetFloat( float min = 0.0f, float max = 1.0f ) const
            {
                KRG_ASSERT( max > min );
                return min + ( ( max - min ) * (float) ldexp( m_rng(), -32 ) );
            }

        private:

            mutable pcg32 m_rng;
        };

        // Threadsafe global versions - Use in non-performance critical code
        //-------------------------------------------------------------------------

        // Get a random unsigned integer value between [min, max]
        KRG_SYSTEM_CORE_API bool GetRandomBool();

        // Get a random unsigned integer value between [min, max]
        KRG_SYSTEM_CORE_API uint32 GetRandomInt( uint32 min = 0, uint32 max = 0xFFFFFFFF );

        // Get a random float value between [min, max]
        KRG_SYSTEM_CORE_API float GetRandomFloat( float min = 0, float max = 1.0f );
    }
}