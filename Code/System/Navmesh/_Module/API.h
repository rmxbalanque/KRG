#pragma once

//-------------------------------------------------------------------------

#ifdef KRG_SYSTEM_NAVMESH

    #define KRG_SYSTEM_NAVMESH_API __declspec(dllexport)

#else

#define KRG_SYSTEM_NAVMESH_API __declspec(dllimport)

#endif