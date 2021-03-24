#pragma once

//-------------------------------------------------------------------------

#ifdef KRG_SYSTEM_DEVTOOLS

    #define KRG_SYSTEM_DEVTOOLS_API __declspec(dllexport)

#else
    
#define KRG_SYSTEM_DEVTOOLS_API __declspec(dllimport)

#endif