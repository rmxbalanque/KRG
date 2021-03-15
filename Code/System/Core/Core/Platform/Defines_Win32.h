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

#define KRG_LITTLE_ENDIAN
#define KRG_FORCE_INLINE __forceinline

//-------------------------------------------------------------------------
// Enable specific warnings
//-------------------------------------------------------------------------

#pragma warning(default:4800)
#pragma warning(default:4389)

//-------------------------------------------------------------------------
// Debug defines
//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS

#define KRG_DISABLE_OPTIMIZATION __pragma( optimize( "", off ) )
#define KRG_ENABLE_OPTIMIZATION __pragma( optimize( "", on ) )

#define KRG_TRACE_MSG( msg ) OutputDebugStringA( msg ); OutputDebugStringA("\r\n")
#define KRG_ASSERT( cond ) do { if( !(cond) ) { KRG_TRACE_MSG( "KRG Assert fired: " #cond " (" KRG_FILE_LINE ")"  ); __debugbreak(); } } while( 0 )
#define KRG_BREAK() __debugbreak()
#define KRG_HALT() __debugbreak()

#endif

//-------------------------------------------------------------------------

#endif