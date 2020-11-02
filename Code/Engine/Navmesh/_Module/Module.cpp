#include "Module.h"
#include "Engine/Core/Modules/EngineModuleContext.h"
#include "System/Core/Debug/DebugDrawingSystem.h"

//-------------------------------------------------------------------------

namespace KRG::Navmesh
{
    bool EngineModule::Initialize( ModuleContext& context )
    {
        m_navmeshSystem.Initialize();

        //-------------------------------------------------------------------------

        context.RegisterResourceLoader( &m_navmeshLoader );

        //-------------------------------------------------------------------------

        context.RegisterSystem( m_navmeshSystem );
        context.RegisterGlobalSystem( &m_navmeshSystem );

        //-------------------------------------------------------------------------

        #if KRG_DEBUG_INSTRUMENTATION
        m_navmeshDebugViewController.Initialize();
        context.RegisterDebugView( &m_navmeshDebugViewController );
        #endif

        //-------------------------------------------------------------------------

        m_initialized = true;
        return m_initialized;
    }

    void EngineModule::Shutdown( ModuleContext& context )
    {
        if ( m_initialized )
        {
            #if KRG_DEBUG_INSTRUMENTATION
            context.UnregisterDebugView( &m_navmeshDebugViewController );
            m_navmeshDebugViewController.Shutdown();
            #endif

            //-------------------------------------------------------------------------

            context.UnregisterGlobalSystem( &m_navmeshSystem );
            context.UnregisterSystem( m_navmeshSystem );

            //-------------------------------------------------------------------------

            context.UnregisterResourceLoader( &m_navmeshLoader );

            //-------------------------------------------------------------------------

            m_navmeshSystem.Shutdown();
        }

        //-------------------------------------------------------------------------

        m_initialized = false;
    }
}
