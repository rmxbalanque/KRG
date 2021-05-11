#pragma once

#include "_Module/API.h"
#include "System/Core/Settings/ISettings.h"
#include "System/Core/Math/Math.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class KRG_SYSTEM_RENDER_API Settings final : public ISettings
    {
    public:

        KRG_SETTINGS_ID( KRG::Render::Settings );

    protected:

        virtual bool ReadSettings( IniFile const& ini ) override;

    public:

        Int2        m_resolution = Int2( 1280, 720 );
        float       m_refreshRate = 60.0f;
        bool        m_isFullscreen = false;
    };
}