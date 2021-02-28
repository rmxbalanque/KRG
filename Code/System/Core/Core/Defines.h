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

//-------------------------------------------------------------------------
// Development
//-------------------------------------------------------------------------
// Are the debug/development tools compiled into the build

#if !KRG_CONFIGURATION_FINAL
#define KRG_DEVELOPMENT_TOOLS 1
#endif

//-------------------------------------------------------------------------
// Platform specific defines
//-------------------------------------------------------------------------

#include "Platform/Defines_Win32.h"

//-------------------------------------------------------------------------
// Debug defines
//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS

    // Platform agnostic asserts
    //-------------------------------------------------------------------------

    #define KRG_STATIC_ASSERT( cond, error ) static_assert( cond, error )
    #define KRG_TRACE_MSG( msg ) { KRG_TRACE_MSG_WIN32( msg ); }
    #define KRG_TRACE_ASSERT( msg ) { KRG_TRACE_MSG( msg ); KRG_HALT(); }
    #define KRG_UNIMPLEMENTED_FUNCTION() KRG_TRACE_ASSERT("Function not implemented!\n")
    #define KRG_UNREACHABLE_CODE() KRG_TRACE_ASSERT("Unreachable code encountered!\n")

#else

    // Platform specific, need to be defined in Platform/Defines_XXX.h

    #define KRG_ASSERT( cond )
    #define KRG_TRACE_MSG_WIN32( msg )
    #define KRG_BREAK()
    #define KRG_HALT()

    #define KRG_DISABLE_OPTIMIZATION
    #define KRG_ENABLE_OPTIMIZATION

    // Platform agnostic asserts
    //-------------------------------------------------------------------------

    #define KRG_STATIC_ASSERT( cond, error )
    #define KRG_TRACE_MSG( msg ) 
    #define KRG_TRACE_ASSERT( msg )
    #define KRG_UNIMPLEMENTED_FUNCTION()
    #define KRG_UNREACHABLE_CODE()
#endif