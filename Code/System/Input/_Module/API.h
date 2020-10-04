#pragma once

//-------------------------------------------------------------------------

#ifdef KRG_SYSTEM_INPUT

    #define KRG_SYSTEM_INPUT_API __declspec(dllexport)

#else

    #define KRG_SYSTEM_INPUT_API __declspec(dllimport)

#endif