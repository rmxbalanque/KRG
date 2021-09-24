#pragma once

//-------------------------------------------------------------------------

#ifdef KRG_SYSTEM_IMGUI

    #define KRG_SYSTEM_IMGUI_API __declspec(dllexport)

#else
    
#define KRG_SYSTEM_IMGUI_API __declspec(dllimport)

#endif