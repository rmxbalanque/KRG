#pragma once

//-------------------------------------------------------------------------

#ifdef KRG_SYSTEM_PHYSICS

    #define KRG_SYSTEM_PHYSICS_API __declspec(dllexport)

#else

    #define KRG_SYSTEM_PHYSICS_API __declspec(dllimport)

#endif