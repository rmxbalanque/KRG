#pragma once

//-------------------------------------------------------------------------

#if KRG_DLL
    #ifdef KRG_SYSTEM_INPUT
        #define KRG_SYSTEM_INPUT_API __declspec(dllexport)
    #else
        #define KRG_SYSTEM_INPUT_API __declspec(dllimport)
    #endif
#else
    #define KRG_SYSTEM_INPUT_API
#endif