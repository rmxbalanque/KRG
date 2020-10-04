#pragma once

//-------------------------------------------------------------------------

#ifdef KRG_SYSTEM_ANIMATION

    #define KRG_SYSTEM_ANIMATION_API __declspec(dllexport)

#else

    #define KRG_SYSTEM_ANIMATION_API __declspec(dllimport)

#endif