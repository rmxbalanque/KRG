#pragma once

//-------------------------------------------------------------------------

#ifdef KRG_ENGINE_RENDER

    #define KRG_ENGINE_RENDER_API __declspec(dllexport)

#else

    #define KRG_ENGINE_RENDER_API __declspec(dllimport)

#endif