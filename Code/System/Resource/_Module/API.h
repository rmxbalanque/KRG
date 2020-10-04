#pragma once

//-------------------------------------------------------------------------

#ifdef KRG_SYSTEM_RESOURCE

    #define KRG_SYSTEM_RESOURCE_API __declspec(dllexport)

#else
    
#define KRG_SYSTEM_RESOURCE_API __declspec(dllimport)

#endif