#pragma once

#include "_Module/API.h"
#include "System/Core/Settings/ConfigSettings.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        namespace Settings
        {
            extern KRG_SYSTEM_RENDER_API ConfigSettingInt     g_resolutionX;
            extern KRG_SYSTEM_RENDER_API ConfigSettingInt     g_resolutionY;
            extern KRG_SYSTEM_RENDER_API ConfigSettingInt     g_refreshRate;
            extern KRG_SYSTEM_RENDER_API ConfigSettingBool    g_fullscreen;
        }
    }
}