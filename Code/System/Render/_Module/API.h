#pragma once

//-------------------------------------------------------------------------

#if KRG_DLL
    #ifdef KRG_SYSTEM_RENDER
        #define KRG_SYSTEM_RENDER_API __declspec(dllexport)
    #else
        #define KRG_SYSTEM_RENDER_API __declspec(dllimport)
    #endif
#else
    #define KRG_SYSTEM_RENDER_API
#endif