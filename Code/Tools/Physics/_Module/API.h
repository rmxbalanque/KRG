#pragma once

//-------------------------------------------------------------------------

#ifdef KRG_TOOLS_PHYSICS

#define KRG_TOOLS_PHYSICS_API __declspec(dllexport)

#else

#define KRG_TOOLS_PHYSICS_API __declspec(dllimport)

#endif