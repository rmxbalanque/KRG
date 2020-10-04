#include "Module.h"
#include "Engine/Core/Modules/EngineModuleContext.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Engine
    {
        bool EngineModule::Initialize( ModuleContext& context )
        {
            m_mapLoader.SetTypeRegistry( context.GetTypeRegistry() );
            context.RegisterResourceLoader( &m_mapLoader );

            m_imguiRenderer.Initialize( context.GetRenderDevice() );
            context.RegisterRenderer( &m_imguiRenderer );

            //-------------------------------------------------------------------------

            #if KRG_DEBUG_INSTRUMENTATION
            m_cameraDebugViewController.Initialize( context.GetSystem<CameraSystem>() );
            m_entityDebugViewController.Initialize( context.GetWorld() );

            context.RegisterDebugView( &m_systemDebugViewController );
            context.RegisterDebugView( &m_entityDebugViewController );
            context.RegisterDebugView( &m_inputDebugViewController );
            context.RegisterDebugView( &m_cameraDebugViewController );
            #endif

            //-------------------------------------------------------------------------

            m_initialized = true;
            return m_initialized;
        }

        void EngineModule::Shutdown( ModuleContext& context )
        {
            //-------------------------------------------------------------------------
            // Unregistration
            //-------------------------------------------------------------------------
            // We only register if we initialize successfully

            if( m_initialized )
            {
                #if KRG_DEBUG_INSTRUMENTATION
                context.UnregisterDebugView( &m_cameraDebugViewController );
                context.UnregisterDebugView( &m_inputDebugViewController );
                context.UnregisterDebugView( &m_entityDebugViewController );
                context.UnregisterDebugView( &m_systemDebugViewController );

                m_entityDebugViewController.Shutdown();
                m_cameraDebugViewController.Shutdown();
                #endif

                //-------------------------------------------------------------------------

                context.UnregisterRenderer( &m_imguiRenderer );
                m_imguiRenderer.Shutdown();

                context.UnregisterResourceLoader( &m_mapLoader );
            }

            m_initialized = false;
        }
    }
}