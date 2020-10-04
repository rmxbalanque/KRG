#pragma once

//-------------------------------------------------------------------------

#ifdef KRG_TOOLS_CORE

#define KRG_TOOLS_CORE_API __declspec(dllexport)

#else

#define KRG_TOOLS_CORE_API __declspec(dllimport)

#endif