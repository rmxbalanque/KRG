#pragma once

//-------------------------------------------------------------------------

#if KRG_DLL
    #ifdef KRG_ENGINE_NAVMESH
        #define KRG_ENGINE_NAVMESH_API __declspec(dllexport)
    #else
        #define KRG_ENGINE_NAVMESH_API __declspec(dllimport)
    #endif
#else
    #define KRG_ENGINE_NAVMESH_API
#endif