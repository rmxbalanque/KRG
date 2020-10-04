#pragma once
//-------------------------------------------------------------------------

#ifdef KRG_GAME_CORE

    #define KRG_GAME_CORE_API __declspec(dllexport)

#else

    #define KRG_GAME_CORE_API __declspec(dllimport)

#endif