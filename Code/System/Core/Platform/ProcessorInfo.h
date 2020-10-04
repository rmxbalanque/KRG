#pragma once

#include "System/Core/_Module/API.h"
#include "System/Core/Core.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Platform
    {
        struct ProcessorInfo
        {
            U16 m_numPhysicalCores = 0;
            U16 m_numLogicalCores = 0;
        };

        KRG_SYSTEM_CORE_API ProcessorInfo GetProcessorInfo();
    }
}