#pragma once

//-------------------------------------------------------------------------

#ifdef KRG_TOOLS_ANIMATION

#define KRG_TOOLS_ANIMATION_API __declspec(dllexport)

#else

#define KRG_TOOLS_ANIMATION_API __declspec(dllimport)

#endif