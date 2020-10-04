#pragma once

//-------------------------------------------------------------------------

#ifdef KRG_RESOURCECOMPILERS_ENTITY

#define KRG_RESOURCECOMPILERS_ENTITY_API __declspec(dllexport)

#else

#define KRG_RESOURCECOMPILERS_ENTITY_API __declspec(dllimport)

#endif