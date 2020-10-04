#pragma once

//-------------------------------------------------------------------------

#if KRG_SYSTEM_CORE

    #define KRG_SYSTEM_CORE_API __declspec(dllexport)

#else
    
    #define KRG_SYSTEM_CORE_API __declspec(dllimport)

#endif