#pragma once

//-------------------------------------------------------------------------

#if KRG_DLL
    #ifdef KRG_ENGINE_CORE
        #define KRG_ENGINE_CORE_API __declspec(dllexport)
    #else
        #define KRG_ENGINE_CORE_API __declspec(dllimport)
    #endif
#else
    #define KRG_ENGINE_CORE_API
#endif