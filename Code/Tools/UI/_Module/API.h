#pragma once

//-------------------------------------------------------------------------

#ifdef KRG_TOOLS_UI

#define KRG_TOOLS_UI_API __declspec(dllexport)

#else

#define KRG_TOOLS_UI_API __declspec(dllimport)

#endif