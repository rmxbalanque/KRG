#pragma once

//-------------------------------------------------------------------------

#ifdef KRG_RESOURCECOMPILERS_ANIMATION

#define KRG_RESOURCECOMPILERS_ANIMATION_API __declspec(dllexport)

#else

#define KRG_RESOURCECOMPILERS_ANIMATION_API __declspec(dllimport)

#endif