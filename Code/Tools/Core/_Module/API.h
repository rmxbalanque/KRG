#pragma once

//-------------------------------------------------------------------------

#if KRG_DLL
    #ifdef KRG_TOOLS_CORE
        #define KRG_TOOLS_CORE_API __declspec(dllexport)
    #else
        #define KRG_TOOLS_CORE_API __declspec(dllimport)
    #endif
#else
    #define KRG_TOOLS_CORE_API
#endif