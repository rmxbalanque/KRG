#pragma once

#include "System/Core/Algorithm/Hash.h"
#include "System/Core/ThirdParty/microprofile/microprofile.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Profiling
    {
        KRG_SYSTEM_CORE_API void Initialize();
        KRG_SYSTEM_CORE_API void Shutdown();
        KRG_SYSTEM_CORE_API void StartFrame();
        KRG_SYSTEM_CORE_API void EndFrame();
    }
}

//-------------------------------------------------------------------------

#define KRG_PROFILE_THREAD_START( ThreadName )\
    MicroProfileOnThreadCreate( ThreadName )

#define KRG_PROFILE_THREAD_END()\
    MicroProfileOnThreadExit();

//-------------------------------------------------------------------------

#define KRG_PROFILE_SCOPE( tag )\
    MICROPROFILE_SCOPEI( "KRG", tag, MP_DARKOLIVEGREEN )

#define KRG_PROFILE_SCOPE_COLOR( tag, color )\
    MICROPROFILE_SCOPEI( "KRG", tag, color )

#define KRG_PROFILE_GROUPED_SCOPE_COLOR( group, tag, color )\
    MICROPROFILE_SCOPEI( group, tag, color )