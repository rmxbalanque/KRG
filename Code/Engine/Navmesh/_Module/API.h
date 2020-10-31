#pragma once

//-------------------------------------------------------------------------

#ifdef KRG_ENGINE_NAVMESH

    #define KRG_ENGINE_NAVMESH_API __declspec(dllexport)

#else

    #define KRG_ENGINE_NAVMESH_API __declspec(dllimport)

#endif