#pragma once

//-------------------------------------------------------------------------

#if KRG_DLL
    #ifdef KRG_SYSTEM_RESOURCE
        #define KRG_SYSTEM_RESOURCE_API __declspec(dllexport)
    #else
        #define KRG_SYSTEM_RESOURCE_API __declspec(dllimport)
    #endif
#else
    #define KRG_SYSTEM_RESOURCE_API
#endif