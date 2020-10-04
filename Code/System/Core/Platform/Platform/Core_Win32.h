#pragma once
#ifdef _WIN32

//-------------------------------------------------------------------------

#ifndef NOMINMAX
    #define NOMINMAX
#endif

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

//-------------------------------------------------------------------------

#define KRG_LITTLE_ENDIAN
#define KRG_FORCE_INLINE __forceinline

//-------------------------------------------------------------------------

#define KRG_ASSERT( msg ) (!(msg)) ? __debugbreak() : 0
#define KRG_TRACE_MSG_WIN32( msg ) OutputDebugStringA( msg ); OutputDebugStringA("\r\n")
#define KRG_BREAK() __debugbreak()
#define KRG_HALT() __debugbreak()

//-------------------------------------------------------------------------

#define KRG_DISABLE_OPTIMIZATION __pragma( optimize( "", off ) )
#define KRG_ENABLE_OPTIMIZATION __pragma( optimize( "", on ) )

// Enable specific warnings
//-------------------------------------------------------------------------

#pragma warning(default:4800)
#pragma warning(default:4389)

#endif
