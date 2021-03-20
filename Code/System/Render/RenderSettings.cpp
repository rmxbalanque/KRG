#include "RenderSettings.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        namespace Settings
        {
            ConfigSettingInt     g_resolutionX( "ResolutionX", "Render", 1280, 640, 10000 );
            ConfigSettingInt     g_resolutionY( "ResolutionY", "Render", 720, 320, 10000 );
            ConfigSettingInt     g_refreshRate( "RefreshRate", "Render", 60, 59, 240 );
            ConfigSettingBool    g_fullscreen( "Fullscreen", "Render", false );
        }
    }
}