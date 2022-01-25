#pragma once

//-------------------------------------------------------------------------

#if KRG_DLL
    #ifdef KRG_TOOLS_ENTITY
        #define KRG_TOOLS_ENTITY_API __declspec(dllexport)
    #else
        #define KRG_TOOLS_ENTITY_API __declspec(dllimport)
    #endif
#else
    #define KRG_TOOLS_ENTITY_API
#endif