#include "Module.h"
#include "Engine/Core/Modules/EngineModuleContext.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Physics
    {
        bool EngineModule::Initialize( ModuleContext& context )
        {
            m_physicsWorld.Initialize();

            //-------------------------------------------------------------------------

            context.RegisterResourceLoader( &m_physicsGeometryLoader );

            //-------------------------------------------------------------------------

            context.RegisterSystem( m_physicsWorld );
            context.RegisterGlobalSystem( &m_physicsWorld );

            #if KRG_DEBUG_INSTRUMENTATION
            m_physicsRenderer.Initialize( context.GetRenderDevice(), &m_physicsWorld );
            context.RegisterRenderer( &m_physicsRenderer );

            m_physicsDebugViewController.Initialize( &m_physicsWorld );
            context.RegisterDebugView( &m_physicsDebugViewController );
            #endif

            //-------------------------------------------------------------------------

            m_initialized = true;
            return m_initialized;
        }

        void EngineModule::Shutdown( ModuleContext& context )
        {
            if( m_initialized )
            {
                #if KRG_DEBUG_INSTRUMENTATION
                context.UnregisterDebugView( &m_physicsDebugViewController );
                m_physicsDebugViewController.Shutdown( &m_physicsWorld );

                context.UnregisterRenderer( &m_physicsRenderer );
                m_physicsRenderer.Shutdown();
                #endif

                context.UnregisterGlobalSystem( &m_physicsWorld );
                context.UnregisterSystem( m_physicsWorld );

                //-------------------------------------------------------------------------

                context.UnregisterResourceLoader( &m_physicsGeometryLoader );

                //-------------------------------------------------------------------------

                m_physicsWorld.Shutdown();
            }

            //-------------------------------------------------------------------------

            m_initialized = false;
        }
    }
}
