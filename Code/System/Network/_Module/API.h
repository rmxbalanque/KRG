#pragma once

//-------------------------------------------------------------------------

#if KRG_DLL
    #ifdef KRG_SYSTEM_NETWORK
        #define KRG_SYSTEM_NETWORK_API __declspec(dllexport)
    #else
        #define KRG_SYSTEM_NETWORK_API __declspec(dllimport)
    #endif
#else
    #define KRG_SYSTEM_NETWORK_API
#endif