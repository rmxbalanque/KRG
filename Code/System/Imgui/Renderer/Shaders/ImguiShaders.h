#pragma once

#include "System/Core/Core.h"

//-------------------------------------------------------------------------

#ifdef _WIN32

    typedef unsigned char BYTE;

    #ifdef KRG_DEBUG
        #include "_AutoGenerated/PS_imgui_x64_Debug.h"
        #include "_AutoGenerated/VS_imgui_x64_Debug.h"
    #else
        #include "_AutoGenerated/PS_imgui_x64_Release.h"
        #include "_AutoGenerated/VS_imgui_x64_Release.h"
    #endif
#endif