#pragma once

//-------------------------------------------------------------------------

#ifdef KRG_TOOLS_EDITOR

#define KRG_TOOLS_EDITOR_API __declspec(dllexport)

#else

#define KRG_TOOLS_EDITOR_API __declspec(dllimport)

#endif