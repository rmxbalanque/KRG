#include "PhysicsSettings.h"
#include "iniparser/krg_ini.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    bool Settings::ReadSettings( IniFile const& ini )
    {
        String s;
        if ( ini.TryGetString( "Physics:PhysicalMaterialDatabasePath", s ) )
        {
            if ( ResourcePath::IsValidPath( s ) )
            {
                m_physicalMaterialDatabasePath = ResourcePath( s );
            }
            else
            {
                KRG_LOG_ERROR( "Physics", "Invalid data path specified for physical material DB" );
                return false;
            }
        }
        else
        {
            KRG_LOG_ERROR( "Physics", "Failed to read physical material database path from ini file" );
            return false;
        }

        return true;
    }
}