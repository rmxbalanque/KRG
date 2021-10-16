#pragma once

#include "_Module/API.h"
#include "System/Resource/ResourcePath.h"
#include "System/Core/Settings/ISettings.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class KRG_ENGINE_PHYSICS_API Settings final : public ISettings
    {
    public:

        KRG_SETTINGS_ID( KRG::Physics::Settings );

    protected:

        virtual bool ReadSettings( IniFile const& ini ) override;

    public:

        ResourcePath      m_physicalMaterialDatabasePath;
    };
}