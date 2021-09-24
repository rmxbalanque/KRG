#pragma once

//-------------------------------------------------------------------------

#if KRG_SYSTEM_CORE

#define ENKITS_BUILD_DLL

#else

#define ENKITS_DLL

#endif

//-------------------------------------------------------------------------

#define ENKITS_TASK_PRIORITIES_NUM 5

//-------------------------------------------------------------------------

#include "System/Core/Defines.h"
#define ENKI_ASSERT KRG_ASSERT