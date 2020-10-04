#include "Hash.h"
#include <xxhash/xxhash.h>

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Hash
    {
        namespace XXHash
        {
            constexpr static U32 const g_hashSeed = 'KRG8';
        }

        U32 XXHash::GetHash32( void const* pData, size_t size )
        {
            return XXH32( pData, size, g_hashSeed );
        }

        U64 XXHash::GetHash64( void const* pData, size_t size )
        {
            return XXH64( pData, size, g_hashSeed );
        }
    }
}