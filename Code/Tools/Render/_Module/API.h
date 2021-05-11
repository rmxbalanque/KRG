#pragma once

//-------------------------------------------------------------------------

#ifdef KRG_TOOLS_RENDER

#define KRG_TOOLS_RENDER_API __declspec(dllexport)

#else

#define KRG_TOOLS_RENDER_API __declspec(dllimport)

#endif