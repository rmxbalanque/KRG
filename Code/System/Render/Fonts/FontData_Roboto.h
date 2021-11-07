#pragma once
#include "../_Module/API.h"
#include "System/Core/Types/IntegralTypes.h"

//-------------------------------------------------------------------------

namespace KRG::Fonts::Roboto
{
    namespace Regular
    {
        KRG_SYSTEM_RENDER_API Byte const* GetData();
    }

    namespace Medium
    {
        KRG_SYSTEM_RENDER_API Byte const* GetData();
    }

    namespace Bold
    {
        KRG_SYSTEM_RENDER_API Byte const* GetData();
    }
};