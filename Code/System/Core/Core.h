#pragma once

//-------------------------------------------------------------------------
// Configurations
//-------------------------------------------------------------------------

// KRG_CONFIGURATION_DEBUG = unoptimized build with debug info (debug drawing, string debug names, etc...)
// KRG_CONFIGURATION_RELEASE = optimized build with debug info (debug drawing, string debug names, etc...)
// KRG_CONFIGURATION_FINAL = optimized build with no debug info

#ifdef KRG_DEBUG
    #define KRG_CONFIGURATION_DEBUG 1
#elif !KRG_CONFIGURATION_FINAL
    #define KRG_CONFIGURATION_RELEASE 1
#endif

#if !KRG_CONFIGURATION_FINAL
#define KRG_DEBUG_INSTRUMENTATION 1
#endif

//-------------------------------------------------------------------------
// Utilities
//-------------------------------------------------------------------------

// Used when trying to concatenate token macros like __LINE__, __FILE__, etc...
#define KRG_MACRO_TOKEN_CONCAT2(x, y) x ## y
#define KRG_MACRO_TOKEN_CONCAT(x, y) KRG_MACRO_TOKEN_CONCAT2(x, y)
#define KRG_MACRO_UNIQUE_NAME( x ) KRG_MACRO_TOKEN_CONCAT( x, __COUNTER__ )

//-------------------------------------------------------------------------
// Core
//-------------------------------------------------------------------------

#include <stdint.h>

namespace KRG
{
    // Core Types
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
            PC,
            XboxOne,
            PS4,
        };
    }
}

//-------------------------------------------------------------------------
// Build And Macros
//-------------------------------------------------------------------------

#ifdef _WIN32
    #include "Platform/Platform/Core_Win32.h"
#endif

//-------------------------------------------------------------------------

#define KRG_STATIC_ASSERT( cond, error ) static_assert( cond, error )
#define KRG_TRACE_MSG( msg ) { KRG_TRACE_MSG_WIN32( msg ); }
#define KRG_TRACE_ASSERT( msg ) { KRG_TRACE_MSG( msg ); KRG_HALT(); }
#define KRG_UNIMPLEMENTED_FUNCTION() KRG_TRACE_ASSERT("Function not implemented!\n")
#define KRG_UNREACHABLE_CODE() KRG_TRACE_ASSERT("Unreachable code encountered!\n")