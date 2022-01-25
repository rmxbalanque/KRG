#include "Module.h"
#include "Engine/Core/Modules/EngineModuleContext.h"

//-------------------------------------------------------------------------

namespace KRG::Navmesh
{
    bool EngineModule::Initialize( ModuleContext& context )
    {
        context.RegisterResourceLoader( &m_navmeshLoader );

        #if KRG_NAVPOWER
        context.RegisterSystem( m_navmeshSystem );
        #endif

        //-------------------------------------------------------------------------

        m_initialized = true;
        return m_initialized;
    }

    void EngineModule::Shutdown( ModuleContext& context )
    {
        if ( m_initialized )
        {
            #if KRG_NAVPOWER
            context.UnregisterSystem( m_navmeshSystem );
            #endif

            context.UnregisterResourceLoader( &m_navmeshLoader );
        }

        //-------------------------------------------------------------------------

        m_initialized = false;
    }
}
