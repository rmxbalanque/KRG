#pragma once

//-------------------------------------------------------------------------

#ifdef KRG_ENGINE_PHYSICS

    #define KRG_ENGINE_PHYSICS_API __declspec(dllexport)

#else

    #define KRG_ENGINE_PHYSICS_API __declspec(dllimport)

#endif