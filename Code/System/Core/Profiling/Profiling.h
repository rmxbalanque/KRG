#pragma once

#include "System/Core/Algorithm/Hash.h"
#include <optick.h>

//-------------------------------------------------------------------------

namespace KRG
{
    namespace FileSystem { class Path; }

    //-------------------------------------------------------------------------

    namespace Profiling
    {
        KRG_SYSTEM_CORE_API void StartFrame();
        KRG_SYSTEM_CORE_API void EndFrame();

        //-------------------------------------------------------------------------

        // Open the profiler application (only available on Win64)
        KRG_SYSTEM_CORE_API void OpenProfiler();

        // Capture management
        KRG_SYSTEM_CORE_API void StartCapture();
        KRG_SYSTEM_CORE_API void StopCapture( FileSystem::Path const& captureSavePath );
    }
}

//-------------------------------------------------------------------------

#define KRG_PROFILE_THREAD_START( ThreadName ) OPTICK_START_THREAD( ThreadName )

#define KRG_PROFILE_THREAD_END() OPTICK_STOP_THREAD()

// Generic scopes
//-------------------------------------------------------------------------

#define KRG_PROFILE_FUNCTION() OPTICK_EVENT()
#define KRG_PROFILE_SCOPE( name ) OPTICK_EVENT( name )

// Tags
//-------------------------------------------------------------------------

#define KRG_PROFILE_TAG( name, value ) OPTICK_TAG( name, value )

// Waits
//-------------------------------------------------------------------------

#define KRG_PROFILE_WAIT( name ) OPTICK_EVENT( name, Optick::Category::Wait )

// Category scopes
//-------------------------------------------------------------------------

#define KRG_PROFILE_FUNCTION_AI() OPTICK_EVENT( OPTICK_FUNC, Optick::Category::AI )
#define KRG_PROFILE_FUNCTION_ANIMATION() OPTICK_EVENT( OPTICK_FUNC, Optick::Category::Animation )
#define KRG_PROFILE_FUNCTION_CAMERA() OPTICK_EVENT( OPTICK_FUNC, Optick::Category::Camera )
#define KRG_PROFILE_FUNCTION_GAMEPLAY() OPTICK_EVENT( OPTICK_FUNC, Optick::Category::GameLogic )
#define KRG_PROFILE_FUNCTION_IO() OPTICK_EVENT( OPTICK_FUNC, Optick::Category::IO )
#define KRG_PROFILE_FUNCTION_NAVIGATION() OPTICK_EVENT( OPTICK_FUNC, Optick::Category::Navigation )
#define KRG_PROFILE_FUNCTION_PHYSICS() OPTICK_EVENT( OPTICK_FUNC, Optick::Category::Physics )
#define KRG_PROFILE_FUNCTION_RENDER() OPTICK_EVENT( OPTICK_FUNC, Optick::Category::Rendering )
#define KRG_PROFILE_FUNCTION_SCENE() OPTICK_EVENT( OPTICK_FUNC, Optick::Category::Scene )
#define KRG_PROFILE_FUNCTION_RESOURCE() OPTICK_EVENT( OPTICK_FUNC, Optick::Category::Streaming )
#define KRG_PROFILE_FUNCTION_NETWORK() OPTICK_EVENT( OPTICK_FUNC, Optick::Category::Network )

#define KRG_PROFILE_SCOPE_AI( name ) OPTICK_EVENT( name, Optick::Category::AI )
#define KRG_PROFILE_SCOPE_ANIMATION( name ) OPTICK_EVENT( name, Optick::Category::Animation )
#define KRG_PROFILE_SCOPE_CAMERA( name) OPTICK_EVENT( OPTICK_FUNC, Optick::Category::Camera )
#define KRG_PROFILE_SCOPE_GAMEPLAY( name) OPTICK_EVENT( OPTICK_FUNC, Optick::Category::GameLogic )
#define KRG_PROFILE_SCOPE_IO( name ) OPTICK_EVENT( name, Optick::Category::IO )
#define KRG_PROFILE_SCOPE_NAVIGATION( name ) OPTICK_EVENT( name, Optick::Category::Navigation )
#define KRG_PROFILE_SCOPE_PHYSICS( name ) OPTICK_EVENT( name, Optick::Category::Physics )
#define KRG_PROFILE_SCOPE_RENDER( name ) OPTICK_EVENT( name, Optick::Category::Rendering )
#define KRG_PROFILE_SCOPE_SCENE( name ) OPTICK_EVENT( name, Optick::Category::Scene )
#define KRG_PROFILE_SCOPE_RESOURCE( name ) OPTICK_EVENT( name, Optick::Category::Streaming )
#define KRG_PROFILE_SCOPE_NETWORK( name ) OPTICK_EVENT( name, Optick::Category::Network )