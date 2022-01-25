#pragma once

//-------------------------------------------------------------------------

#if KRG_DLL
    #ifdef KRG_TOOLS_ANIMATION
        #define KRG_TOOLS_ANIMATION_API __declspec(dllexport)
    #else
        #define KRG_TOOLS_ANIMATION_API __declspec(dllimport)
    #endif
#else
    #define KRG_TOOLS_ANIMATION_API
#endif