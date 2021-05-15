#include "Module.h"
#include "Engine/Core/Modules/EngineModuleContext.h"

//-------------------------------------------------------------------------

namespace KRG::Navmesh
{
    bool EngineModule::Initialize( ModuleContext& context )
    {
        auto pDebugDrawingSystem = context.GetSystem<Debug::DrawingSystem>();
        KRG_ASSERT( pDebugDrawingSystem != nullptr );
        m_navmeshSystem.Initialize( pDebugDrawingSystem );

        //-------------------------------------------------------------------------

        context.RegisterResourceLoader( &m_navmeshLoader );

        //-------------------------------------------------------------------------

        context.RegisterSystem( m_navmeshSystem );
        context.RegisterWorldSystem( &m_navmeshWorldSystem );

        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
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
            #if KRG_DEVELOPMENT_TOOLS
            context.UnregisterDebugView( &m_navmeshDebugViewController );
            m_navmeshDebugViewController.Shutdown();
            #endif

            //-------------------------------------------------------------------------

            context.UnregisterWorldSystem( &m_navmeshWorldSystem );
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
