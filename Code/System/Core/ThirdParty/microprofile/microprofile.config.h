#pragma once

//-------------------------------------------------------------------------

#define MICROPROFILE_ENABLED 1
#define MICROPROFILE_GPU_TIMERS 0

//-------------------------------------------------------------------------

#if KRG_SYSTEM_CORE

#define MICROPROFILE_API __declspec(dllexport)

#else

#define MICROPROFILE_API __declspec(dllimport)

#endif