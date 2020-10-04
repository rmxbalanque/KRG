#pragma once

//-------------------------------------------------------------------------

#ifdef KRG_RESOURCECOMPILERS_RENDER

#define KRG_RESOURCECOMPILERS_RENDER_API __declspec(dllexport)

#else

#define KRG_RESOURCECOMPILERS_RENDER_API __declspec(dllimport)

#endif