#pragma once

//-------------------------------------------------------------------------

#ifdef KRG_ENGINE_ANIMATION

    #define KRG_ENGINE_ANIMATION_API __declspec(dllexport)

#else

    #define KRG_ENGINE_ANIMATION_API __declspec(dllimport)

#endif