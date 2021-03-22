#pragma once

//-------------------------------------------------------------------------

#ifdef KRG_ENGINE_CAMERA

    #define KRG_ENGINE_CAMERA_API __declspec(dllexport)

#else

    #define KRG_ENGINE_CAMERA_API __declspec(dllimport)

#endif