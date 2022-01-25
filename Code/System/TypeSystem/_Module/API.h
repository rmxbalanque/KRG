#pragma once

//-------------------------------------------------------------------------

#if KRG_DLL
    #ifdef KRG_SYSTEM_TYPESYSTEM
        #define KRG_SYSTEM_TYPESYSTEM_API __declspec(dllexport)
    #else
        #define KRG_SYSTEM_TYPESYSTEM_API __declspec(dllimport)
    #endif
#else
    #define KRG_SYSTEM_TYPESYSTEM_API
#endif