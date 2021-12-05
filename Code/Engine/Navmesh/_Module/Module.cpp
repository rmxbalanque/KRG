#include "Module.h"
#include "Engine/Core/Modules/EngineModuleContext.h"

//-------------------------------------------------------------------------

namespace KRG::Navmesh
{
    bool EngineModule::Initialize( ModuleContext& context )
    {
        context.RegisterResourceLoader( &m_navmeshLoader );
        context.RegisterSystem( m_navmeshSystem );

        //-------------------------------------------------------------------------

        m_initialized = true;
        return m_initialized;
    }

    void EngineModule::Shutdown( ModuleContext& context )
    {
        if ( m_initialized )
        {
            context.UnregisterSystem( m_navmeshSystem );
            context.UnregisterResourceLoader( &m_navmeshLoader );
        }

        //-------------------------------------------------------------------------

        m_initialized = false;
    }
}
