#pragma once

//-------------------------------------------------------------------------

#ifdef KRG_SYSTEM_ENTITY

    #define KRG_SYSTEM_ENTITY_API __declspec(dllexport)

#else

    #define KRG_SYSTEM_ENTITY_API __declspec(dllimport)

#endif