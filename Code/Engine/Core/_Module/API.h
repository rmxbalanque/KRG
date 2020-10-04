#pragma once

//-------------------------------------------------------------------------

#ifdef KRG_ENGINE_CORE

    #define KRG_ENGINE_CORE_API __declspec(dllexport)

#else

    #define KRG_ENGINE_CORE_API __declspec(dllimport)

#endif