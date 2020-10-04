#pragma once

#include "System/Core/_Module/API.h"
#include "System/Core/ThirdParty/pcg/include/pcg_random.hpp"
#include "System/Core/Core.h"
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
            RNG( U32 seed ); // Deterministic RNG

            inline U32 GetInt( U32 min = 0, U32 max = 0xFFFFFFFF ) const
            {
                KRG_ASSERT( max > min );
                U32 range = Min( 0xFFFFFFFE, max - min );
                return min + m_rng( range + 1 );
            }

            inline F32 GetFloat( F32 min = 0.0f, F32 max = 1.0f ) const
            {
                KRG_ASSERT( max > min );
                return min + ( ( max - min ) * (F32) ldexp( m_rng(), -32 ) );
            }

        private:

            mutable pcg32 m_rng;
        };

        // Threadsafe global versions - Use in non-performance critical code
        //-------------------------------------------------------------------------

        KRG_SYSTEM_CORE_API U32 GetRandomInt( U32 min = 0, U32 max = 0xFFFFFFFF );
        KRG_SYSTEM_CORE_API F32 GetRandomFloat( F32 min = 0, F32 max = 1.0f );
    }
}