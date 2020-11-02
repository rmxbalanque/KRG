#include "Module.h"
#include "Engine/Core/Modules/EngineModuleContext.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    bool EngineModule::Initialize( ModuleContext& context )
    {
        m_physicsSystem.Initialize();

        //-------------------------------------------------------------------------

        m_physicsGeometryLoader.SetPhysics( m_physicsSystem.GetPhysics() );
        context.RegisterResourceLoader( &m_physicsGeometryLoader );

        //-------------------------------------------------------------------------

        context.RegisterSystem( m_physicsSystem );
        context.RegisterGlobalSystem( &m_physicsSystem );

        #if KRG_DEBUG_INSTRUMENTATION
        m_physicsRenderer.Initialize( context.GetRenderDevice(), &m_physicsSystem );
        context.RegisterRenderer( &m_physicsRenderer );

        m_physicsDebugViewController.Initialize( &m_physicsSystem );
        context.RegisterDebugView( &m_physicsDebugViewController );
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
            context.UnregisterDebugView( &m_physicsDebugViewController );
            m_physicsDebugViewController.Shutdown( &m_physicsSystem );

            context.UnregisterRenderer( &m_physicsRenderer );
            m_physicsRenderer.Shutdown();
            #endif

            context.UnregisterGlobalSystem( &m_physicsSystem );
            context.UnregisterSystem( m_physicsSystem );

            //-------------------------------------------------------------------------

            context.UnregisterResourceLoader( &m_physicsGeometryLoader );
            m_physicsGeometryLoader.ClearPhysics();

            //-------------------------------------------------------------------------

            m_physicsSystem.Shutdown();
        }

        //-------------------------------------------------------------------------

        m_initialized = false;
    }
}
