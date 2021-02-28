#pragma once

#include "Defines.h"
#include <stdint.h>

//-------------------------------------------------------------------------

namespace KRG
{
    typedef int8_t              S8;
    typedef uint8_t             U8;
    typedef int16_t             S16;
    typedef uint16_t            U16;
    typedef int32_t             S32;
    typedef uint32_t            U32;
    typedef int64_t             S64;
    typedef uint64_t            U64;
    typedef float               F32;
    typedef double              F64;
    typedef unsigned char       Byte;

    //-------------------------------------------------------------------------

    #define InvalidIndex -1

    //-------------------------------------------------------------------------

    namespace Platform
    {
        enum class Target
        {
            PC = 0,
        };
    }
}