#pragma once

//-------------------------------------------------------------------------

#ifdef KRG_TOOLS_RESOURCE

#define KRG_TOOLS_RESOURCE_API __declspec(dllexport)

#else

#define KRG_TOOLS_RESOURCE_API __declspec(dllimport)

#endif