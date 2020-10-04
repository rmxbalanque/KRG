#pragma once

//-------------------------------------------------------------------------

#ifdef KRG_RESOURCECOMPILERS_PHYSICS

#define KRG_RESOURCECOMPILERS_PHYSICS_API __declspec(dllexport)

#else

#define KRG_RESOURCECOMPILERS_PHYSICS_API __declspec(dllimport)

#endif