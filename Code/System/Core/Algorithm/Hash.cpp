#include "Hash.h"

#define XXH_INLINE_ALL
#include <xxhash/xxhash.h>

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Hash
    {
        namespace XXHash
        {
            constexpr static uint32 const g_hashSeed = 'KRG8';
        }

        uint32 XXHash::GetHash32( void const* pData, size_t size )
        {
            return XXH32( pData, size, g_hashSeed );
        }

        uint64 XXHash::GetHash64( void const* pData, size_t size )
        {
            return XXH64( pData, size, g_hashSeed );
        }
    }
}