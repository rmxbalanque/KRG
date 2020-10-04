#pragma once

//-------------------------------------------------------------------------

#ifdef KRG_TOOLS_ENTITY

#define KRG_TOOLS_ENTITY_API __declspec(dllexport)

#else

#define KRG_TOOLS_ENTITY_API __declspec(dllimport)

#endif