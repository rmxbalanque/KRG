#pragma once

//-------------------------------------------------------------------------

#if KRG_DLL
    #ifdef KRG_ENGINE_ANIMATION
        #define KRG_ENGINE_ANIMATION_API __declspec(dllexport)
    #else
        #define KRG_ENGINE_ANIMATION_API __declspec(dllimport)
    #endif
#else
    #define KRG_ENGINE_ANIMATION_API
#endif