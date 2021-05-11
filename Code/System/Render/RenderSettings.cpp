#include "RenderSettings.h"
#include "iniparser/krg_ini.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    bool Settings::ReadSettings( IniFile const& ini )
    {
        m_resolution.m_x = ini.GetIntOrDefault( "Render:ResolutionX", 1280 );
        m_resolution.m_y = ini.GetIntOrDefault( "Render:ResolutionX", 720 );
        m_refreshRate = ini.GetFloatOrDefault( "Render:ResolutionX", 60 );
        m_isFullscreen = ini.GetBoolOrDefault( "Render:Fullscreen", false );

        //-------------------------------------------------------------------------

        if ( m_resolution.m_x < 0 || m_resolution.m_y < 0 || m_refreshRate < 0 )
        {
            KRG_LOG_ERROR( "Render", "Invalid render settings read from ini file." );
            return false;
        }

        //-------------------------------------------------------------------------

        return true;
    }
}